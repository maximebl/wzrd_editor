#include "pch.h"
#include "MainPage.h"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::System::Threading;

namespace winrt::wzrd_editor::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
		EnableDebugLayer();

		check_hresult(
			CreateDXGIFactory1(__uuidof(m_factory), m_factory.put_void())
		);

		check_hresult(
			D3D12CreateDevice(
				nullptr,
				D3D_FEATURE_LEVEL_11_0,
				__uuidof(m_device),
				m_device.put_void()
			)
		);

		check_hresult(
			m_device->CreateFence(
				0,
				D3D12_FENCE_FLAG_NONE,
				__uuidof(m_fence),
				m_fence.put_void()
			)
		);

		CreateCommandObjects();

		CreateAndAssociateSwapChain();
		CreateDescriptorHeaps();

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < m_swapChainBufferCount; i++)
		{
			check_hresult(
				m_swapChain->GetBuffer(i, __uuidof(m_swapChainBuffer[i].get()), m_swapChainBuffer[i].put_void())
			);
			m_device->CreateRenderTargetView(m_swapChainBuffer[i].get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
		}

		CreateDepthStencilBufferAndView();

		m_simple_input_layout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		m_inputLayout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//BuildRootSignature();
		simple_BuildConstantBuffers();
		simple_BuildRootSignature();
		simple_BuildGeometry();
		//BuildBoxGeometry();
		//BuildMaterials();
		//BuildRenderItems();
		//BuildFrameResources();

		check_hresult(m_graphicsCommandList->Close());
		ID3D12CommandList* cmdsLists[] = { m_graphicsCommandList.get() };
		m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		m_running = true;
		m_window = Window::Current().CoreWindow().GetForCurrentThread();

		m_ui_thread = winrt::apartment_context();

		m_render_loop_work_item = WorkItemHandler([this](Windows::Foundation::IAsyncAction action)
		{
			m_timer.Reset();

			while (m_running)
			{
				ui_thread_work();
				m_timer.Tick();

				if (m_windowVisible)
				{
					Update(m_timer);
					Render();
				}
			}
		});

    }

    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    Windows::Foundation::IAsyncAction MainPage::texturePicker_Click(IInspectable const&, RoutedEventArgs const&)
    {
		// Pick and read the file data
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".dds");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}

		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto dataReader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer);

		std::vector<unsigned char> file_bytes;
		int fileSize = fileBuffer.Length();
		file_bytes.assign(fileSize, 0);
		dataReader.ReadBytes(file_bytes);
		
		// Create the texture 
		auto woodCrateTexture = std::make_unique<Texture>();
		woodCrateTexture->Name = "woodCrateTexture";

		Microsoft::WRL::ComPtr<ID3D12Resource> tmpResource = woodCrateTexture->Resource.get();
		Microsoft::WRL::ComPtr<ID3D12Resource> tmpUploadHeap = woodCrateTexture->UploadHeap.get();

		check_hresult(m_graphicsCommandList->Reset(m_commandAllocator.get(), nullptr));
		// Create the dds texture
		check_hresult(
			DirectX::CreateDDSTextureFromMemory12(
				m_device.get(),
				m_graphicsCommandList.get(),
				&file_bytes.front(),
				fileSize,
				tmpResource,
				tmpUploadHeap
			)
		);

		woodCrateTexture->Resource.attach(tmpResource.Get());
		woodCrateTexture->UploadHeap.attach(tmpUploadHeap.Get());

		m_textures[woodCrateTexture->Name] = std::move(woodCrateTexture);

		check_hresult(m_graphicsCommandList->Close());

		BuildShaderResources();
    }

	Windows::Foundation::IAsyncAction MainPage::pixelShaderPicker_Click(IInspectable const&, RoutedEventArgs const&)
	{
		// Pick and read the shader data
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".hlsl");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}

		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto file_bytes = Utilities::read_shader_file(fileBuffer).get();
		m_shaders["woodCratePS"] = Utilities::compile_shader("ps_5_0", file_bytes, "PS");
	}

	Windows::Foundation::IAsyncAction MainPage::vertexShaderPicker_Click(IInspectable const&, RoutedEventArgs const&)
	{
		// Pick and read the shader data
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".hlsl");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}

		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		//auto dataReader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer);

		//std::vector<unsigned char> file_bytes;
		//int fileSize = fileBuffer.Length();
		//file_bytes.assign(fileSize, 0);
		//dataReader.ReadBytes(file_bytes);
		auto file_bytes = Utilities::read_shader_file(fileBuffer).get();
		m_shaders["woodCrateVS"] = Utilities::compile_shader("vs_5_0", file_bytes, "VS");
	}

	void MainPage::buildPSO_Click(IInspectable const&, RoutedEventArgs const&)
	{
		//BuildPSOs();
		simple_BuildPSOs();
		m_renderLoopWorker = ThreadPool::RunAsync(m_render_loop_work_item);
	}

	void MainPage::simple_BuildGeometry()
	{
		using namespace DirectX;

		std::array<Vertex, 8> vertices =
		{
			Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
			Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
		};

		std::array<std::uint16_t, 36> indices =
		{
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		mBoxGeo = std::make_unique<MeshGeometry>();
		mBoxGeo->Name = "boxGeo";

		check_hresult(D3DCreateBlob(vbByteSize, mBoxGeo->VertexBufferCPU.put()));
		CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		check_hresult(D3DCreateBlob(ibByteSize, mBoxGeo->IndexBufferCPU.put()));
		CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		mBoxGeo->VertexBufferGPU = Utilities::create_default_buffer(m_device.get(),
			m_graphicsCommandList.get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

		mBoxGeo->IndexBufferGPU = Utilities::create_default_buffer(m_device.get(),
			m_graphicsCommandList.get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

		mBoxGeo->VertexByteStride = sizeof(Vertex);
		mBoxGeo->VertexBufferByteSize = vbByteSize;
		mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
		mBoxGeo->IndexBufferByteSize = ibByteSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		mBoxGeo->DrawArgs["box"] = submesh;
	}

	void MainPage::BuildBoxGeometry()
	{
		//GeometryGenerator geoGen;
		//GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);

		//SubmeshGeometry boxSubmesh;
		//boxSubmesh.IndexCount = (UINT)box.Indices32.size();
		//boxSubmesh.StartIndexLocation = 0;
		//boxSubmesh.BaseVertexLocation = 0;

		//std::vector<Vertex> vertices(box.Vertices.size());
		//
		//for (size_t i = 0; i < box.Vertices.size(); ++i)
		//{
		//	vertices[i].Pos = box.Vertices[i].Position;
		//	vertices[i].Normal = box.Vertices[i].Normal;
		//	vertices[i].TexC = box.Vertices[i].TexC;
		//}

		//std::vector<std::uint16_t> indices = box.GetIndices16();

		//const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		//const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);
		//
		//auto geo = std::make_unique<MeshGeometry>();
		//geo->Name = "crateGeo";

		//// TODO:: Bechmark those 2 methods to see whats the fastest
		////auto bufferPtr = reinterpret_cast<Vertex *>(geo->VertexBufferCPU->GetBufferPointer());
		////std::copy(vertices.begin(), vertices.end(), bufferPtr);
		//check_hresult(D3DCreateBlob(vbByteSize, geo->VertexBufferCPU.put()));
		//std::memcpy(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		//check_hresult(D3DCreateBlob(ibByteSize, geo->IndexBufferCPU.put()));
		//std::memcpy(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		//geo->VertexBufferGPU = Utilities::create_default_buffer(m_device.get(),
		//	m_graphicsCommandList.get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

		//geo->IndexBufferGPU = Utilities::create_default_buffer(m_device.get(),
		//	m_graphicsCommandList.get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		//geo->VertexByteStride = sizeof(Vertex);
		//geo->VertexBufferByteSize = vbByteSize;
		//geo->IndexBufferByteSize = ibByteSize;
		//geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		//
		//geo->DrawArgs["box"] = boxSubmesh;
		//
		//m_geometries[geo->Name] = std::move(geo);
	}

	void MainPage::BuildMaterials()
	{
		auto woodCrate = std::make_unique<Material>();
		woodCrate->name = "woodCrate";
		woodCrate->mat_cb_index = 0;
		woodCrate->diffuse_srv_heap_index = 0;
		woodCrate->diffuse_albedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		woodCrate->fresnel_r0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
		woodCrate->roughness = 0.2f;

		m_materials[woodCrate->name] = std::move(woodCrate);
	}

	void MainPage::BuildRenderItems()
	{
		auto box_render_item = std::make_unique<render_item>();
		box_render_item->obj_cb_index = 0;
		box_render_item->material = m_materials["woodCrate"].get();
		box_render_item->mesh_geometry = m_geometries["crateGeo"].get();
		box_render_item->primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		box_render_item->index_count = box_render_item->mesh_geometry->DrawArgs["box"].IndexCount;
		box_render_item->start_index_location = box_render_item->mesh_geometry->DrawArgs["box"].StartIndexLocation;
		box_render_item->base_vertex_location = box_render_item->mesh_geometry->DrawArgs["box"].BaseVertexLocation;

		m_render_items.push_back(std::move(box_render_item));

		for (auto& render_item : m_render_items)
		{
			m_opaque_render_items.push_back(render_item.get());
		}
	}

	void MainPage::BuildFrameResources()
	{
		for (int i = 0; i < global_num_frame_resources; ++i)
		{
			m_frame_resources.push_back(std::make_unique<frame_resource>(m_device.get(),
				1, (UINT)m_render_items.size(), (UINT)m_materials.size()));
		}
	}

	void MainPage::simple_BuildPSOs()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaque_pso_desc;
		ZeroMemory(&opaque_pso_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		opaque_pso_desc.InputLayout = { m_simple_input_layout.data(), (UINT)m_simple_input_layout.size() };
		opaque_pso_desc.pRootSignature = m_rootSignature.get();
		opaque_pso_desc.VS =
		{
			reinterpret_cast<unsigned char*>(m_shaders["woodCrateVS"]->GetBufferPointer()),
			m_shaders["woodCrateVS"]->GetBufferSize()
		};
		opaque_pso_desc.PS =
		{
			reinterpret_cast<unsigned char*>(m_shaders["woodCratePS"]->GetBufferPointer()),
			m_shaders["woodCratePS"]->GetBufferSize()
		};
		opaque_pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaque_pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaque_pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaque_pso_desc.SampleMask = UINT_MAX;
		opaque_pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaque_pso_desc.NumRenderTargets = 1;
		opaque_pso_desc.RTVFormats[0] = m_backBufferFormat;
		opaque_pso_desc.SampleDesc.Count = 1;
		opaque_pso_desc.SampleDesc.Quality = 0;
		opaque_pso_desc.DSVFormat = m_depthStencilFormat;

		check_hresult(m_device->CreateGraphicsPipelineState(&opaque_pso_desc, __uuidof(m_opaque_pso), m_opaque_pso.put_void()));
	}

	void MainPage::BuildPSOs()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaque_pso_desc;
		ZeroMemory(&opaque_pso_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		opaque_pso_desc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
		opaque_pso_desc.pRootSignature = m_rootSignature.get();
		opaque_pso_desc.VS = 
		{
			reinterpret_cast<unsigned char*>(m_shaders["woodCrateVS"]->GetBufferPointer()),
			m_shaders["woodCrateVS"]->GetBufferSize()
		};
		opaque_pso_desc.PS =
		{
			reinterpret_cast<unsigned char*>(m_shaders["woodCratePS"]->GetBufferPointer()),
			m_shaders["woodCratePS"]->GetBufferSize()
		};
		opaque_pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaque_pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaque_pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaque_pso_desc.SampleMask = UINT_MAX;
		opaque_pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaque_pso_desc.NumRenderTargets = 1;
		opaque_pso_desc.RTVFormats[0] = m_backBufferFormat;
		opaque_pso_desc.SampleDesc.Count = 1;
		opaque_pso_desc.SampleDesc.Quality = 0;
		opaque_pso_desc.DSVFormat = m_depthStencilFormat;

		check_hresult(m_device->CreateGraphicsPipelineState(&opaque_pso_desc, __uuidof(m_opaque_pso), m_opaque_pso.put_void()));
	}

	Windows::Foundation::IAsyncAction MainPage::ui_thread_work()
	{
		// Switch to the UI thread
		co_await m_ui_thread;

		m_windowVisible = m_window.Visible();
		m_currentSelectedColor = colorPicker().Color();
		m_current_slider_x = slider_x().Value();
		m_current_slider_y = slider_y().Value();
		m_current_slider_z = slider_z().Value();
		m_current_radius_control = slider_radius().Value();

		// Leaving this function switches back to the rendering thread
	}

	void MainPage::EnableDebugLayer()
	{
		com_ptr<ID3D12Debug> debugController;
		check_hresult(
			D3D12GetDebugInterface(
				__uuidof(debugController), debugController.put_void()
			)
		);
		debugController->EnableDebugLayer();
	}

	void MainPage::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		check_hresult(
			m_device->CreateCommandQueue(
				&commandQueueDesc,
				__uuidof(m_commandQueue),
				m_commandQueue.put_void())
		);

		check_hresult(
			m_device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				__uuidof(m_commandAllocator),
				m_commandAllocator.put_void()
			)
		);

		check_hresult(
			m_device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_commandAllocator.get(),
				nullptr,
				__uuidof(m_graphicsCommandList),
				m_graphicsCommandList.put_void()
			)
		);
	}

	void MainPage::CreateAndAssociateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = output_width;
		swapChainDesc.Height = output_height;
		swapChainDesc.Format = m_backBufferFormat;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_swapChainBufferCount; // triple buffering
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		check_hresult(
			m_factory->CreateSwapChainForComposition(
				m_commandQueue.get(),
				&swapChainDesc,
				nullptr,
				m_swapChain.put()
			)
		);

		// associate DXGI swap chain with the XAML SwapChainPanel
		check_hresult(
			swapChainPanel().as<ISwapChainPanelNative>()->SetSwapChain(m_swapChain.get())
		);
	}

	void MainPage::CreateDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = m_swapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		check_hresult(
			m_device->CreateDescriptorHeap(
				&rtvHeapDesc,
				__uuidof(m_rtvHeap),
				m_rtvHeap.put_void()
			)
		);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		check_hresult(
			m_device->CreateDescriptorHeap(
				&dsvHeapDesc,
				__uuidof(m_dsvHeap),
				m_dsvHeap.put_void()
			)
		);

/*		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;

		check_hresult(
			m_device->CreateDescriptorHeap(&cbvHeapDesc, __uuidof(m_cbvHeap), m_cbvHeap.put_void())
		)*/;
	}

	void MainPage::CreateDepthStencilBufferAndView()
	{
		D3D12_RESOURCE_DESC depthStencilDesc = {};
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = output_width;
		depthStencilDesc.Height = output_height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = m_depthStencilFormat;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		check_hresult(
			m_device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&depthStencilDesc,
				D3D12_RESOURCE_STATE_COMMON,
				&clearValue,
				__uuidof(m_depthStencilBuffer),
				m_depthStencilBuffer.put_void()
			)
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = m_depthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		m_device->CreateDepthStencilView(m_depthStencilBuffer.get(), &dsvDesc, GetDepthStencilView());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE MainPage::GetDepthStencilView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_dsvHeap->GetCPUDescriptorHandleForHeapStart()
		);
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> MainPage::GetStaticSamplers()
	{
		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8);                               // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8);                                // maxAnisotropy

		return {
			pointWrap, pointClamp,
			linearWrap, linearClamp,
			anisotropicWrap, anisotropicClamp };
	}

	void MainPage::BuildShaderResources()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		check_hresult(m_device->CreateDescriptorHeap(&srvHeapDesc, __uuidof(m_srvDescriptorHeap), m_srvDescriptorHeap.put_void()));

		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		auto woodcrateTexture = m_textures["woodCrateTexture"]->Resource;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = woodcrateTexture->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = woodcrateTexture->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_device->CreateShaderResourceView(woodcrateTexture.get(), &srvDesc, hDescriptor);
	}

	void MainPage::simple_BuildRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE srvTable;
		srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER slotRootParameter[2];
		slotRootParameter[0].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		com_ptr<ID3DBlob> serializedRootSig = nullptr;
		com_ptr<ID3DBlob> errorBlob = nullptr;
		D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.put(), errorBlob.put());

		m_device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), __uuidof(m_rootSignature), m_rootSignature.put_void());
	}

	void MainPage::simple_BuildConstantBuffers()
	{

		m_simple_object_cb = std::make_unique<upload_buffer<simple_object_constants>>(m_device.get(), 1, true);

		// Unnecessary because its already initialized as a ConstantBufferView in the root parameters

		//UINT objCBByteSize = Utilities::constant_buffer_byte_size(sizeof(simple_object_constants));
		//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_simple_object_cb->get_resource()->GetGPUVirtualAddress();
		//int boxCBufIndex = 0;
		//cbAddress += boxCBufIndex * objCBByteSize;
		//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		//cbvDesc.BufferLocation = cbAddress;
		//cbvDesc.SizeInBytes = Utilities::constant_buffer_byte_size(sizeof(simple_object_constants));

		//m_device->CreateConstantBufferView(
		//	&cbvDesc,
		//	m_cbvHeap->GetCPUDescriptorHandleForHeapStart()
		//);
	}

	void MainPage::BuildRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER slotRootParameter[4];
		slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[1].InitAsConstantBufferView(0);
		slotRootParameter[2].InitAsConstantBufferView(1);
		slotRootParameter[3].InitAsConstantBufferView(2);

		auto staticSamplers = GetStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(4, slotRootParameter, 
			(UINT)staticSamplers.size(), staticSamplers.data(), 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		com_ptr<ID3DBlob> serializedRootSig = nullptr;
		com_ptr<ID3DBlob> errorBlob = nullptr;
		check_hresult(
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.put(), errorBlob.put()
		));

		check_hresult(
			m_device->CreateRootSignature(
				0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				__uuidof(m_rootSignature),
				m_rootSignature.put_void()
			)
		);
	}

	void MainPage::WaitForGPU()
	{
		// If the GPU's completed fence is not as high as the fence of the CPU, wait for the GPU to arrive at the latest fence point.
		//if (m_current_frame_resource->fence != 0 && m_fence->GetCompletedValue() < m_current_frame_resource->fence)
		//{
			//HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			//check_hresult(m_fence->SetEventOnCompletion(m_current_frame_resource->fence, eventHandle));
			//WaitForSingleObject(eventHandle, INFINITE);
			//CloseHandle(eventHandle);
		//}
	}

	void MainPage::FlushCommandQueue()
	{
		m_currentFence++;
		check_hresult(m_commandQueue->Signal(m_fence.get(), m_currentFence));

		if (m_fence->GetCompletedValue() < m_currentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			check_hresult(m_fence->SetEventOnCompletion(m_currentFence, eventHandle));

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	ID3D12Resource* MainPage::CurrentBackBuffer() const
	{
		return m_swapChainBuffer[m_currentBackBuffer].get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE MainPage::CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_currentBackBuffer,
			m_rtvDescriptorSize
		);
	}

	void MainPage::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<render_item*>& render_items)
	{
		UINT obj_cb_byte_size = Utilities::constant_buffer_byte_size(sizeof(object_constants));
		UINT mat_cb_byte_size = Utilities::constant_buffer_byte_size(sizeof(material_constants));

		auto object_cb = m_current_frame_resource->object_cb->get_resource();
		auto mat_cb = m_current_frame_resource->material_cb->get_resource();

		for (size_t i = 0; i < m_render_items.size(); ++i)
		{
			auto render_item = render_items[i];

			cmdList->IASetVertexBuffers(0, 1, &render_item->mesh_geometry->VertexBufferView());
			cmdList->IASetIndexBuffer(&render_item->mesh_geometry->IndexBufferView());
			cmdList->IASetPrimitiveTopology(render_item->primitive_topology);

			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			tex.Offset(render_item->material->diffuse_srv_heap_index, m_cbvSrvUavDescriptorSize);

			D3D12_GPU_VIRTUAL_ADDRESS obj_cb_address = object_cb->GetGPUVirtualAddress() + (render_item->obj_cb_index * obj_cb_byte_size);
			D3D12_GPU_VIRTUAL_ADDRESS mat_cb_address = mat_cb->GetGPUVirtualAddress() + (render_item->material->mat_cb_index * mat_cb_byte_size);

			cmdList->SetGraphicsRootDescriptorTable(0, tex);
			cmdList->SetGraphicsRootConstantBufferView(1, obj_cb_address);
			cmdList->SetGraphicsRootConstantBufferView(3, mat_cb_address);

			cmdList->DrawIndexedInstanced(render_item->index_count, 1, render_item->start_index_location, render_item->base_vertex_location, 0);
		}
	}

	void MainPage::UpdateObjectCBs(const GameTimer& gt)
	{
		using namespace DirectX;

		auto current_object_cb = m_current_frame_resource->object_cb.get();

		for (auto& render_item : m_render_items)
		{
			if (render_item->num_frames_dirty > 0)
			{
				XMMATRIX world = XMLoadFloat4x4(&render_item->world);
				XMMATRIX tex_transform = XMLoadFloat4x4(&render_item->tex_transform);

				object_constants object_constants;
				XMStoreFloat4x4(&object_constants.world, XMMatrixTranspose(world));
				XMStoreFloat4x4(&object_constants.texture_transform, XMMatrixTranspose(tex_transform));

				current_object_cb->copy_data(render_item->obj_cb_index, object_constants);

				render_item->num_frames_dirty--;
			}
		}
	}

	void MainPage::UpdateMaterialCBs(const GameTimer& gt)
	{
		using namespace DirectX;

		auto current_material_cb = m_current_frame_resource->material_cb.get();

		for (auto& material : m_materials)
		{
			Material* mat_ptr = material.second.get();
			if (mat_ptr->num_frames_dirty > 0)
			{
				XMMATRIX mat_transform = XMLoadFloat4x4(&mat_ptr->mat_transform);

				material_constants material_constants;
				material_constants.diffuse_albedo = mat_ptr->diffuse_albedo;
				material_constants.fresnel_r0 = mat_ptr->fresnel_r0;
				material_constants.roughness = mat_ptr->roughness;
				XMStoreFloat4x4(&material_constants.mat_transform, XMMatrixTranspose(mat_transform));

				current_material_cb->copy_data(mat_ptr->mat_cb_index, material_constants);

				mat_ptr->num_frames_dirty--;
			}
		}
	}

	void MainPage::UpdateMainPassCB(const GameTimer& gt)
	{
		using namespace DirectX;

		XMMATRIX view = XMLoadFloat4x4(&m_view);
		XMMATRIX proj = XMLoadFloat4x4(&m_proj);

		XMMATRIX view_proj = XMMatrixMultiply(view, proj);
		XMMATRIX inv_view = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX inv_proj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX inv_view_proj = XMMatrixInverse(&XMMatrixDeterminant(view_proj), view_proj);

		XMStoreFloat4x4(&m_main_pass_constants.view, XMMatrixTranspose(view));
		XMStoreFloat4x4(&m_main_pass_constants.inv_view, XMMatrixTranspose(inv_view));
		XMStoreFloat4x4(&m_main_pass_constants.proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&m_main_pass_constants.inv_proj, XMMatrixTranspose(inv_proj));
		XMStoreFloat4x4(&m_main_pass_constants.view_proj, XMMatrixTranspose(view_proj));
		XMStoreFloat4x4(&m_main_pass_constants.inv_view_proj, XMMatrixTranspose(inv_view_proj));

		m_main_pass_constants.eye_pos_w = m_eye_position;
		m_main_pass_constants.render_target_size = XMFLOAT2((float)output_width, (float)output_height);
		m_main_pass_constants.inv_render_target_size = XMFLOAT2(1.0f / output_width, 1.0f / output_height);
		m_main_pass_constants.near_z = 1.0f;
		m_main_pass_constants.far_z = 1000.0f;
		m_main_pass_constants.total_time = gt.TotalTime();
		m_main_pass_constants.delta_time = gt.DeltaTime();
		m_main_pass_constants.ambient_light = { 0.25f, 0.25f, 0.35f, 1.0f };
		m_main_pass_constants.lights[0].direction = { 0.57735f, -0.57735f, 0.57735f };
		m_main_pass_constants.lights[0].strength = { 0.6f, 0.6f, 0.6f };
		m_main_pass_constants.lights[1].direction = { -0.57735f, -0.57735f, 0.57735f };
		m_main_pass_constants.lights[1].strength = { 0.3f, 0.3f, 0.3f };
		m_main_pass_constants.lights[2].direction = { 0.0f, -0.707f, -0.707f };
		m_main_pass_constants.lights[2].strength = { 0.15f, 0.15f, 0.15f };

		auto current_pass_cb = m_current_frame_resource->pass_cb.get();
		current_pass_cb->copy_data(0, m_main_pass_constants);
	}

	void MainPage::UpdateCamera(const GameTimer& gt)
	{
		using namespace DirectX;

		// Convert spherical to cartesian coordinates.
		m_eye_position.x = m_radius * sinf(m_phi) * cosf(m_theta);
		m_eye_position.z = m_radius * sinf(m_phi) * sinf(m_theta);
		m_eye_position.y = m_radius * cosf(m_phi);

		// Build the view matrix.
		XMVECTOR position = XMVectorSet(m_eye_position.x, m_eye_position.y, m_eye_position.z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookAtLH(position, target, up);
		XMStoreFloat4x4(&m_view, view);

		XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, static_cast<float>(output_width) / output_height, 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_proj, proj);
	}

	void MainPage::Update(const GameTimer& gt)
	{
		using namespace DirectX;

		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, static_cast<float>(output_width) / output_height, 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_proj, P);

		m_radius = m_current_radius_control;

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
		XMMATRIX worldViewProj = world * view * proj;

		XMFLOAT4X4 texTransform_floats = MathHelper::Identity4x4();
		XMMATRIX texTransform = XMLoadFloat4x4(&texTransform_floats);

		simple_object_constants objConstants;
		XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
		XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
		m_simple_object_cb->copy_data(0, objConstants);
		//UpdateCamera(gt);

		//m_current_frame_resource_index = (m_current_frame_resource_index + 1) % global_num_frame_resources;
		//m_current_frame_resource = m_frame_resources[m_current_frame_resource_index].get();

		//FlushCommandQueue();
		//WaitForGPU();

		//UpdateObjectCBs(gt);
		//UpdateMaterialCBs(gt);
		//UpdateMainPassCB(gt);
	}

	bool MainPage::Render()
	{
		FlushCommandQueue();

		/*auto cmd_list_alloc = m_current_frame_resource->cmd_list_allocator;

		check_hresult(cmd_list_alloc->Reset());
		check_hresult(m_graphicsCommandList->Reset(cmd_list_alloc.get(), m_opaque_pso.get()));*/
		check_hresult(m_commandAllocator->Reset());
		check_hresult(m_graphicsCommandList->Reset(m_commandAllocator.get(), m_opaque_pso.get()));

		m_screenViewport.TopLeftX = 0;
		m_screenViewport.TopLeftY = 0;
		m_screenViewport.Width = static_cast<float>(output_width);
		m_screenViewport.Height = static_cast<float>(output_height);
		m_screenViewport.MinDepth = 0.0f;
		m_screenViewport.MaxDepth = 1.0f;

		m_scissorsRect = { 0, 0, output_width, output_height };

		m_graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		m_graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_depthStencilBuffer.get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		winrt::Windows::UI::Color pickerColor = m_currentSelectedColor;
		FLOAT currentColor[4] = { pickerColor.R / 255.f, pickerColor.G / 255.f, pickerColor.B / 255.f, pickerColor.A / 255.f };

		m_graphicsCommandList->RSSetViewports(1, &m_screenViewport);
		m_graphicsCommandList->RSSetScissorRects(1, &m_scissorsRect);
		m_graphicsCommandList->ClearRenderTargetView(CurrentBackBufferView(), currentColor, 0, nullptr);
		m_graphicsCommandList->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		m_graphicsCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_srvDescriptorHeap.get() };
		m_graphicsCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		m_graphicsCommandList->SetGraphicsRootSignature(m_rootSignature.get());
		
		m_graphicsCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
		m_graphicsCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
		m_graphicsCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_graphicsCommandList->SetGraphicsRootDescriptorTable(0, m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		m_graphicsCommandList->SetGraphicsRootConstantBufferView(1, m_simple_object_cb->get_resource()->GetGPUVirtualAddress());

		m_graphicsCommandList->DrawIndexedInstanced(
			mBoxGeo->DrawArgs["box"].IndexCount,
			1, 0, 0, 0);
		//auto pass_cb = m_current_frame_resource->pass_cb->get_resource();
		//m_graphicsCommandList->SetGraphicsRootConstantBufferView(2, pass_cb->GetGPUVirtualAddress());
		//DrawRenderItems(m_graphicsCommandList.get(), m_opaque_render_items);

		m_graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));

		check_hresult(m_graphicsCommandList->Close());

		FlushCommandQueue();

		ID3D12CommandList* cmdsLists[] = { m_graphicsCommandList.get() };
		m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		check_hresult(m_swapChain->Present(0, 0));
		m_currentBackBuffer = (m_currentBackBuffer + 1) % m_swapChainBufferCount;

		// Set the new CPU fence point
		//m_current_frame_resource->fence = ++m_currentFence;

		// Add an instruction to the command queue to set a new fence point. 
		// Because we are on the GPU timeline, the new fence point won't be 
		// set until the GPU finishes processing all the commands prior to this Signal().
		check_hresult(m_commandQueue->Signal(m_fence.get(), m_currentFence));

		return true;
	}
}
