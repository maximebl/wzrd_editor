#include "pch.h"
#include "renderer.h"

ID3D12Device* winrt::graphics::implementation::renderer::g_device = nullptr;
ID3D12GraphicsCommandList* winrt::graphics::implementation::renderer::g_cmd_list = nullptr;

namespace winrt::graphics::implementation
{
	Windows::Foundation::IAsyncOperation<graphics::compilation_result> renderer::pick_and_compile_shader(
		hstring const shader_name,
		hstring const entry_point,
		hstring const version)
	{
		graphics::compilation_result result;

		auto shader_file_bytes = co_await pick_file(winrt::hstring(L".hlsl"));

		if (shader_file_bytes.size() == 0)
		{
			result.status = graphics::compilation_status::cancelled;
			return result;
		}

		co_await winrt::resume_background();
		//auto[compilation_result, result_blob] = compile_shader(version, shader_file_bytes, entry_point);

		UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		com_ptr<ID3DBlob> byte_code = nullptr;
		com_ptr<ID3DBlob> errors = nullptr;

		D3DCompile(
			&shader_file_bytes.front(),
			shader_file_bytes.size(),
			nullptr,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			to_string(entry_point).c_str(),
			to_string(version).c_str(),
			compile_flags,
			0,
			byte_code.put(),
			errors.put()
		);

		if (errors != nullptr)
		{
			auto error_msg_ptr = static_cast<const char*>(errors->GetBufferPointer());
			hstring message = to_hstring(error_msg_ptr);

			result.status = graphics::compilation_status::error;
			result.error_message = message;
		}

		result.status = graphics::compilation_status::success;
		m_shaders[to_string(shader_name)] = byte_code;
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

	//std::pair<graphics::compilation_result, com_ptr<ID3D10Blob>> renderer::compile_shader(
	//	hstring const& version,
	//	const std::vector<unsigned char>& file_bytes,
	//	hstring const& entry_point)
	//{
	//	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//	com_ptr<ID3DBlob> byte_code = nullptr;
	//	com_ptr<ID3DBlob> errors = nullptr;
	//	graphics::compilation_result result;

	//	D3DCompile(
	//		&file_bytes.front(),
	//		file_bytes.size(),
	//		nullptr,
	//		nullptr,
	//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
	//		to_string(entry_point).c_str(),
	//		to_string(version).c_str(),
	//		compileFlags,
	//		0,
	//		byte_code.put(),
	//		errors.put()
	//	);

	//	if (errors != nullptr)
	//	{
	//		auto error_msg_ptr = static_cast<const char*>(errors->GetBufferPointer());
	//		hstring message = to_hstring(error_msg_ptr);

	//		result.is_success = false;
	//		result.error_message = message;
	//		return std::make_pair(result, errors);
	//	}
	//	result.is_success = true;
	//	return std::make_pair(result, byte_code);
	//}

	renderer::renderer()
	{
	}

	void renderer::enable_debug_layer()
	{
		com_ptr<ID3D12Debug1> debug_controller;
		check_hresult(
			D3D12GetDebugInterface(__uuidof(ID3D12Debug1), debug_controller.put_void())
		);
		debug_controller->EnableDebugLayer();
		debug_controller->SetEnableGPUBasedValidation(true);
	}

	void renderer::initialize(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain)
	{
		create_factory();
		create_device();
		create_fence();
		create_cmd_objects();
		create_descriptor_heaps();
		create_depthstencil_buffer();
		create_swapchain_xaml(target_swapchain);
		create_render_targets();
		create_rootsignature();
	}

	Windows::Foundation::IAsyncAction renderer::main_loop()
	{
		co_await winrt::resume_background();

		while (m_is_rendering)
		{
			render();
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

	void renderer::render()
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
		//m_graphics_cmdlist->SetGraphicsRootConstantBufferView(0, m_object_cb->get_resource()->GetGPUVirtualAddress());

		auto res = m_current_buffer->get_view();
		auto res2 = (D3D12_VERTEX_BUFFER_VIEW*)&res;

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

	void renderer::update()
	{
		//using namespace DirectX;

		//XMMATRIX perspective_projection = XMMatrixPerspectiveFovLH(
		//	0.25f * XM_PI,
		//	static_cast<float>(m_output_width) / m_output_height,
		//	1.0f,
		//	1000.0f);

		//XMStoreFloat4x4(&m_proj, perspective_projection);

		//float x = m_radius * sinf(m_phi) * cosf(m_theta);
		//float z = m_radius * sinf(m_phi) * sinf(m_theta);
		//float y = m_radius * cosf(m_phi);

		//XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
		//XMVECTOR target = XMVectorZero();
		//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		//XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		//XMStoreFloat4x4(&m_view, view);

		//XMMATRIX world = XMLoadFloat4x4(&m_world);
		//XMMATRIX proj = XMLoadFloat4x4(&m_proj);
		//XMMATRIX world_view_proj = world * view * proj;

		//XMFLOAT4X4 tex_transform_floats = MathHelper::Identity4x4();
		//XMMATRIX tex_transforms = XMLoadFloat4x4(&tex_transform_floats);

		//object_constants object_constants;
		//XMStoreFloat4x4(&object_constants.texture_transform, XMMatrixTranspose(tex_transforms));
		//XMStoreFloat4x4(&object_constants.world, XMMatrixTranspose(world_view_proj));

		//m_object_cb->copy_data(0, object_constants);
	}

	void renderer::start_render_loop()
	{
		bool is_ready_to_render = !m_is_rendering && m_shaders.size() > 0;
		if (is_ready_to_render)
		{
			m_is_rendering = true;
			init_psos();
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

	void renderer::create_factory()
	{
		check_hresult(
			CreateDXGIFactory1(__uuidof(IDXGIFactory4), m_dxgi_factory.put_void())
		);
	}

	void renderer::create_device()
	{
		check_hresult(
			D3D12CreateDevice(
				nullptr,
				D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
				__uuidof(ID3D12Device),
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
				__uuidof(ID3D12Fence),
				m_gpu_fence.put_void())
		);
	}

	void renderer::create_cmd_objects()
	{
		D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
		cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;

		check_hresult(
			m_device->CreateCommandQueue(&cmd_queue_desc, __uuidof(ID3D12CommandQueue), m_cmd_queue.put_void())
		);

		check_hresult(
			m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), m_cmd_allocator.put_void())
		);

		check_hresult(
			m_device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_cmd_allocator.get(),
				nullptr,
				__uuidof(ID3D12GraphicsCommandList),
				m_graphics_cmdlist.put_void())
		);
		g_cmd_list = m_graphics_cmdlist.get();
	}

	void renderer::create_descriptor_heaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtv_heap_desc.NodeMask = 0;
		rtv_heap_desc.NumDescriptors = m_swapchain_buffer_count;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		check_hresult(
			m_device->CreateDescriptorHeap(&rtv_heap_desc, __uuidof(ID3D12DescriptorHeap), m_rtv_heap.put_void())
		);

		D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
		dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsv_heap_desc.NodeMask = 0;
		dsv_heap_desc.NumDescriptors = 1;
		dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		check_hresult(
			m_device->CreateDescriptorHeap(&dsv_heap_desc, __uuidof(ID3D12DescriptorHeap), m_dsv_heap.put_void())
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
				__uuidof(ID3D12Resource),
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
				m_swapchain->GetBuffer(i, __uuidof(ID3D12Resource), m_swapchain_buffer[i].put_void())
			);
			m_device->CreateRenderTargetView(m_swapchain_buffer[i].get(), nullptr, rtv_heap_handle);
			rtv_heap_handle.Offset(1, m_rtv_descriptor_size);
		}
	}

	void renderer::create_rootsignature()
	{
		auto samplers = get_static_samplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc(
			0,
			nullptr,
			(UINT)samplers.size(),
			samplers.data(),
			D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		com_ptr<ID3DBlob> serialized_rootsig = nullptr;
		com_ptr<ID3DBlob> error_blob = nullptr;
		D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1, serialized_rootsig.put(), error_blob.put());

		m_device->CreateRootSignature(
			0,
			serialized_rootsig->GetBufferPointer(),
			serialized_rootsig->GetBufferSize(),
			__uuidof(ID3D12RootSignature),
			m_rootsig.put_void());
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> renderer::get_static_samplers()
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
		auto vs = std::string("default_vs");
		auto ps = std::string("default_ps");

		create_basic_input_layout();
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, m_points_pso);
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, m_triangles_pso);
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, m_lines_pso);
		execute_cmd_list();
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
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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
		com_ptr<ID3D10Blob> vertex_shader,
		com_ptr<ID3D10Blob> pixel_shader,
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

		// shaders
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

		check_hresult(m_device->CreateGraphicsPipelineState(&pso_desc, __uuidof(ID3D12PipelineState), m_pso.put_void()));
	}
}
