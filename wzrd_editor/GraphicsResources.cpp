#include "pch.h"
#include "GraphicsResources.h"

GraphicsResources::GraphicsResources()
{
}

GraphicsResources::~GraphicsResources()
{
}

void GraphicsResources::enable_debug_layer()
{
	winrt::com_ptr<ID3D12Debug1> debugController;
	winrt::check_hresult(
		D3D12GetDebugInterface(
			winrt::guid_of<ID3D12Debug1>(), debugController.put_void()
		)
	);
	debugController->EnableDebugLayer();
	debugController->SetEnableGPUBasedValidation(true);
}

void GraphicsResources::create_factory()
{
	winrt::check_hresult(
		CreateDXGIFactory1(winrt::guid_of<IDXGIFactory4>(), m_factory.put_void())
	);
}

void GraphicsResources::create_device()
{
	winrt::check_hresult(
		D3D12CreateDevice(nullptr,
			D3D_FEATURE_LEVEL_11_0,
			winrt::guid_of<ID3D12Device>(),
			m_device.put_void()
		)
	);
	
	Utilities::device = m_device.get();
}

void GraphicsResources::create_fence()
{
	winrt::check_hresult(
		m_device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			winrt::guid_of<ID3D12Fence>(),
			m_gpu_fence.put_void()
		)
	);
}

void GraphicsResources::create_cmd_objects()
{
	D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
	cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	winrt::check_hresult(
		m_device->CreateCommandQueue(&cmd_queue_desc, winrt::guid_of<ID3D12CommandQueue>(), m_cmd_queue.put_void())
	);

	winrt::check_hresult(
		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, winrt::guid_of<ID3D12CommandAllocator>(), m_cmd_allocator.put_void())
	);

	winrt::check_hresult(
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmd_allocator.get(), nullptr, winrt::guid_of<ID3D12GraphicsCommandList>(), m_graphics_cmdlist.put_void())
	);
	Utilities::cmd_list = m_graphics_cmdlist.get();
}

void GraphicsResources::create_descriptor_heaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0;
	rtv_heap_desc.NumDescriptors = m_swapchain_buffer_count;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	winrt::check_hresult(
		m_device->CreateDescriptorHeap(&rtv_heap_desc, winrt::guid_of<ID3D12DescriptorHeap>(), m_rtv_heap.put_void())
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0;
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	winrt::check_hresult(
		m_device->CreateDescriptorHeap(&dsv_heap_desc, winrt::guid_of<ID3D12DescriptorHeap>(), m_dsv_heap.put_void())
	);
}

void GraphicsResources::create_swapchain_corewindow(winrt::Windows::UI::Core::CoreWindow window)
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
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchain_desc.Flags = 0;
		swapchain_desc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
		swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		IUnknown* window_abi = winrt::get_unknown(window);

		winrt::check_hresult(
			m_factory->CreateSwapChainForCoreWindow(
				m_cmd_queue.get(), window_abi, &swapchain_desc, nullptr, m_swapchain.put()
			));
	}
}

void GraphicsResources::create_swapchain_xamlpanel(winrt::Windows::UI::Xaml::Controls::SwapChainPanel xaml_swapchain_panel)
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
		swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapchain_desc.Flags = 0;
		swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
		swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		winrt::check_hresult(
			m_factory->CreateSwapChainForComposition(
				m_cmd_queue.get(),
				&swapchain_desc,
				nullptr,
				m_swapchain.put()
			));

		// associate DXGI swap chain with the XAML SwapChainPanel
		winrt::check_hresult(
			xaml_swapchain_panel.as<ISwapChainPanelNative>()->SetSwapChain(m_swapchain.get())
		);
	}
}
void GraphicsResources::create_render_targets()
{
	m_rtv_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < m_swapchain_buffer_count; i++)
	{
		winrt::check_hresult(
			m_swapchain->GetBuffer(i, winrt::guid_of<ID3D12Resource>(), m_swapchain_buffer[i].put_void())
		);
		m_device->CreateRenderTargetView(m_swapchain_buffer[i].get(), nullptr, rtv_heap_handle);
		rtv_heap_handle.Offset(1, m_rtv_descriptor_size);
	}
}

void GraphicsResources::create_depthstencil_buffer()
{
	D3D12_RESOURCE_DESC depth_stencil_desc = {};
	depth_stencil_desc.Alignment = 0;
	depth_stencil_desc.DepthOrArraySize = 1;
	depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depth_stencil_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depth_stencil_desc.Height = m_output_height;
	depth_stencil_desc.Width = m_output_width;
	depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
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
			D3D12_HEAP_FLAG_NONE,
			&depth_stencil_desc,
			D3D12_RESOURCE_STATE_COMMON,
			&clear_value,
			winrt::guid_of<ID3D12Resource>(),
			m_depthstencil_buffer.put_void()
		)
	);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
	dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Format = m_depthstencil_format;
	dsv_desc.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView(
		m_depthstencil_buffer.get(),
		&dsv_desc,
		m_dsv_heap->GetCPUDescriptorHandleForHeapStart()
	);
}

void GraphicsResources::create_constant_buffers()
{
	m_object_cb = std::make_unique<upload_buffer<object_constants>>(m_device.get(), 1, true, false);
	m_box_geo = std::make_unique<MeshGeometry>();
}

void GraphicsResources::create_rootsignature()
{
	CD3DX12_ROOT_PARAMETER root_parameters[1];
	root_parameters[0].InitAsConstantBufferView(0);

	auto samplers = Utilities::get_static_samplers();
	CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc(1, root_parameters, (UINT)samplers.size(), samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	winrt::com_ptr<ID3DBlob> serialized_rootsig = nullptr;
	winrt::com_ptr<ID3DBlob> error_blob = nullptr;
	D3D12SerializeRootSignature(&rootsig_desc, D3D_ROOT_SIGNATURE_VERSION_1, serialized_rootsig.put(), error_blob.put());

	if (error_blob != nullptr)
	{
		auto error_msg_ptr = static_cast<const char*>(error_blob->GetBufferPointer());
		winrt::hstring message = winrt::to_hstring(error_msg_ptr);
	}

	m_device->CreateRootSignature(
		0,
		serialized_rootsig->GetBufferPointer(),
		serialized_rootsig->GetBufferSize(),
		winrt::guid_of<ID3D12RootSignature>(),
		m_rootsig.put_void()
	);
}

std::unique_ptr<winrt::wzrd_editor::data::Texture> GraphicsResources::create_texture(std::vector<unsigned char> bytes, int file_size, std::string texture_name)
{
	auto woodcrate_texture = std::make_unique<winrt::wzrd_editor::data::Texture>();
	woodcrate_texture->Name = texture_name;

	Microsoft::WRL::ComPtr<ID3D12Resource> tmpResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> tmpUploadHeap = nullptr;

	//winrt::check_hresult(
	//	DirectX::CreateDDSTextureFromMemory12(
	//		m_device.get(),
	//		m_graphics_cmdlist.get(),
	//		&bytes.front(),
	//		file_size,
	//		tmpResource,
	//		tmpUploadHeap
	//	)
	//);

	woodcrate_texture->Resource.copy_from(tmpResource.Get());
	woodcrate_texture->UploadHeap.copy_from(tmpUploadHeap.Get());

	return woodcrate_texture;
}

void GraphicsResources::create_shader_resources(ID3D12Resource* resource /* turn this into a vector of resources eventually */)
{
	D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
	srv_heap_desc.NumDescriptors = 1;
	srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	winrt::check_hresult(m_device->CreateDescriptorHeap(&srv_heap_desc, winrt::guid_of<ID3D12DescriptorHeap>(), m_srv_heap.put_void()));

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = resource->GetDesc().Format;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;

	m_device->CreateShaderResourceView(resource, &srv_desc, m_srv_heap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicsResources::create_points_pso(winrt::com_ptr<ID3D10Blob> vertex_shader, winrt::com_ptr<ID3D10Blob> pixel_shader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC points_pso_desc = {};

	points_pso_desc.InputLayout = { m_basic_input_layout.data(), static_cast<UINT>(m_basic_input_layout.size()) };
	points_pso_desc.pRootSignature = m_rootsig.get();
	points_pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	points_pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	points_pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	points_pso_desc.SampleMask = UINT_MAX;
	points_pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	points_pso_desc.NumRenderTargets = 1;
	points_pso_desc.RTVFormats[0] = m_backbuffer_format;
	points_pso_desc.SampleDesc.Count = 1;
	points_pso_desc.SampleDesc.Quality = 0;
	points_pso_desc.DSVFormat = m_depthstencil_format;

	// shaders
	points_pso_desc.VS =
	{
		reinterpret_cast<unsigned char*>(vertex_shader->GetBufferPointer()),
		vertex_shader->GetBufferSize()
	};

	points_pso_desc.PS =
	{
		reinterpret_cast<unsigned char*>(pixel_shader->GetBufferPointer()),
		pixel_shader->GetBufferSize()
	};

	winrt::check_hresult(m_device->CreateGraphicsPipelineState(&points_pso_desc, winrt::guid_of<ID3D12PipelineState>(), m_points_pso.put_void()));
}

void GraphicsResources::create_lines_pso(winrt::com_ptr<ID3D10Blob> vertex_shader, winrt::com_ptr<ID3D10Blob> pixel_shader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC lines_pso_desc = {};

	lines_pso_desc.InputLayout = { m_basic_input_layout.data(), static_cast<UINT>(m_basic_input_layout.size()) };
	lines_pso_desc.pRootSignature = m_rootsig.get();
	lines_pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	lines_pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	lines_pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	lines_pso_desc.SampleMask = UINT_MAX;
	lines_pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	lines_pso_desc.NumRenderTargets = 1;
	lines_pso_desc.RTVFormats[0] = m_backbuffer_format;
	lines_pso_desc.SampleDesc.Count = 1;
	lines_pso_desc.SampleDesc.Quality = 0;
	lines_pso_desc.DSVFormat = m_depthstencil_format;

	// shaders
	lines_pso_desc.VS =
	{
		reinterpret_cast<unsigned char*>(vertex_shader->GetBufferPointer()),
		vertex_shader->GetBufferSize()
	};

	lines_pso_desc.PS =
	{
		reinterpret_cast<unsigned char*>(pixel_shader->GetBufferPointer()),
		pixel_shader->GetBufferSize()
	};

	winrt::check_hresult(m_device->CreateGraphicsPipelineState(&lines_pso_desc, winrt::guid_of<ID3D12PipelineState>(), m_lines_pso.put_void()));
}

void GraphicsResources::create_triangles_pso(winrt::com_ptr<ID3D10Blob> vertex_shader, winrt::com_ptr<ID3D10Blob> pixel_shader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC triangles_pso_desc = {};

	triangles_pso_desc.InputLayout = { m_basic_input_layout.data(), static_cast<UINT>(m_basic_input_layout.size()) };
	triangles_pso_desc.pRootSignature = m_rootsig.get();
	triangles_pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	triangles_pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	triangles_pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	triangles_pso_desc.SampleMask = UINT_MAX;
	triangles_pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	triangles_pso_desc.NumRenderTargets = 1;
	triangles_pso_desc.RTVFormats[0] = m_backbuffer_format;
	triangles_pso_desc.SampleDesc.Count = 1;
	triangles_pso_desc.SampleDesc.Quality = 0;
	triangles_pso_desc.DSVFormat = m_depthstencil_format;

	// shaders
	triangles_pso_desc.VS =
	{
		reinterpret_cast<unsigned char*>(vertex_shader->GetBufferPointer()),
		vertex_shader->GetBufferSize()
	};

	triangles_pso_desc.PS =
	{
		reinterpret_cast<unsigned char*>(pixel_shader->GetBufferPointer()),
		pixel_shader->GetBufferSize()
	};

	winrt::check_hresult(m_device->CreateGraphicsPipelineState(&triangles_pso_desc, winrt::guid_of<ID3D12PipelineState>(), m_triangles_pso.put_void()));
}

void GraphicsResources::create_basic_input_layout()
{
	m_basic_input_layout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void GraphicsResources::create_texture_input_layout()
{
	m_texture_input_layout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void GraphicsResources::flush_cmd_queue()
{
	m_cpu_fence++;
	winrt::check_hresult(
		m_cmd_queue->Signal(m_gpu_fence.get(), m_cpu_fence)
	);

	if (m_gpu_fence->GetCompletedValue() < m_cpu_fence)
	{
		HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		winrt::check_hresult(m_gpu_fence->SetEventOnCompletion(m_cpu_fence, event_handle));
		WaitForSingleObject(event_handle, INFINITE);

		CloseHandle(event_handle);
	}
}

void GraphicsResources::execute_cmd_list()
{
	winrt::check_hresult(m_graphics_cmdlist->Close());
	std::array<ID3D12CommandList*, 1> cmd_lists = { m_graphics_cmdlist.get() };
	m_cmd_queue->ExecuteCommandLists((UINT)cmd_lists.size(), &cmd_lists[0]);
}

void GraphicsResources::swap_upload_buffer(int32_t new_element_count, std::vector<Vertex_tex> vertices)
{
	auto current_size = vertices.size();

	if (!is_using_swap_buffer)
	{
		// discard m_swap_buffer if already exist
		swap_vertex_buffer.reset();
		swap_index_buffer.reset();
		swap_vertex_buffer = std::make_unique<upload_buffer<Vertex_tex>>(m_device.get(), new_element_count, false, true);
		swap_index_buffer = std::make_unique<upload_buffer<std::uint16_t>>(m_device.get(), new_element_count, false, true);

		m_box_geo->SwapVertexBufferByteSize = new_element_count * sizeof(Vertex_tex);
		m_box_geo->SwapIndexBufferByteSize = new_element_count * sizeof(std::uint16_t);
		m_box_geo->SwapVertexByteStride = sizeof(Vertex_tex);
		m_box_geo->index_count = new_element_count;

		for (size_t i = 0; i < current_size; ++i)
		{
			swap_vertex_buffer->copy_data(i, vertices[i]);
			swap_index_buffer->copy_data(i, i);
		}
		m_box_geo->SwapVertexBufferGPU.detach();
		m_box_geo->SwapIndexBufferGPU.detach();
		m_box_geo->SwapVertexBufferGPU.attach(swap_vertex_buffer->get_resource());
		m_box_geo->SwapIndexBufferGPU.attach(swap_index_buffer->get_resource());

		m_box_geo->SwapVertexBufferGPU->SetName(std::wstring(L"swap_vertex_buffer_GPU").c_str());
		m_box_geo->SwapIndexBufferGPU->SetName(std::wstring(L"swap_index_buffer_GPU").c_str());

		is_using_swap_buffer = true;
	}
	else {
		// discard m_dynamic_buffer
		// create new upload_buffer
		m_dynamic_vertex_buffer.reset();
		m_dynamic_index_buffer.reset();
		m_dynamic_vertex_buffer = std::make_unique<upload_buffer<Vertex_tex>>(m_device.get(), new_element_count, false, true);
		m_dynamic_index_buffer = std::make_unique<upload_buffer<std::uint16_t>>(m_device.get(), new_element_count, false, true);

		m_box_geo->VertexBufferByteSize = new_element_count * sizeof(Vertex_tex);
		m_box_geo->IndexBufferByteSize = new_element_count * sizeof(std::uint16_t);
		m_box_geo->VertexByteStride = sizeof(Vertex_tex);
		m_box_geo->index_count = new_element_count;
		// assign new data to it
		for (size_t i = 0; i < current_size; ++i)
		{
			m_dynamic_vertex_buffer->copy_data(i, vertices[i]);
			m_dynamic_index_buffer->copy_data(i, i);
		}
		// assign its resource pointer to the VertexBufferGPU
		m_box_geo->VertexBufferGPU.detach();
		m_box_geo->IndexBufferGPU.detach();
		m_box_geo->VertexBufferGPU.attach(m_dynamic_vertex_buffer->get_resource());
		m_box_geo->IndexBufferGPU.attach(m_dynamic_index_buffer->get_resource());

		m_box_geo->VertexBufferGPU->SetName(std::wstring(L"vertex_buffer_GPU").c_str());
		m_box_geo->IndexBufferGPU->SetName(std::wstring(L"index_buffer_GPU").c_str());

		is_using_swap_buffer = false;
	}
}

void GraphicsResources::init_static_buffer(std::vector<Vertex_tex>& vertices)
{
	using namespace DirectX;

	std::vector<std::uint16_t> indices = { 0,1,2,3,4,5 };

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex_tex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_box_geo->Name = "boxGeo";

	m_box_geo->VertexBufferGPU = Utilities::create_default_buffer(
		m_device.get(),
		m_graphics_cmdlist.get(),
		vertices.data(),
		vbByteSize,
		m_box_geo->VertexBufferUploader
	);

	m_box_geo->IndexBufferGPU = Utilities::create_default_buffer(
		m_device.get(),
		m_graphics_cmdlist.get(),
		indices.data(),
		ibByteSize,
		m_box_geo->IndexBufferUploader
	);

	m_box_geo->VertexByteStride = sizeof(Vertex_tex);
	m_box_geo->VertexBufferByteSize = vbByteSize;
	m_box_geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_box_geo->IndexBufferByteSize = ibByteSize;

	m_box_geo->index_count = indices.size();
	m_box_geo->start_index_location = 0;
	m_box_geo->base_vertex_location = 0;
}

void GraphicsResources::init_dynamic_buffer(int32_t vertex_count, bool is_auto_resize)
{
	using namespace DirectX;

	const UINT vbByteSize = vertex_count * sizeof(Vertex_tex);
	const UINT ibByteSize = vertex_count * sizeof(std::uint16_t);

	m_dynamic_vertex_buffer = std::make_unique<upload_buffer<Vertex_tex>>(m_device.get(), vertex_count, false, is_auto_resize);
	m_dynamic_index_buffer = std::make_unique<upload_buffer<std::uint16_t>>(m_device.get(), vertex_count, false, is_auto_resize);

	m_box_geo->VertexBufferGPU.attach(m_dynamic_vertex_buffer->get_resource());
	m_box_geo->IndexBufferGPU.attach(m_dynamic_index_buffer->get_resource());

	m_box_geo->Name = "boxGeo";
	m_box_geo->VertexByteStride = sizeof(Vertex_tex);
	m_box_geo->VertexBufferByteSize = vbByteSize;
	m_box_geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_box_geo->IndexBufferByteSize = ibByteSize;

	m_box_geo->VertexBufferGPU->SetName(std::wstring(L"initial_vertex_buffer_GPU").c_str());
	m_box_geo->IndexBufferGPU->SetName(std::wstring(L"initial_index_buffer_GPU").c_str());

	m_box_geo->index_count = vertex_count;
	m_box_geo->start_index_location = 0;
	m_box_geo->base_vertex_location = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsResources::current_backbuffer_view() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtv_heap->GetCPUDescriptorHandleForHeapStart(),
		m_current_backbuffer,
		m_rtv_descriptor_size
	);
}

void GraphicsResources::update_current_buffer(std::vector<Vertex_tex>& vertices)
{
	if (is_using_swap_buffer)
	{
		update_swap_buffer(vertices);
	}
	else
	{
		update_dynamic_buffer(vertices);
	}
}

void GraphicsResources::update_swap_buffer(std::vector<Vertex_tex>& vertices)
{
	update_vbv_content(vertices, swap_vertex_buffer, swap_index_buffer);
}

void GraphicsResources::update_dynamic_buffer(std::vector<Vertex_tex>& vertices)
{
	update_vbv_content(vertices, m_dynamic_vertex_buffer, m_dynamic_index_buffer);
}

void GraphicsResources::update_vbv_content(
	std::vector<Vertex_tex>& vertices,
	std::unique_ptr<upload_buffer<Vertex_tex>>& target_vertex_buffer,
	std::unique_ptr<upload_buffer<std::uint16_t>>& target_index_buffer)
{
	auto current_size = vertices.size();
	bool is_safe_to_clear = (m_box_geo && target_vertex_buffer && target_index_buffer);

	if (current_size == 0 && is_safe_to_clear)
	{
		m_box_geo->VertexBufferByteSize = 0;
		m_box_geo->IndexBufferByteSize = 0;
		m_box_geo->index_count = 0;
		target_vertex_buffer->clear_data();
		target_index_buffer->clear_data();
	}
	else
	{
		for (size_t i = 0; i < current_size; ++i)
		{
			m_box_geo->VertexBufferByteSize = current_size * sizeof(Vertex_tex);
			m_box_geo->IndexBufferByteSize = current_size * sizeof(std::uint16_t);
			m_box_geo->index_count = current_size;
			target_vertex_buffer->copy_data(i, vertices[i]);
			target_index_buffer->copy_data(i, i);
		}
	}
}

void GraphicsResources::init_psos()
{
	auto vs = std::string("woodCrateVS");
	auto ps = std::string("woodCratePS");

	create_basic_input_layout();
	create_points_pso(m_shaders[vs], m_shaders[ps]);
	create_triangles_pso(m_shaders[vs], m_shaders[ps]);
	create_lines_pso(m_shaders[vs], m_shaders[ps]);
	execute_cmd_list();
}

void GraphicsResources::update()
{
	using namespace DirectX;

	// Make each pixel correspond to a quarter of a degree
	float mouse_position_delta_x = current_mouse_position.X - last_mouse_position.X;
	float mouse_position_delta_y = current_mouse_position.Y - last_mouse_position.Y;

	float delta_x = XMConvertToRadians(mouse_position_delta_x);
	float delta_y = XMConvertToRadians(mouse_position_delta_y);

	m_theta += delta_x;
	m_phi += delta_y;

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, static_cast<float>(m_output_width) / m_output_height, 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, P);

	float x = m_radius * sinf(m_phi) * cosf(m_theta);
	float z = m_radius * sinf(m_phi) * sinf(m_theta);
	float y = m_radius * cosf(m_phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_view, view);

	XMMATRIX world = XMLoadFloat4x4(&m_world);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX world_view_proj = world * view * proj;

	XMFLOAT4X4 tex_transform_floats = MathHelper::Identity4x4();
	XMMATRIX tex_transforms = XMLoadFloat4x4(&tex_transform_floats);

	object_constants object_constants;
	XMStoreFloat4x4(&object_constants.texture_transform, XMMatrixTranspose(tex_transforms));
	XMStoreFloat4x4(&object_constants.world, XMMatrixTranspose(world_view_proj));

	m_object_cb->copy_data(0, object_constants);
}

void GraphicsResources::render()
{
	winrt::check_hresult(m_cmd_allocator->Reset());
	winrt::check_hresult(m_graphics_cmdlist->Reset(m_cmd_allocator.get(), m_points_pso.get()));

	m_screen_viewport.TopLeftX = 0;
	m_screen_viewport.TopLeftY = 0;
	m_screen_viewport.Width = static_cast<float>(m_output_width);
	m_screen_viewport.Height = static_cast<float>(m_output_height);
	m_screen_viewport.MinDepth = 0.0f;
	m_screen_viewport.MaxDepth = 1.0f;
	m_scissor_rect = { 0, 0, m_output_width, m_output_height };

	m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_swapchain_buffer[m_current_backbuffer].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthstencil_buffer.get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	switch (m_current_rendering_mode) {
	case GraphicsResources::rendering_mode::points:
		m_graphics_cmdlist->SetPipelineState(m_points_pso.get());
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case GraphicsResources::rendering_mode::triangles:
		m_graphics_cmdlist->SetPipelineState(m_triangles_pso.get());
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case GraphicsResources::rendering_mode::lines:
		m_graphics_cmdlist->SetPipelineState(m_lines_pso.get());
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case GraphicsResources::rendering_mode::linestrips:
		m_graphics_cmdlist->SetPipelineState(m_lines_pso.get());
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		break;
	case GraphicsResources::rendering_mode::trianglestrips:
		m_graphics_cmdlist->SetPipelineState(m_triangles_pso.get());
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	default:
		m_graphics_cmdlist->SetPipelineState(m_triangles_pso.get());
		m_graphics_cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	}

	m_graphics_cmdlist->RSSetViewports(1, &m_screen_viewport);
	m_graphics_cmdlist->RSSetScissorRects(1, &m_scissor_rect);
	m_graphics_cmdlist->ClearRenderTargetView(current_backbuffer_view(), DirectX::Colors::Beige, 0, nullptr);
	m_graphics_cmdlist->ClearDepthStencilView(m_dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	m_graphics_cmdlist->OMSetRenderTargets(1, &current_backbuffer_view(), true, &m_dsv_heap->GetCPUDescriptorHandleForHeapStart());

	m_graphics_cmdlist->SetGraphicsRootSignature(m_rootsig.get());
	m_graphics_cmdlist->SetGraphicsRootConstantBufferView(0, m_object_cb->get_resource()->GetGPUVirtualAddress());

	m_graphics_cmdlist->IASetVertexBuffers(0, 1, (D3D12_VERTEX_BUFFER_VIEW*)&vertex_buffer->get_view());
	m_graphics_cmdlist->DrawInstanced(vertex_buffer->current_size(), 1, 0, 0);

	m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_swapchain_buffer[m_current_backbuffer].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	m_graphics_cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthstencil_buffer.get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));

	execute_cmd_list();
	flush_cmd_queue();

	winrt::check_hresult(m_swapchain->Present(0, 0));
	m_current_backbuffer = (m_current_backbuffer + 1) % m_swapchain_buffer_count;
}