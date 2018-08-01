﻿#include "pch.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::System::Threading;

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
		check_hresult(m_graphicsCommandList->Close());
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
		BuildRootSignature();

		m_running = true;
		m_window = Window::Current().CoreWindow().GetForCurrentThread();

		m_ui_thread = winrt::apartment_context();

		auto workItem = WorkItemHandler([this](Windows::Foundation::IAsyncAction action)
		{
			while (m_running)
			{
				ui_thread_work();

				if (m_windowVisible)
				{
					Render();
					WaitForGPU();
				}
			}
		});

		m_renderLoopWorker = ThreadPool::RunAsync(workItem);
    }

    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
    }

	Windows::Foundation::IAsyncAction MainPage::ui_thread_work()
	{
		// Switch to the UI thread
		co_await m_ui_thread;

		m_windowVisible = m_window.Visible();
		m_currentSelectedColor = colorPicker().Color();

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
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

	bool MainPage::Render()
	{

		check_hresult(m_commandAllocator->Reset());
		check_hresult(m_graphicsCommandList->Reset(m_commandAllocator.get(), nullptr));

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

		//winrt::Windows::UI::Color pickerColor = colorPicker().Color();
		winrt::Windows::UI::Color pickerColor = m_currentSelectedColor;
		FLOAT currentColor[4] = { pickerColor.R / 255.f, pickerColor.G / 255.f, pickerColor.B / 255.f, pickerColor.A / 255.f };

		m_graphicsCommandList->RSSetViewports(1, &m_screenViewport);
		m_graphicsCommandList->RSSetScissorRects(1, &m_scissorsRect);
		m_graphicsCommandList->ClearRenderTargetView(CurrentBackBufferView(), currentColor, 0, nullptr);
		m_graphicsCommandList->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		m_graphicsCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

		m_graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		m_graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));

		check_hresult(m_graphicsCommandList->Close());

		ID3D12CommandList* cmdsLists[] = { m_graphicsCommandList.get() };
		m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		m_swapChain->Present(0, 0);
		m_currentBackBuffer = (m_currentBackBuffer + 1) % m_swapChainBufferCount;

		// Set the new CPU fence point
		m_currentFence++;

		// Add an instruction to the command queue to set a new fence point. 
		// Because we are on the GPU timeline, the new fence point won't be 
		// set until the GPU finishes processing all the commands prior to this Signal().
		check_hresult(m_commandQueue->Signal(m_fence.get(), m_currentFence));

		return true;
	}
}
