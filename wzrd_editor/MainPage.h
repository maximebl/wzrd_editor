//
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "utilities.h"
#include "DDSTextureLoader.h"

namespace winrt::wzrd_editor::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);
        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		Windows::UI::Core::CoreWindow m_window = nullptr;
		Windows::Foundation::IAsyncAction m_renderLoopWorker;
		bool m_windowVisible = false;
		winrt::Windows::UI::Color m_currentSelectedColor;
		winrt::apartment_context m_ui_thread;
		Windows::Foundation::IAsyncAction ui_thread_work();

		bool Render();
		void EnableDebugLayer();
		void CreateCommandObjects();
		void CreateAndAssociateSwapChain();
		void CreateDescriptorHeaps();
		void CreateDepthStencilBufferAndView();
		void WaitForGPU();
		void BuildRootSignature();
		void LoadTextures();

		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

		bool m_running = false;
		int output_width = 500;
		int output_height = 500;

		D3D12_VIEWPORT m_screenViewport;
		D3D12_RECT m_scissorsRect;

		static const int m_swapChainBufferCount = 3; // c_frameCount (Triple buffering)
		int m_currentBackBuffer = 0;
		com_ptr<ID3D12Resource> m_swapChainBuffer[m_swapChainBufferCount];
		com_ptr<ID3D12Resource> m_depthStencilBuffer;
		DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		com_ptr<IDXGISwapChain1> m_swapChain;
		com_ptr<IDXGIFactory4> m_factory;
		com_ptr<ID3D12CommandQueue> m_commandQueue;
		com_ptr<ID3D12CommandAllocator> m_commandAllocator;
		com_ptr<ID3D12GraphicsCommandList> m_graphicsCommandList;
		com_ptr<ID3D12Device> m_device;

		com_ptr<ID3D12RootSignature> m_rootSignature = nullptr;

		com_ptr<ID3D12Fence> m_fence;
		UINT64 m_currentFence = 0;

		com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
		com_ptr<ID3D12DescriptorHeap> m_dsvHeap;

		UINT m_rtvDescriptorSize = 0;
		UINT m_dsvDescriptorSize = 0;
		UINT m_cbvSrvUavDescriptorSize = 0;
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
