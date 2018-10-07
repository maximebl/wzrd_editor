//
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "utilities.h"
#include "DDSTextureLoader.h"
#include "FrameResource.h"
#include "GraphicsResources.h"

namespace winrt::wzrd_editor::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

	private:
		GraphicsResources m_graphics_resources;

	public:
        int32_t MyProperty();
        void MyProperty(int32_t value);

        Windows::Foundation::IAsyncAction texturePicker_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction pixelShaderPicker_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction vertexShaderPicker_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction buildPSO_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		
		winrt::event_token token;
		GameTimer m_timer;
		Windows::UI::Core::CoreWindow m_window = nullptr;
		Windows::UI::Core::CoreWindow m_window_debug = nullptr;
		Windows::ApplicationModel::Core::CoreApplicationView newCoreApplicationView = nullptr;

		int m_main_view_id = 0;
		int m_new_view_id = 0;
		bool m_window_shown = false;

		Windows::Foundation::IAsyncAction m_renderLoopWorker;
		Windows::System::Threading::WorkItemHandler m_render_loop_work_item = nullptr;
		bool m_windowVisible = false;
		double m_current_slider_x = 0;
		double m_current_slider_y = 0;
		double m_current_slider_z = 0;
		double m_current_radius_control = 2.5;
		winrt::Windows::UI::Color m_currentSelectedColor;
		winrt::apartment_context m_ui_thread;
		Windows::Foundation::IAsyncAction ui_thread_work();

		Windows::Foundation::IAsyncAction activate_debug_window();

		bool Render();
		void Update(const GameTimer& gt);
		void UpdateCamera(const GameTimer& gt);
		void UpdateObjectCBs(const GameTimer& gt);
		void UpdateMaterialCBs(const GameTimer& gt);
		void UpdateMainPassCB(const GameTimer& gt);
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<render_item*>& render_items);
		void EnableDebugLayer();
		void CreateCommandObjects();
		void CreateXamlSwapChain();
		void CreateRenderTargets();
		void CreateDescriptorHeaps();
		void CreateDepthStencilBufferAndView();
		void BuildRootSignature();
		void BuildShaderResources();
		void BuildBoxGeometry();
		void BuildPSOs();
		void BuildMaterials();
		void BuildRenderItems();
		void BuildFrameResources();

		// test simplified
		void simple_BuildRootSignature();
		void simple_BuildConstantBuffers();
		void simple_BuildPSOs();
		void simple_BuildGeometry();
		std::unique_ptr<upload_buffer<simple_object_constants>> m_simple_object_cb;
		com_ptr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_srvHeap = nullptr;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_simple_input_layout;
		std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
		///////////////////////////////////////////////////////////////////////
		void WaitForGPU();
		void FlushCommandQueue();

		std::vector<std::unique_ptr<render_item>> m_render_items;
		std::vector<render_item*> m_opaque_render_items;
		std::vector<std::unique_ptr<frame_resource>> m_frame_resources;
		frame_resource* m_current_frame_resource = nullptr;
		int m_current_frame_resource_index = 0;

		std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
		std::unordered_map<std::string, com_ptr<ID3DBlob>> m_shaders;
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		com_ptr<ID3D12DescriptorHeap> m_srvDescriptorHeap = nullptr;

		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

		bool m_running = false;
		int output_width = 700;
		int output_height = 700;

		D3D12_VIEWPORT m_screenViewport;
		D3D12_RECT m_scissorsRect;

		static const int m_swapChainBufferCount = 3; // c_frameCount (Triple buffering)
		int m_currentBackBuffer = 0;
		com_ptr<ID3D12Resource> m_swapChainBuffer[m_swapChainBufferCount];
		com_ptr<ID3D12Resource> m_depthStencilBuffer;

		DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		
		com_ptr<IDXGISwapChain1> m_swapChain;
		com_ptr<IDXGIFactory4> m_factory;
		com_ptr<ID3D12CommandQueue> m_commandQueue;
		com_ptr<ID3D12CommandAllocator> m_commandAllocator;
		com_ptr<ID3D12GraphicsCommandList> m_graphicsCommandList;
		com_ptr<ID3D12Device> m_device;
		com_ptr<ID3D12PipelineState> m_opaque_pso = nullptr;

		com_ptr<ID3D12RootSignature> m_rootSignature = nullptr;

		com_ptr<ID3D12Fence> m_fence;
		UINT64 m_currentFence = 0;

		com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
		com_ptr<ID3D12DescriptorHeap> m_dsvHeap;

		UINT m_rtvDescriptorSize = 0;
		UINT m_dsvDescriptorSize = 0;
		UINT m_cbvSrvUavDescriptorSize = 0;

		pass_constants m_main_pass_constants;
		DirectX::XMFLOAT3 m_eye_position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();

		float m_theta = 1.5f * DirectX::XM_PI;
		float m_phi = 0.4f * DirectX::XM_PIDIV4;
		float m_radius = 5.0f;
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
