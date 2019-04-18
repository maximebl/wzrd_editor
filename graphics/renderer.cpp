#include "pch.h"
#include "renderer.h"

ID3D12Device* winrt::graphics::implementation::renderer::g_device = nullptr;
ID3D12GraphicsCommandList* winrt::graphics::implementation::renderer::g_cmd_list = nullptr;

namespace winrt::graphics::implementation
{
	Windows::Foundation::IAsyncOperation<graphics::operation_result>
		renderer::pick_and_compile_shader(graphics::shader new_shader)
	{
		m_ui_thread = winrt::apartment_context();

		graphics::operation_result result;

		Windows::Storage::Pickers::FileOpenPicker picker;
		picker.FileTypeFilter().Append(hstring(L".hlsl"));
		auto file = co_await picker.PickSingleFileAsync();

		if (file == nullptr)
		{
			result.status = graphics::operation_status::cancelled;
			return result;
		}

		auto shader_file_buffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto shader_file_bytes = co_await os_utilities::read_file_bytes(shader_file_buffer);
		auto shader_file_name = file.Name();
		new_shader.file_name(shader_file_name);

		co_await winrt::resume_background();

		UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		com_ptr<ID3DBlob> byte_code = nullptr;
		com_ptr<ID3DBlob> errors = nullptr;

		std::string entry_point{};
		std::string version{};

		switch (new_shader.shader_type())
		{
		case graphics::shader_type::pixel:
			entry_point = "PS";
			version = "ps_5_0";
			break;

		case graphics::shader_type::vertex:
			entry_point = "VS";
			version = "vs_5_0";
			break;

		case graphics::shader_type::geometry:
			entry_point = "GS";
			version = "gs_5_0";
			break;

		default:
			break;
		}

		D3DCompile(
			&shader_file_bytes.front(),
			shader_file_bytes.size(),
			nullptr,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entry_point.c_str(),
			version.c_str(),
			compile_flags,
			0,
			byte_code.put(),
			errors.put()
		);

		if (errors != nullptr)
		{
			auto error_msg_ptr = static_cast<const char*>(errors->GetBufferPointer());
			hstring message = to_hstring(error_msg_ptr);

			result.status = graphics::operation_status::error;
			result.error_message = message;
			return result;
		}

		result.status = graphics::operation_status::success;
		m_shaders[new_shader.shader_name()] = byte_code;

		co_await m_ui_thread;

		auto impl_shader = new_shader.as<implementation::shader>();
		impl_shader->byte_code(byte_code);
		reflect_shader(impl_shader);

		return result;
	}

	graphics::primitive_types renderer::current_topology()
	{
		return m_current_topology;
	}

	void renderer::current_topology(graphics::primitive_types const& value)
	{
		m_current_topology = value;
	}

	int32_t renderer::viewport_width()
	{
		return m_output_width;
	}

	void renderer::viewport_width(int32_t value)
	{
		update_value(L"viewport_width", m_output_width, value);
	}

	int32_t renderer::viewport_height()
	{
		return m_output_height;
	}

	void renderer::viewport_height(int32_t value)
	{
		update_value(L"viewport_height", m_output_height, value);
	}

	renderer::renderer()
	{
	}

	void renderer::enable_debug_layer()
	{
		com_ptr<ID3D12Debug1> debug_controller;
		check_hresult(
			D3D12GetDebugInterface(winrt::guid_of<ID3D12Debug1>(), debug_controller.put_void())
		);
		debug_controller->EnableDebugLayer();
		debug_controller->SetEnableGPUBasedValidation(true);
	}

	void renderer::initialize_buffers_showcase(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain)
	{
		create_factory();
		create_device();
		create_fence();
		create_cmd_objects();
		create_dsv_heap();
		create_rtv_heap();
		create_depthstencil_buffer();
		create_swapchain_xaml(target_swapchain);
		create_render_targets();
		create_empty_rootsignature(get_static_samplers());
	}

	void renderer::initialize_textures_showcase(
		Windows::Foundation::Collections::IMap<hstring,
		Windows::Foundation::IInspectable> const& ui_items
	)
	{
		auto boxed_swapchain_panel = ui_items.Lookup(hstring{ L"swapchain_panel" });
		m_swapchain_panel = unbox_value<Windows::UI::Xaml::Controls::SwapChainPanel>(boxed_swapchain_panel);

		create_factory();
		create_device();
		create_fence();
		create_cmd_objects();
		create_dsv_heap();
		create_rtv_heap();
		create_srv_heap();
		create_sampler_heap();
		create_depthstencil_buffer();
		create_swapchain_xaml(m_swapchain_panel);
		create_render_targets();
		create_texture_rootsignature(get_static_samplers());
		create_point();
		create_cb_texcoord();
		create_cb_billboard_pos();
		create_uav();
		create_texture_srv();
		//create_simple_triangle();
	}

	Windows::Foundation::IAsyncAction renderer::main_loop()
	{
		co_await winrt::resume_background();

		while (m_is_rendering)
		{
			render_2();
		}
	}

	void renderer::flush_cmd_queue()
	{
		m_cpu_fence++;
		check_hresult(
			m_cmd_queue->Signal(m_gpu_fence.get(), m_cpu_fence)
		);

		if (m_gpu_fence->GetCompletedValue() < m_cpu_fence)
		{
			HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

			check_hresult(m_gpu_fence->SetEventOnCompletion(m_cpu_fence, event_handle));
			WaitForSingleObject(event_handle, INFINITE);
			CloseHandle(event_handle);
		}
	}

	void renderer::render_1()
	{
		check_hresult(m_cmd_allocator->Reset());
		check_hresult(m_graphics_cmdlist->Reset(m_cmd_allocator.get(), m_points_pso.get()));

		m_screen_viewport.TopLeftX = 0;
		m_screen_viewport.TopLeftY = 0;
		m_screen_viewport.Width = static_cast<float>(m_output_width);
		m_screen_viewport.Height = static_cast<float>(m_output_height);
		m_screen_viewport.MinDepth = 0.0f;
		m_screen_viewport.MaxDepth = 1.0f;
		m_scissor_rect = { 0, 0, m_output_width, m_output_height };

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_swapchain_buffer[m_current_backbuffer].get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_depthstencil_buffer.get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

		select_primitive_topology();

		m_graphics_cmdlist->RSSetViewports(1, &m_screen_viewport);
		m_graphics_cmdlist->RSSetScissorRects(1, &m_scissor_rect);
		m_graphics_cmdlist->ClearRenderTargetView(current_backbuffer_view(), DirectX::Colors::Beige, 0, nullptr);
		m_graphics_cmdlist->ClearDepthStencilView(m_dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		m_graphics_cmdlist->OMSetRenderTargets(1, &current_backbuffer_view(), true, &m_dsv_heap->GetCPUDescriptorHandleForHeapStart());

		m_graphics_cmdlist->SetGraphicsRootSignature(m_rootsig.get());

		m_graphics_cmdlist->IASetVertexBuffers(0, 1, (D3D12_VERTEX_BUFFER_VIEW*)& m_current_buffer->get_view());
		m_graphics_cmdlist->DrawInstanced(m_current_buffer->current_size(), 1, 0, 0);

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_swapchain_buffer[m_current_backbuffer].get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));
		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_depthstencil_buffer.get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_COMMON));

		execute_cmd_list();
		flush_cmd_queue();

		check_hresult(m_swapchain->Present(0, 0));
		m_current_backbuffer = (m_current_backbuffer + 1) % m_swapchain_buffer_count;
	}

	struct gs_box
	{
		DirectX::XMFLOAT2 topleft;
		DirectX::XMFLOAT2 topright;
		DirectX::XMFLOAT2 bottomleft;
		DirectX::XMFLOAT2 bottomright;
	};

	void renderer::update_samplers()
	{
		auto current_border_color = m_textures[L"default_texture"].sampler_border_color();
		auto r = static_cast<float>(current_border_color.R / 255.f);
		auto g = static_cast<float>(current_border_color.G / 255.f);
		auto b = static_cast<float>(current_border_color.B / 255.f);
		auto a = static_cast<float>(current_border_color.A / 255.f);
		float border_color[4] = { r,g,b,a };

		memcpy(m_sampler_desc.BorderColor, border_color, sizeof(FLOAT) * 4);
		m_sampler_desc.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(m_textures[L"default_texture"].u_address_mode());
		m_sampler_desc.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(m_textures[L"default_texture"].v_address_mode());
		m_sampler_desc.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(m_textures[L"default_texture"].w_address_mode());
		m_sampler_desc.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(m_textures[L"default_texture"].comparison_function());
		m_sampler_desc.MaxAnisotropy = static_cast<UINT>(m_textures[L"default_texture"].max_anisotropy());
		m_sampler_desc.Filter = select_filter();
		m_sampler_desc.MinLOD = m_textures[L"default_texture"].sampler_minLOD();
		m_sampler_desc.MaxLOD = m_textures[L"default_texture"].sampler_maxLOD();
		m_sampler_desc.MipLODBias = 0;

		m_device->CreateSampler(&m_sampler_desc, m_sampler_heap->GetCPUDescriptorHandleForHeapStart());

		//comparison sampler
		D3D12_CPU_DESCRIPTOR_HANDLE cmp_sampler_descriptor_handle = {};
		auto sampler_heap_descriptor_handle_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		cmp_sampler_descriptor_handle.ptr = m_sampler_heap->GetCPUDescriptorHandleForHeapStart().ptr + sampler_heap_descriptor_handle_size;

		m_device->CreateSampler(&m_sampler_desc, cmp_sampler_descriptor_handle);
	}

	D3D12_FILTER renderer::select_filter()
	{
		D3D12_FILTER_REDUCTION_TYPE reduction_type = static_cast<D3D12_FILTER_REDUCTION_TYPE>(m_textures[L"default_texture"].reduction());
		D3D12_FILTER_REDUCTION_TYPE is_anisotropic = static_cast<D3D12_FILTER_REDUCTION_TYPE>(m_textures[L"default_texture"].is_anisotropic());

		D3D12_FILTER result;

		if (is_anisotropic)
		{
			result = D3D12_ENCODE_ANISOTROPIC_FILTER(reduction_type);
		}
		else
		{
			D3D12_FILTER_TYPE minification_filter = static_cast<D3D12_FILTER_TYPE>(m_textures[L"default_texture"].minification_filter());
			D3D12_FILTER_TYPE magnification_filter = static_cast<D3D12_FILTER_TYPE>(m_textures[L"default_texture"].magnification_filter());
			D3D12_FILTER_TYPE miplevel_filter = static_cast<D3D12_FILTER_TYPE>(m_textures[L"default_texture"].miplevel_sampling_filter());

			result = D3D12_ENCODE_BASIC_FILTER(
				minification_filter,
				magnification_filter,
				miplevel_filter,
				reduction_type);
		}

		return result;
	}

	void renderer::render_2()
	{
		gs_box texcoords;

		texcoords.topleft.x = m_textures[L"default_texture"].topleft_u();
		texcoords.topleft.y = m_textures[L"default_texture"].topleft_v();

		texcoords.topright.x = m_textures[L"default_texture"].topright_u();
		texcoords.topright.y = m_textures[L"default_texture"].topright_v();

		texcoords.bottomleft.x = m_textures[L"default_texture"].bottomleft_u();
		texcoords.bottomleft.y = m_textures[L"default_texture"].bottomleft_v();

		texcoords.bottomright.x = m_textures[L"default_texture"].bottomright_u();
		texcoords.bottomright.y = m_textures[L"default_texture"].bottomright_v();

		std::memcpy(
			reinterpret_cast<void*>(m_mapped_texcoord_data),
			reinterpret_cast<void*>(&texcoords),
			sizeof(gs_box));

		gs_box vertex_positions;

		vertex_positions.topleft.x = m_textures[L"default_texture"].topleft_x();
		vertex_positions.topleft.y = m_textures[L"default_texture"].topleft_y();

		vertex_positions.topright.x = m_textures[L"default_texture"].topright_x();
		vertex_positions.topright.y = m_textures[L"default_texture"].topright_y();

		vertex_positions.bottomleft.x = m_textures[L"default_texture"].bottomleft_x();
		vertex_positions.bottomleft.y = m_textures[L"default_texture"].bottomleft_y();

		vertex_positions.bottomright.x = m_textures[L"default_texture"].bottomright_x();
		vertex_positions.bottomright.y = m_textures[L"default_texture"].bottomright_y();

		std::memcpy(
			reinterpret_cast<void*>(m_mapped_position_data),
			reinterpret_cast<void*>(&vertex_positions),
			sizeof(gs_box));

		check_hresult(m_cmd_allocator->Reset());
		check_hresult(m_graphics_cmdlist->Reset(m_cmd_allocator.get(), m_billboard_pso.get()));

		m_screen_viewport.TopLeftX = 0;
		m_screen_viewport.TopLeftY = 0;
		m_screen_viewport.Width = static_cast<float>(m_output_width);
		m_screen_viewport.Height = static_cast<float>(m_output_height);
		m_screen_viewport.MinDepth = 0.0f;
		m_screen_viewport.MaxDepth = 1.0f;
		m_scissor_rect = { 0, 0, m_output_width, m_output_height };

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_swapchain_buffer[m_current_backbuffer].get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_depthstencil_buffer.get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));

		m_graphics_cmdlist->RSSetViewports(1, &m_screen_viewport);
		m_graphics_cmdlist->RSSetScissorRects(1, &m_scissor_rect);
		m_graphics_cmdlist->ClearRenderTargetView(current_backbuffer_view(), DirectX::Colors::Brown, 0, nullptr);
		m_graphics_cmdlist->ClearDepthStencilView(m_dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		m_graphics_cmdlist->OMSetRenderTargets(1, &current_backbuffer_view(), true, &m_dsv_heap->GetCPUDescriptorHandleForHeapStart());

		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		m_graphics_cmdlist->SetGraphicsRootSignature(m_rootsig.get());
		m_graphics_cmdlist->SetGraphicsRootConstantBufferView(1, m_cb_texcoord_upload_buffer->GetGPUVirtualAddress());
		m_graphics_cmdlist->SetGraphicsRootConstantBufferView(3, m_cb_position_upload_buffer->GetGPUVirtualAddress());
		m_graphics_cmdlist->SetGraphicsRootUnorderedAccessView(4, m_uav_lod_default_buffer->GetGPUVirtualAddress());

		float new_scale = m_textures[L"default_texture"].scale();
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &new_scale, 0);

		uint32_t filter_reduction = static_cast<uint32_t>(m_textures[L"default_texture"].reduction());
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &filter_reduction, 1);

		float forced_miplevel = m_textures[L"default_texture"].forced_miplevel();
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &forced_miplevel, 2);

		float sample_comparison_value = m_textures[L"default_texture"].sample_comparison_value();
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &sample_comparison_value, 3);

		uint32_t is_forced_mip_level = static_cast<uint32_t>(m_textures[L"default_texture"].is_forced_mip_level());
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &is_forced_mip_level, 4);

		uint32_t sampling_function = static_cast<uint32_t>(m_textures[L"default_texture"].sampling_function());
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &sampling_function, 5);

		uint32_t is_screen_coords = static_cast<uint32_t>(m_textures[L"default_texture"].is_screen_coords());
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &is_screen_coords, 6);

		uint32_t x_pixel_offset = static_cast<int32_t>(m_textures[L"default_texture"].x_pixel_offset());
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &x_pixel_offset, 7);

		uint32_t y_pixel_offset = static_cast<int32_t>(m_textures[L"default_texture"].y_pixel_offset());
		m_graphics_cmdlist->SetGraphicsRoot32BitConstants(5, 1, &y_pixel_offset, 8);

		m_graphics_cmdlist->SetPipelineState(m_billboard_pso.get());

		update_samplers();

		ID3D12DescriptorHeap* heaps[] = { m_srv_heap.get(), m_sampler_heap.get() };
		m_graphics_cmdlist->SetDescriptorHeaps(_countof(heaps), heaps);
		m_graphics_cmdlist->SetGraphicsRootDescriptorTable(0, m_srv_heap->GetGPUDescriptorHandleForHeapStart());
		m_graphics_cmdlist->SetGraphicsRootDescriptorTable(2, m_sampler_heap->GetGPUDescriptorHandleForHeapStart());

		m_graphics_cmdlist->IASetVertexBuffers(0, 1, (D3D12_VERTEX_BUFFER_VIEW*)& m_current_buffer->get_view());
		m_graphics_cmdlist->DrawInstanced(m_current_buffer->current_size(), 1, 0, 0);

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_uav_lod_default_buffer.get(),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE
		));

		m_graphics_cmdlist->CopyResource(m_uav_lod_readback_buffer.get(), m_uav_lod_default_buffer.get());

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_uav_lod_default_buffer.get(),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON
		));

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_swapchain_buffer[m_current_backbuffer].get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));
		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_depthstencil_buffer.get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_COMMON));

		execute_cmd_list();
		flush_cmd_queue();

		D3D12_RANGE rb_range;
		rb_range.Begin = 0;
		rb_range.End = sizeof(shader_readback_data);
		void* pData = nullptr;
		check_hresult(
			m_uav_lod_readback_buffer->Map(0, &rb_range, &pData)
		);

		auto current_shader_data = *static_cast<shader_readback_data*>(pData);
		auto current_lod = current_shader_data.level_of_detail;
		update_ui(current_lod);

		D3D12_RANGE empty_range;
		empty_range.Begin = 0;
		empty_range.End = 0;
		m_uav_lod_readback_buffer->Unmap(0, &empty_range);

		check_hresult(m_swapchain->Present(0, 0));
		m_current_backbuffer = (m_current_backbuffer + 1) % m_swapchain_buffer_count;
	}

	winrt::event_token renderer::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const & handler)
	{
		return m_property_changed.add(handler);
	}

	void renderer::PropertyChanged(winrt::event_token const & token) noexcept
	{
		m_property_changed.remove(token);
	}

	void renderer::start_render_loop()
	{
		bool is_ready_to_render = !m_is_rendering && m_shaders.size() > 0;
		if (is_ready_to_render)
		{
			m_is_rendering = true;
			init_psos();
			execute_cmd_list();
			main_loop();
		}
	}

	void renderer::stop_render_loop()
	{
		m_is_rendering = false;
	}

	void renderer::clear_shaders()
	{
		m_shaders.clear();
	}

	void renderer::clear_textures()
	{
		m_textures.clear();
	}

	void renderer::remove_texture(winrt::hstring name)
	{
		m_textures.erase(name);
	}

	void renderer::remove_shader(winrt::hstring name)
	{
		m_shaders.erase(name);
	}

	bool renderer::is_rendering()
	{
		return m_is_rendering;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE renderer::current_backbuffer_view() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_rtv_heap->GetCPUDescriptorHandleForHeapStart(),
			m_current_backbuffer,
			m_rtv_descriptor_size
		);
	}

	graphics::buffer renderer::current_buffer()
	{
		return m_current_buffer.as<graphics::buffer>();
	}

	void renderer::current_buffer(graphics::buffer const& value)
	{
		m_current_buffer = value.as<graphics::implementation::buffer>();
	}

	Windows::Foundation::IAsyncOperationWithProgress<graphics::operation_result, hstring> renderer::create_dds_textures(
		hstring name,
		uint64_t width,
		uint64_t height,
		graphics::alpha_mode const& alpha_mode,
		IObservableVector<graphics::texture>& new_textures)
	{
		using namespace DirectX;

		Image new_image;
		new_image.format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
		new_image.height = width;
		new_image.width = height;

		check_hresult(
			ComputePitch(new_image.format, new_image.width, new_image.height, new_image.rowPitch, new_image.slicePitch)
		);

		com_ptr<graphics::implementation::texture> new_texture = winrt::make_self<graphics::implementation::texture>();
		new_texture->texture_name(name);
		new_texture->alpha_mode(alpha_mode);
		//new_texture->mip_levels(loaded_image_md.mipLevels);
		new_texture->dimension(get_dimension(D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D));
		new_texture->width(new_image.width);
		new_texture->height(new_image.height);
		new_texture->row_pitch(new_image.rowPitch);
		new_texture->slice_pitch(new_image.slicePitch);

		Windows::Storage::Pickers::FileOpenPicker picker;
		picker.FileTypeFilter().Append(L".bmp");
		Windows::Storage::StorageFile file = co_await picker.PickSingleFileAsync();
		Windows::Storage::Streams::IRandomAccessStream stream = co_await file.OpenAsync(Windows::Storage::FileAccessMode::Read);
		Windows::Graphics::Imaging::BitmapDecoder decoder = co_await Windows::Graphics::Imaging::BitmapDecoder::CreateAsync(stream);
		auto pixel_data_provider = co_await decoder.GetPixelDataAsync();
		new_image.pixels = pixel_data_provider.DetachPixelData().data();

		Blob new_blob;
		check_hresult(SaveToDDSMemory(new_image, DDS_FLAGS::DDS_FLAGS_FORCE_DX10_EXT, new_blob));

		// debugging
		auto folder = Windows::Storage::ApplicationData::Current().LocalFolder().Path();
		auto file_dest = folder + L"\\new_tex.dds";
		check_hresult(SaveToDDSFile(new_image, DDS_FLAGS::DDS_FLAGS_NONE, file_dest.data()));
		// TODO: use the other overload that takes an array of images and metadata

		//std::unique_ptr<TexMetadata> metadata = nullptr;
		//ScratchImage scratch;
		//DirectX::LoadFromDDSMemory(new_blob.GetBufferPointer(), new_blob.GetBufferSize(), DDS_FLAGS::DDS_FLAGS_FORCE_DX10_EXT, metadata.get(), scratch);
		//TexMetadata loaded_image_md = scratch.GetMetadata();

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		check_hresult(
			LoadDDSTextureFromMemoryEx(
				g_device,
				reinterpret_cast<uint8_t*>(new_blob.GetBufferPointer()),
				new_blob.GetBufferSize(),
				sizeof(size_t),
				D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
				DDS_LOADER_FLAGS::DDS_LOADER_DEFAULT,
				new_texture->texture_default_buffer.put(),
				subresources,
				&static_cast<DDS_ALPHA_MODE>(alpha_mode),
				false
			));

		co_await upload_to_gpu(new_texture.as<graphics::texture>(), subresources, new_image.format);

		m_textures[name] = new_texture.as<graphics::texture>();

		graphics::operation_result result;
		result.status = operation_status::success;
		co_return result;
	}

	Windows::Foundation::IAsyncOperationWithProgress<graphics::operation_result, hstring> renderer::pick_texture(graphics::texture& new_texture, hstring name)
	{
		using namespace Windows::Graphics::Imaging;

		graphics::operation_result result;

		auto progress = co_await winrt::get_progress_token();

		Windows::Storage::Pickers::FileOpenPicker picker;
		picker.FileTypeFilter().Append(hstring(L".dds"));

		Windows::Storage::StorageFile file = co_await picker.PickSingleFileAsync();

		if (file == nullptr)
		{
			result.status = operation_status::cancelled;
			result.error_message = L"The user closed the file picker.";
			co_return result;
		}

		new_texture = graphics::texture();
		new_texture.file_name(file.Name());
		new_texture.texture_name(name);

		progress(L"Reading image file bytes");
		auto texture_file_buffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto texture_file_bytes = co_await os_utilities::read_file_bytes(texture_file_buffer);

		uint32_t sizeof_dds_magic_number = sizeof(uint32_t);
		unsigned char* dds_data = texture_file_bytes.data();
		// DDS files start with a section called dwMagicNumber of size DWORD, which is 32bits, followed by a DDS_HEADER section.
		// That means we have to offset our pointer to the beginning of the file by 32 bits in order to access the DDS_HEADER section of the DDS file.
		DDS_HEADER* dds_header = reinterpret_cast<DDS_HEADER*>(dds_data + sizeof_dds_magic_number);

		// We now offset to the beginning of the extended header, which is positioned after the DDS_HEADER
		DDS_HEADER_DXT10* extended_header = reinterpret_cast<DDS_HEADER_DXT10*>(dds_data + sizeof_dds_magic_number + dds_header->size);

		bool is_dx10_extended = dds_header->ddspf.fourCC == MAKEFOURCC('D', 'X', '1', '0');
		if (!is_dx10_extended)
		{
			result.status = operation_status::error;
			result.error_message = L"The file " + file.Name() + L" does not contain a DDS_HEADER_DXT10 extended header.";
			co_return result;
		}

		auto new_alpha_mode = static_cast<DDS_ALPHA_MODE>(extended_header->miscFlags2 & 0x7L);

		new_texture.alpha_mode(static_cast<graphics::alpha_mode>(new_alpha_mode));
		new_texture.mip_levels(dds_header->mipMapCount);
		new_texture.width(dds_header->width);
		new_texture.height(dds_header->height);
		new_texture.dimension(get_dimension(extended_header->resourceDimension));

		auto pixel_size = 4;
		auto row_pitch = dds_header->width * pixel_size;
		auto slice_pitch = row_pitch * dds_header->height;

		new_texture.row_pitch(row_pitch);
		new_texture.slice_pitch(slice_pitch);

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		winrt::check_hresult(
			DirectX::LoadDDSTextureFromMemory(
				g_device,
				&texture_file_bytes.front(),
				texture_file_bytes.size(),
				new_texture.as<graphics::implementation::texture>()->texture_default_buffer.put(),
				subresources
			)
		);

		progress(L"Uploading " + name + L" to the GPU");
		co_await upload_to_gpu(new_texture, subresources, extended_header->dxgiFormat);

		progress(L"Generating bitmaps from mipmaps");
		auto new_mipmaps = single_threaded_observable_vector<IInspectable>();
		co_await create_subresources_for_ui(subresources, new_mipmaps, extended_header->dxgiFormat, dds_header->width);
		new_texture.mipmaps(new_mipmaps);

		Windows::Storage::Streams::IRandomAccessStream stream;
		stream = co_await file.OpenAsync(Windows::Storage::FileAccessMode::Read);
		auto decoder = co_await BitmapDecoder::CreateAsync(stream);

		SoftwareBitmap new_software_bitmap = nullptr;
		new_software_bitmap = co_await decoder.GetSoftwareBitmapAsync();
		new_software_bitmap = SoftwareBitmap::Convert(new_software_bitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);

		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource new_bitmap_source;
		co_await new_bitmap_source.SetBitmapAsync(new_software_bitmap);

		new_texture.bitmap_source(new_bitmap_source);

		m_textures[name] = new_texture;
		result.status = operation_status::success;
		result.error_message = L"";

		co_return result;
	}

	Windows::Foundation::IAsyncAction renderer::upload_to_gpu(graphics::texture& texture, std::vector<D3D12_SUBRESOURCE_DATA> subresources, DXGI_FORMAT texture_format)
	{
		D3D12_HEAP_PROPERTIES heap_props;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		heap_props.VisibleNodeMask = 0;
		heap_props.CreationNodeMask = 0;

		auto required_size = GetRequiredIntermediateSize(texture.as<graphics::implementation::texture>()->texture_default_buffer.get(), 0, subresources.size());

		winrt::check_hresult(
			m_device->CreateCommittedResource(
				&heap_props,
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(required_size),
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				winrt::guid_of<ID3D12Resource>(),
				intermediate_upload_resource.put_void()
			)
		);

		UpdateSubresources(g_cmd_list, texture.as<graphics::implementation::texture>()->texture_default_buffer.get(), intermediate_upload_resource.get(), 0, 0, subresources.size(), subresources.data());

		texture.as<graphics::implementation::texture>()->texture_upload_buffer = intermediate_upload_resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = texture.mip_levels();
		srv_desc.Format = texture_format;

		m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			texture.as<graphics::implementation::texture>()->texture_default_buffer.get(),
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		));

		m_device->CreateShaderResourceView(
			texture.as<graphics::implementation::texture>()->texture_default_buffer.get(),
			&srv_desc,
			m_srv_heap->GetCPUDescriptorHandleForHeapStart());

		co_return;
	}

	hstring renderer::get_dimension(D3D12_RESOURCE_DIMENSION dimension)
	{
		hstring new_dimension;
		switch (dimension)
		{
		case D3D12_RESOURCE_DIMENSION_UNKNOWN:
			new_dimension = L"Unknown";
			break;
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			new_dimension = L"Buffer";
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			new_dimension = L"Texture 1D";
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			new_dimension = L"Texture 2D";
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			new_dimension = L"Texture 3D";
			break;
		default:
			new_dimension = L"Unknown";
			break;
		}
		return new_dimension;
	}

	Windows::Foundation::IAsyncAction renderer::update_ui(float new_val)
	{
		co_await m_ui_thread;
		m_textures[L"default_texture"].current_lod(new_val);
	}

	void renderer::create_factory()
	{
		check_hresult(
			CreateDXGIFactory1(guid_of<IDXGIFactory4>(), m_dxgi_factory.put_void())
		);
	}

	void renderer::create_device()
	{
		check_hresult(
			D3D12CreateDevice(
				nullptr,
				D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
				guid_of<ID3D12Device>(),
				m_device.put_void())
		);
		g_device = m_device.get();
	}

	void renderer::create_fence()
	{
		check_hresult(
			m_device->CreateFence(
				0,
				D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE,
				guid_of<ID3D12Fence>(),
				m_gpu_fence.put_void())
		);
	}

	void renderer::create_cmd_objects()
	{
		D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
		cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

		check_hresult(
			m_device->CreateCommandQueue(&cmd_queue_desc, guid_of<ID3D12CommandQueue>(), m_cmd_queue.put_void())
		);

		check_hresult(
			m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, guid_of<ID3D12CommandAllocator>(), m_cmd_allocator.put_void())
		);

		check_hresult(
			m_device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_cmd_allocator.get(),
				nullptr,
				guid_of<ID3D12GraphicsCommandList>(),
				m_graphics_cmdlist.put_void())
		);
		g_cmd_list = m_graphics_cmdlist.get();
	}

	void renderer::create_dsv_heap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
		dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsv_heap_desc.NodeMask = 0;
		dsv_heap_desc.NumDescriptors = 1;
		dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		check_hresult(
			m_device->CreateDescriptorHeap(&dsv_heap_desc, guid_of<ID3D12DescriptorHeap>(), m_dsv_heap.put_void())
		);
	}

	void renderer::create_rtv_heap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtv_heap_desc.NodeMask = 0;
		rtv_heap_desc.NumDescriptors = m_swapchain_buffer_count;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		check_hresult(
			m_device->CreateDescriptorHeap(&rtv_heap_desc, guid_of<ID3D12DescriptorHeap>(), m_rtv_heap.put_void())
		);
	}

	void renderer::create_srv_heap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
		srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srv_heap_desc.NodeMask = 0;
		srv_heap_desc.NumDescriptors = 2;
		srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		check_hresult(
			m_device->CreateDescriptorHeap(&srv_heap_desc, guid_of<ID3D12DescriptorHeap>(), m_srv_heap.put_void())
		);
	}

	void renderer::create_sampler_heap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC sampler_heap_desc = {};
		sampler_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		sampler_heap_desc.NodeMask = 0;
		sampler_heap_desc.NumDescriptors = 2;
		sampler_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

		check_hresult(
			m_device->CreateDescriptorHeap(&sampler_heap_desc, guid_of<ID3D12DescriptorHeap>(), m_sampler_heap.put_void())
		);
	}

	void renderer::create_depthstencil_buffer()
	{
		D3D12_RESOURCE_DESC depth_stencil_desc = {};
		depth_stencil_desc.Alignment = 0;
		depth_stencil_desc.DepthOrArraySize = 1;
		depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depth_stencil_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depth_stencil_desc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		depth_stencil_desc.Height = m_output_height;
		depth_stencil_desc.Width = m_output_width;
		depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depth_stencil_desc.MipLevels = 1;
		depth_stencil_desc.SampleDesc.Count = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;

		D3D12_CLEAR_VALUE clear_value;
		clear_value.DepthStencil.Depth = 1.0f;
		clear_value.DepthStencil.Stencil = 0;
		clear_value.Format = m_depthstencil_format;

		winrt::check_hresult(
			m_device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&depth_stencil_desc,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
				&clear_value,
				guid_of<ID3D12Resource>(),
				m_depthstencil_buffer.put_void()
			)
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
		dsv_desc.Flags = D3D12_DSV_FLAGS::D3D12_DSV_FLAG_NONE;
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Format = m_depthstencil_format;
		dsv_desc.Texture2D.MipSlice = 0;

		m_device->CreateDepthStencilView(
			m_depthstencil_buffer.get(),
			&dsv_desc,
			m_dsv_heap->GetCPUDescriptorHandleForHeapStart()
		);
	}

	void renderer::create_swapchain_xaml(Windows::UI::Xaml::Controls::SwapChainPanel target_swapchain)
	{
		if (m_swapchain == nullptr)
		{
			DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
			swapchain_desc.BufferCount = m_swapchain_buffer_count; // triple buffering
			swapchain_desc.Width = m_output_width;
			swapchain_desc.Height = m_output_height;
			swapchain_desc.Format = m_backbuffer_format;
			swapchain_desc.Stereo = false;
			swapchain_desc.SampleDesc.Count = 1;
			swapchain_desc.SampleDesc.Quality = 0;
			swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapchain_desc.Flags = 0;
			swapchain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
			swapchain_desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE;

			winrt::check_hresult(
				m_dxgi_factory->CreateSwapChainForComposition(
					m_cmd_queue.get(),
					&swapchain_desc,
					nullptr,
					m_swapchain.put()
				));

			// associate DXGI swap chain with the XAML SwapChainPanel
			check_hresult(
				target_swapchain.as<ISwapChainPanelNative>()->SetSwapChain(m_swapchain.get())
			);
		}
	}

	void renderer::create_render_targets()
	{
		m_rtv_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < m_swapchain_buffer_count; i++)
		{
			check_hresult(
				m_swapchain->GetBuffer(i, guid_of<ID3D12Resource>(), m_swapchain_buffer[i].put_void())
			);
			m_device->CreateRenderTargetView(m_swapchain_buffer[i].get(), nullptr, rtv_heap_handle);
			rtv_heap_handle.Offset(1, m_rtv_descriptor_size);
		}
	}

	void renderer::create_texture_rootsignature(std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers)
	{
		//recreate the texture's range
		D3D12_DESCRIPTOR_RANGE textures_range = {};
		textures_range.BaseShaderRegister = 0;
		textures_range.NumDescriptors = 2;
		textures_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		textures_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		textures_range.RegisterSpace = 0;

		D3D12_DESCRIPTOR_RANGE samplers_range = {};
		samplers_range.BaseShaderRegister = 0;
		samplers_range.NumDescriptors = 2;
		samplers_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		samplers_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		samplers_range.RegisterSpace = 0;

		CD3DX12_ROOT_PARAMETER root_parameter[6];
		root_parameter[0].InitAsDescriptorTable(1, &textures_range, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);
		root_parameter[1].InitAsConstantBufferView(0);
		root_parameter[2].InitAsDescriptorTable(1, &samplers_range, D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL);
		root_parameter[3].InitAsConstantBufferView(1);
		root_parameter[4].InitAsUnorderedAccessView(1);
		root_parameter[5].InitAsConstants(9, 2);

		CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc(
			6,
			root_parameter,
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		com_ptr<ID3DBlob> serialized_rootsig = nullptr;
		com_ptr<ID3DBlob> error_blob = nullptr;
		D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serialized_rootsig.put(), error_blob.put());

		if (error_blob != nullptr)
		{
			auto error_msg_ptr = static_cast<const char*>(error_blob->GetBufferPointer());
			winrt::hstring message = winrt::to_hstring(error_msg_ptr);
		}

		m_device->CreateRootSignature(
			0,
			serialized_rootsig->GetBufferPointer(),
			serialized_rootsig->GetBufferSize(),
			guid_of<ID3D12RootSignature>(),
			m_rootsig.put_void());
	}

	void renderer::create_point()
	{
		auto new_vertex = graphics::vertex(
			0.f, 0.f, 0.f,
			0.f, 0.f, 0.0f, 0.f,
			0.5f, 0.f
		);

		Windows::Foundation::Collections::IObservableVector<graphics::vertex> tmp_vec{ winrt::single_threaded_observable_vector<graphics::vertex>() };

		tmp_vec.Append(new_vertex);

		current_buffer(
			graphics::buffer(
				graphics::buffer_type::static_buffer,
				tmp_vec,
				tmp_vec.Size(),
				0,
				false
			));
	}

	void renderer::create_simple_triangle()
	{
		auto new_vertex1 = graphics::vertex(
			0.0f, 0.25f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.5f, 0.0f
		);

		auto new_vertex2 = graphics::vertex(
			0.25f, -0.25f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f
		);

		auto new_vertex3 = graphics::vertex(
			-0.25f, -0.25f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f
		);

		Windows::Foundation::Collections::IObservableVector<graphics::vertex> tmp_vec{ winrt::single_threaded_observable_vector<graphics::vertex>() };

		tmp_vec.Append(new_vertex1);
		tmp_vec.Append(new_vertex2);
		tmp_vec.Append(new_vertex3);

		current_buffer(
			graphics::buffer(
				graphics::buffer_type::static_buffer,
				tmp_vec,
				tmp_vec.Size(),
				0,
				false
			));
	}

	void renderer::create_texture_srv()
	{
		UINT texture_width = 512;
		UINT texture_height = 512;
		UINT texture_pixel_size = 4;

		D3D12_RESOURCE_DESC texture_desc = {};
		texture_desc.MipLevels = 1;
		texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_desc.Width = texture_width;
		texture_desc.Height = texture_height;
		texture_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		texture_desc.DepthOrArraySize = 1;
		texture_desc.SampleDesc.Count = 1;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		std::vector<UINT8> texture_data = generate_texture_data(texture_width, texture_height, texture_pixel_size);

		m_checkerboard_texture = utilities::create_static_texture_resource(
			renderer::g_device,
			renderer::g_cmd_list,
			texture_desc,
			texture_data.data(),
			texture_width,
			texture_height,
			texture_pixel_size,
			m_texture_upload_buffer);

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Format = texture_desc.Format;

		D3D12_CPU_DESCRIPTOR_HANDLE checkerboard_texture_descriptor_handle = {};
		auto texture_heap_descriptor_handle_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		checkerboard_texture_descriptor_handle.ptr = m_srv_heap->GetCPUDescriptorHandleForHeapStart().ptr + texture_heap_descriptor_handle_size;

		m_device->CreateShaderResourceView(m_checkerboard_texture.get(), &srv_desc, checkerboard_texture_descriptor_handle);
	}

	void renderer::create_cb_texcoord()
	{
		D3D12_HEAP_PROPERTIES heap_props = {};
		heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC cb_res_desc = {};
		cb_res_desc.Alignment = 0;
		cb_res_desc.DepthOrArraySize = 1;
		cb_res_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		cb_res_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		cb_res_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		cb_res_desc.Height = 1;
		cb_res_desc.Width = (sizeof(gs_box) + 255) & ~255;
		cb_res_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		cb_res_desc.MipLevels = 1;
		cb_res_desc.SampleDesc.Count = 1;
		cb_res_desc.SampleDesc.Quality = 0;

		check_hresult(
			m_device->CreateCommittedResource(
				&heap_props,
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&cb_res_desc,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				guid_of<ID3D12Resource>(),
				m_cb_texcoord_upload_buffer.put_void()
			)
		);

		m_cb_texcoord_upload_buffer->SetName(hstring{ L"cb_texcoord" }.c_str());
		m_cb_texcoord_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_texcoord_data));
	}

	void renderer::create_cb_billboard_pos()
	{
		D3D12_HEAP_PROPERTIES heap_props = {};
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.CreationNodeMask = 0;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC cb_res_desc = {};
		cb_res_desc.Alignment = 0;
		cb_res_desc.DepthOrArraySize = 1;
		cb_res_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		cb_res_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
		cb_res_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		cb_res_desc.Height = 1;
		auto req_size_for_cb = 255;
		auto size_in_bytes = sizeof(gs_box);
		auto biased_size = size_in_bytes + req_size_for_cb;
		auto inverted_cb_req_size = ~255;
		auto width = biased_size & inverted_cb_req_size;
		cb_res_desc.Width = width;
		cb_res_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		cb_res_desc.MipLevels = 1;
		cb_res_desc.SampleDesc.Count = 1;
		cb_res_desc.SampleDesc.Quality = 0;

		check_hresult(
			m_device->CreateCommittedResource(&heap_props,
				D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
				&cb_res_desc,
				D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				winrt::guid_of<ID3D12Resource>(),
				m_cb_position_upload_buffer.put_void()
			)
		);

		m_cb_position_upload_buffer->SetName(hstring{ L"cb_position" }.c_str());
		m_cb_position_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_position_data));
	}

	void renderer::create_uav()
	{
		{
			D3D12_HEAP_PROPERTIES heap_props;
			heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heap_props.CreationNodeMask = 0;
			heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
			heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
			heap_props.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC rb_res_desc = {};
			rb_res_desc.Alignment = 0;
			rb_res_desc.DepthOrArraySize = 1;
			rb_res_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
			rb_res_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
			rb_res_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			rb_res_desc.Height = 1;
			rb_res_desc.Width = sizeof(shader_readback_data);
			rb_res_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			rb_res_desc.MipLevels = 1;
			rb_res_desc.SampleDesc.Count = 1;
			rb_res_desc.SampleDesc.Quality = 0;


			check_hresult(
				m_device->CreateCommittedResource(
					&heap_props,
					D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
					&rb_res_desc,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					winrt::guid_of<ID3D12Resource>(),
					m_uav_lod_readback_buffer.put_void()
				));

			m_uav_lod_readback_buffer->SetName(hstring(L"uav_readback_buffer").c_str());
		}

		{
			D3D12_HEAP_PROPERTIES heap_props;
			heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heap_props.CreationNodeMask = 0;
			heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
			heap_props.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
			heap_props.VisibleNodeMask = 0;

			D3D12_RESOURCE_DESC upload_res_desc = {};
			upload_res_desc.Alignment = 0;
			upload_res_desc.DepthOrArraySize = 1;
			upload_res_desc.Dimension = D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
			upload_res_desc.Flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			upload_res_desc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			upload_res_desc.Height = 1;
			upload_res_desc.Width = sizeof(shader_readback_data);
			upload_res_desc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			upload_res_desc.MipLevels = 1;
			upload_res_desc.SampleDesc.Count = 1;
			upload_res_desc.SampleDesc.Quality = 0;

			check_hresult(
				m_device->CreateCommittedResource(
					&heap_props,
					D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
					&upload_res_desc,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					winrt::guid_of<ID3D12Resource>(),
					m_uav_lod_default_buffer.put_void()
				));

			m_uav_lod_default_buffer->SetName(hstring(L"uav_default_buffer").c_str());
		}
	}

	std::vector<UINT8> renderer::generate_texture_data(UINT texture_width, UINT texture_height, UINT texture_pixel_size)
	{
		const UINT rowPitch = texture_width * texture_pixel_size;
		const UINT cellPitch = rowPitch >> 3;		// The width of a cell in the checkboard texture.
		const UINT cellHeight = texture_width >> 3;	// The height of a cell in the checkerboard texture.
		const UINT textureSize = rowPitch * texture_height;

		std::vector<UINT8> data(textureSize);
		UINT8* pData = &data[0];

		for (UINT n = 0; n < textureSize; n += texture_pixel_size)
		{
			UINT x = n % rowPitch;
			UINT y = n / rowPitch;
			UINT i = x / cellPitch;
			UINT j = y / cellHeight;

			if (i % 2 == j % 2)
			{
				pData[n] = 0x00;		// R
				pData[n + 1] = 0x00;	// G
				pData[n + 2] = 0x00;	// B
				pData[n + 3] = 0xff;	// A
			}
			else
			{
				pData[n] = 0xff;		// R
				pData[n + 1] = 0xff;	// G
				pData[n + 2] = 0xff;	// B
				pData[n + 3] = 0xff;	// A
			}
		}

		return data;
	}

	Windows::Foundation::IAsyncAction renderer::create_subresources_for_ui(std::vector<D3D12_SUBRESOURCE_DATA>& original_mipmaps, IObservableVector<IInspectable>& ui_mipmaps, DXGI_FORMAT format, uint32_t texture_width)
	{
		using namespace Windows::UI::Xaml::Media::Imaging;
		using namespace Windows::Graphics::Imaging;
		using namespace Windows::Storage::Streams;

		for (int i = 0; i < original_mipmaps.size(); ++i)
		{
			int32_t slice_pitch = original_mipmaps[i].SlicePitch;
			int32_t row_pitch = original_mipmaps[i].RowPitch;
			int32_t subresource_width = max(1, texture_width >> i);

			DirectX::Image bc_image;
			bc_image.format = format;
			bc_image.height = subresource_width;
			bc_image.width = subresource_width;
			bc_image.pixels = (uint8_t*)original_mipmaps[i].pData;
			bc_image.rowPitch = row_pitch;
			bc_image.slicePitch = slice_pitch;

			DirectX::ScratchImage decompressed_images;
			check_hresult(DirectX::Decompress(bc_image, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, decompressed_images));
			const DirectX::Image* decompressed_image = decompressed_images.GetImages();
			WriteableBitmap writeable_bitmap(subresource_width, subresource_width);
			uint8_t* target_data = writeable_bitmap.PixelBuffer().data();
			memcpy(target_data, decompressed_image->pixels, decompressed_image->slicePitch);

			SoftwareBitmap new_software_bitmap = SoftwareBitmap::CreateCopyFromBuffer(writeable_bitmap.PixelBuffer(), BitmapPixelFormat::Bgra8, subresource_width, subresource_width);
			new_software_bitmap = SoftwareBitmap::Convert(new_software_bitmap, BitmapPixelFormat::Bgra8, BitmapAlphaMode::Premultiplied);

			SoftwareBitmapSource new_software_bitmap_source;
			co_await new_software_bitmap_source.SetBitmapAsync(new_software_bitmap);

			graphics::subresource new_ui_mipmap;
			new_ui_mipmap.mipmap_bitmap_source(new_software_bitmap_source);
			new_ui_mipmap.height(decompressed_image->height);
			new_ui_mipmap.width(decompressed_image->width);
			new_ui_mipmap.mip_level(i);
			new_ui_mipmap.row_pitch(decompressed_image->rowPitch);
			new_ui_mipmap.slice_pitch(decompressed_image->slicePitch);

			ui_mipmaps.Append(new_ui_mipmap);
		}
	}

	void renderer::create_empty_rootsignature(std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers)
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc(
			0,
			nullptr,
			(UINT)samplers.size(),
			&samplers[0],
			D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		com_ptr<ID3DBlob> serialized_rootsig = nullptr;
		com_ptr<ID3DBlob> error_blob = nullptr;
		D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serialized_rootsig.put(), error_blob.put());

		if (error_blob != nullptr)
		{
			auto error_msg_ptr = static_cast<const char*>(error_blob->GetBufferPointer());
			winrt::hstring message = winrt::to_hstring(error_msg_ptr);
		}

		m_device->CreateRootSignature(
			0,
			serialized_rootsig->GetBufferPointer(),
			serialized_rootsig->GetBufferSize(),
			guid_of<ID3D12RootSignature>(),
			m_rootsig.put_void());
	}

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> renderer::get_static_samplers()
	{
		const CD3DX12_STATIC_SAMPLER_DESC point_wrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC point_clamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linear_wrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linear_clamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC anisotropic_wrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8);                               // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC anisotropic_clamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8);                                // maxAnisotropy

		return {
			point_wrap, point_clamp,
			linear_wrap, linear_clamp,
			anisotropic_wrap, anisotropic_clamp };
	}

	void renderer::init_psos()
	{
		auto vs = hstring(L"default_vs");
		auto ps = hstring(L"default_ps");
		auto gs = hstring(L"default_gs");

		create_basic_input_layout();
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, m_points_pso);
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, m_triangles_pso);
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, m_lines_pso);
		create_billboard_pso(m_shaders[vs], m_shaders[ps], m_shaders[gs], m_billboard_pso);
	}

	void renderer::execute_cmd_list()
	{
		check_hresult(m_graphics_cmdlist->Close());
		std::array<ID3D12CommandList*, 1> cmd_lists = { m_graphics_cmdlist.get() };
		m_cmd_queue->ExecuteCommandLists((UINT)cmd_lists.size(), &cmd_lists[0]);
	}

	void renderer::create_basic_input_layout()
	{
		m_basic_input_layout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
	}

	void renderer::select_primitive_topology()
	{
		switch (m_current_topology)
		{
		case winrt::graphics::primitive_types::points:
			m_graphics_cmdlist->SetPipelineState(m_points_pso.get());
			m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;
		case winrt::graphics::primitive_types::triangle_list:
			m_graphics_cmdlist->SetPipelineState(m_triangles_pso.get());
			m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case winrt::graphics::primitive_types::triangle_strips:
			m_graphics_cmdlist->SetPipelineState(m_triangles_pso.get());
			m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		case winrt::graphics::primitive_types::line_lists:
			m_graphics_cmdlist->SetPipelineState(m_lines_pso.get());
			m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		case winrt::graphics::primitive_types::line_strips:
			m_graphics_cmdlist->SetPipelineState(m_lines_pso.get());
			m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
			break;
		default:
			m_graphics_cmdlist->SetPipelineState(m_triangles_pso.get());
			m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;
		}
	}

	void renderer::create_pso(
		com_ptr<ID3DBlob> vertex_shader,
		com_ptr<ID3DBlob> pixel_shader,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topolgy_type,
		com_ptr<ID3D12PipelineState>& m_pso)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

		pso_desc.InputLayout = { m_basic_input_layout.data(), static_cast<UINT>(m_basic_input_layout.size()) };
		pso_desc.pRootSignature = m_rootsig.get();
		pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pso_desc.SampleMask = UINT_MAX;
		pso_desc.PrimitiveTopologyType = topolgy_type;
		pso_desc.NumRenderTargets = 1;
		pso_desc.RTVFormats[0] = m_backbuffer_format;
		pso_desc.SampleDesc.Count = 1;
		pso_desc.SampleDesc.Quality = 0;
		pso_desc.DSVFormat = m_depthstencil_format;

		pso_desc.VS =
		{
			reinterpret_cast<unsigned char*>(vertex_shader->GetBufferPointer()),
			vertex_shader->GetBufferSize()
		};

		pso_desc.PS =
		{
			reinterpret_cast<unsigned char*>(pixel_shader->GetBufferPointer()),
			pixel_shader->GetBufferSize()
		};

		check_hresult(m_device->CreateGraphicsPipelineState(&pso_desc, guid_of<ID3D12PipelineState>(), m_pso.put_void()));
	}

	void renderer::create_billboard_pso(
		com_ptr<ID3DBlob> vertex_shader,
		com_ptr<ID3DBlob> pixel_shader,
		com_ptr<ID3DBlob> geometry_shader,
		com_ptr<ID3D12PipelineState>& m_pso)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};

		pso_desc.InputLayout = { m_basic_input_layout.data(), static_cast<UINT>(m_basic_input_layout.size()) };
		pso_desc.pRootSignature = m_rootsig.get();
		pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pso_desc.RasterizerState.CullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pso_desc.SampleMask = UINT_MAX;
		pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		pso_desc.NumRenderTargets = 1;
		pso_desc.RTVFormats[0] = m_backbuffer_format;
		pso_desc.SampleDesc.Count = 1;
		pso_desc.SampleDesc.Quality = 0;
		pso_desc.DSVFormat = m_depthstencil_format;

		pso_desc.VS =
		{
			reinterpret_cast<unsigned char*>(vertex_shader->GetBufferPointer()),
			vertex_shader->GetBufferSize()
		};

		pso_desc.PS =
		{
			reinterpret_cast<unsigned char*>(pixel_shader->GetBufferPointer()),
			pixel_shader->GetBufferSize()
		};

		pso_desc.GS =
		{
			reinterpret_cast<unsigned char*>(geometry_shader->GetBufferPointer()),
			geometry_shader->GetBufferSize()
		};

		check_hresult(m_device->CreateGraphicsPipelineState(&pso_desc, guid_of<ID3D12PipelineState>(), m_pso.put_void()));
	}
	void renderer::reflect_shader(com_ptr<implementation::shader> target_shader)
	{
		auto shader_byte_code = target_shader->byte_code();

		if (shader_byte_code)
		{

			com_ptr<ID3D12ShaderReflection> shader_reflector;
			check_hresult(D3DReflect(
				shader_byte_code->GetBufferPointer(),
				shader_byte_code->GetBufferSize(),
				guid_of<ID3D12ShaderReflection>(),
				shader_reflector.put_void()));

			D3D12_SHADER_DESC shader_desc = {};
			check_hresult(shader_reflector->GetDesc(&shader_desc));

			auto new_reflection_attributes = single_threaded_observable_vector<IInspectable>();

			{
				target_shader->compiler(to_hstring(shader_desc.Creator));
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Compiler" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.Creator));
				new_shader_reflection_attribute.attribute_description(hstring{ L"The name of the originating compiler of the shader." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));

			}

			{
				target_shader->instruction_count(shader_desc.InstructionCount);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Instructions" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.InstructionCount));
				new_shader_reflection_attribute.attribute_description(hstring{ L"The number of intermediate-language instructions in the compiled shader." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{

				// Decode the shader program type and version
				D3D12_SHADER_VERSION_TYPE shader_program_type = static_cast<D3D12_SHADER_VERSION_TYPE>((shader_desc.Version & 0xFFFF0000) >> 16);
				hstring shader_major_version = to_hstring((shader_desc.Version & 0x000000F0) >> 4);
				hstring shader_minor_version = to_hstring(shader_desc.Version & 0x0000000F);
				hstring shader_program_type_string;

				switch (shader_program_type)
				{
				case D3D12_SHVER_PIXEL_SHADER:
					shader_program_type_string = L"Pixel";
					break;
				case D3D12_SHVER_VERTEX_SHADER:
					shader_program_type_string = L"Vertex";
					break;
				case D3D12_SHVER_GEOMETRY_SHADER:
					shader_program_type_string = L"Geometry";
					break;
				case D3D12_SHVER_HULL_SHADER:
					shader_program_type_string = L"Hull";
					break;
				case D3D12_SHVER_DOMAIN_SHADER:
					shader_program_type_string = L"Domain";
					break;
				case D3D12_SHVER_COMPUTE_SHADER:
					shader_program_type_string = L"Compute";
					break;
				default:
					shader_program_type_string = L"Unknown";
					break;
				}

				hstring shader_version = shader_program_type_string + L" " + shader_major_version + L"." + shader_minor_version;

				target_shader->version(shader_desc.Version);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Version" });
				new_shader_reflection_attribute.attribute_value(shader_version);
				new_shader_reflection_attribute.attribute_description(hstring{ L"The shader version" });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->input_parameters_count(shader_desc.InputParameters);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Input parameters" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.InputParameters));
				new_shader_reflection_attribute.attribute_description(hstring{ L"The number of parameters in the input signature." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->output_parameters_count(shader_desc.OutputParameters);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Output parameters" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.OutputParameters));
				new_shader_reflection_attribute.attribute_description(hstring{ L"The number of parameters in the output signature." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->constant_buffer_count(shader_desc.ConstantBuffers);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Constant buffers" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.ConstantBuffers));
				new_shader_reflection_attribute.attribute_description(hstring{ L"The number of shader-constant buffers." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->bound_resources_count(shader_desc.BoundResources);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Bound resources" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.BoundResources));
				new_shader_reflection_attribute.attribute_description(hstring{ L"The number of resource (textures and buffers) bound to a shader." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->texture_load_instructions_count(shader_desc.TextureLoadInstructions);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Texture load instructions" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.TextureLoadInstructions));
				new_shader_reflection_attribute.attribute_description(hstring{ L"Number of texture load instructions" });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->texture_normal_instructions_count(shader_desc.TextureNormalInstructions);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Texture normal instructions" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.TextureNormalInstructions));
				new_shader_reflection_attribute.attribute_description(hstring{ L"Number of non-categorized texture instructions." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->texture_comparison_instructions_count(shader_desc.TextureCompInstructions);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Texture comparison instructions" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.TextureCompInstructions));
				new_shader_reflection_attribute.attribute_description(hstring{ L"Number of texture comparison instructions" });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->texture_bias_instructions_count(shader_desc.TextureBiasInstructions);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Texture bias instructions" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.TextureBiasInstructions));
				new_shader_reflection_attribute.attribute_description(hstring{ L"Number of texture bias instructions" });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			{
				target_shader->texture_gradient_instructions_count(shader_desc.TextureGradientInstructions);
				graphics::generic_attribute new_shader_reflection_attribute;
				new_shader_reflection_attribute.attribute_name(hstring{ L"Texture gradient instructions" });
				new_shader_reflection_attribute.attribute_value(to_hstring(shader_desc.TextureGradientInstructions));
				new_shader_reflection_attribute.attribute_description(hstring{ L"Number of texture gradient instructions." });
				new_reflection_attributes.Append(box_value(new_shader_reflection_attribute));
			}

			target_shader->shader_reflection_data(new_reflection_attributes);

			if (shader_desc.BoundResources > 0)
			{

				for (size_t i = 0; i < shader_desc.BoundResources; ++i)
				{
					D3D12_SHADER_INPUT_BIND_DESC shader_bind_desc = {};
					check_hresult(shader_reflector->GetResourceBindingDesc(i, &shader_bind_desc));

					{
						graphics::generic_attribute new_shader_bound_resource;
						new_shader_bound_resource.attribute_name(hstring{ L"Resource name" });
						new_shader_bound_resource.attribute_value(to_hstring(shader_bind_desc.Name));
						new_shader_bound_resource.attribute_description(hstring{ L"Name of the shader resource." });
						target_shader->shader_bound_resources().Append(new_shader_bound_resource);
					}

					{
						graphics::generic_attribute new_shader_bound_resource;

						hstring input_type;
						hstring resource_type_description;

						switch (shader_bind_desc.Type)
						{
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
							input_type = L"Sampler";
							resource_type_description = L"The shader resource is a sampler.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
							input_type = L"Constant buffer";
							resource_type_description = L"The shader resource is a constant buffer.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER:
							input_type = L"Texture buffer";
							resource_type_description = L"The shader resource is a texture buffer.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
							input_type = L"Texture";
							resource_type_description = L"The shader resource is a texture.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
							input_type = L"Read-and-write buffer";
							resource_type_description = L"Shader model 4 resources are read only. Shader model 5 implements a new corresponding set of read/write resources.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
							input_type = L"Structured buffer";
							resource_type_description = L"A structured buffer is a buffer that contains elements of equal sizes. Use a structure with one or more member types to define an element.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
							input_type = L"Read-and-write structured buffer";
							resource_type_description = L"Shader model 4 resources are read only. Shader model 5 implements a new corresponding set of read/write resources. See structured buffer.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS:
							input_type = L"Byte-address buffer";
							resource_type_description = L"Buffer that uses a byte value offset from the beginning of the buffer to access data instead of indexing per element using a stride.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS:
							input_type = L"Read-and-write byte-address buffer";
							resource_type_description = L"Shader model 4 resources are read only. Shader model 5 implements a new corresponding set of read/write resources. See Byte-address buffers.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED:
							input_type = L"Append-structured buffer";
							resource_type_description = L"Output buffer that appears as a stream the shader may append to.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED:
							input_type = L"Consume-structured buffer";
							resource_type_description = L"Input buffer that appears as a stream the shader may pull values from.";
							break;
						case _D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
							input_type = L"Read-and-write structured buffer that uses the built-in counter to append or consume";
							resource_type_description = L"Shader model 4 resources are read only. Shader model 5 implements a new corresponding set of read/write resources. See Consume-structured buffer.";
							break;
						default:
							input_type = L"Unknown";
							resource_type_description = L"";
							break;
						}

						new_shader_bound_resource.attribute_name(hstring{ L"Resource type" });
						new_shader_bound_resource.attribute_value(input_type);
						new_shader_bound_resource.attribute_description(resource_type_description);
						target_shader->shader_bound_resources().Append(new_shader_bound_resource);
					}
				}
			}
		}
	}
}
