#include "pch.h"
#include "renderer.h"

namespace winrt::graphics::implementation
{
	Windows::Foundation::IAsyncOperation<graphics::compilation_result> renderer::pick_and_compile_shader(hstring const shader_name, hstring const entry_point, hstring const version)
	{
		auto shader_file_bytes = co_await pick_file(winrt::hstring(L".hlsl"));

		co_await winrt::resume_background();
		auto [compilation_result, result_blob] = compile_shader(version, shader_file_bytes, entry_point);

		if (compilation_result.is_success)
		{
			m_shaders[to_string(shader_name)];
		}
		return compilation_result;
		//return IAsyncOperation<graphics::compilation_result>
	}

	std::pair<graphics::compilation_result, com_ptr<ID3D10Blob>> renderer::compile_shader(
		hstring const& version, 
		const std::vector<unsigned char>& file_bytes, 
		hstring const& entry_point)
	{
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		com_ptr<ID3DBlob> byte_code = nullptr;
		com_ptr<ID3DBlob> errors = nullptr;
		graphics::compilation_result result;

		D3DCompile(
			&file_bytes.front(),
			file_bytes.size(),
			nullptr,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			to_string(entry_point).c_str(),
			to_string(version).c_str(),
			compileFlags,
			0,
			byte_code.put(),
			errors.put()
		);

		if (errors != nullptr)
		{
			auto error_msg_ptr = static_cast<const char*>(errors->GetBufferPointer());
			hstring message = to_hstring(error_msg_ptr);

			result.is_success = false;
			result.error_message = message;
			return std::make_pair(result, errors);
		}
		result.is_success = true;
		return std::make_pair(result, byte_code);
	}

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
		//create_device();
		//create_fence();
		//create_cmd_objects();
		//create_descriptor_heaps();
		//create_depthstencil_buffer();
		//create_swapchain_xaml(target_swapchain);
		//create_render_targets();
		//create_constant_buffers();
		//create_rootsignature();
	}

	void renderer::start_render_loop()
	{
		if (!m_running)
		{
			init_psos();
		}
	}

	graphics::buffer renderer::current_buffer()
	{
		return m_current_buffer;
	}

	void renderer::current_buffer(graphics::buffer const& value)
	{
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

	void renderer::create_constant_buffers()
	{
		m_object_cb = std::make_unique<upload_buffer<object_constants>>(m_device.get(), 1, true, false);
	}

	void renderer::create_rootsignature()
	{
		CD3DX12_ROOT_PARAMETER root_parameters[1];
		root_parameters[0].InitAsConstantBufferView(0);

		auto samplers = get_static_samplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc(
			1,
			root_parameters,
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
		create_pso(m_shaders[vs], m_shaders[ps], D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, m_triangles_pso);
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

	void renderer::create_pso(com_ptr<ID3D10Blob> vertex_shader,
		com_ptr<ID3D10Blob> pixel_shader,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topolgy_type,
		com_ptr<ID3D12PipelineState> m_pso)
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
