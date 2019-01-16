#pragma once

#include "renderer.g.h"
#include "upload_buffer.h"

namespace winrt::graphics::implementation
{
	struct object_constants
	{
		DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 texture_transform = MathHelper::Identity4x4();
	};

	//struct compilation_result 
	//{
	//	bool is_success = false;
	//	hstring error_message = L"";
	//};

	struct renderer : rendererT<renderer>
	{
		renderer();

		void enable_debug_layer();
		void initialize(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain);
		void start_render_loop();
		graphics::buffer current_buffer();
		void current_buffer(graphics::buffer const& value);
		Windows::Foundation::IAsyncOperation<graphics::compilation_result> pick_and_compile_shader(hstring const shader_name, hstring const entry_point, hstring const version);

	private:
		bool m_running = false;
		buffer m_current_buffer;
		constexpr static int m_swapchain_buffer_count = 2;
		UINT m_rtv_descriptor_size = 0;
		constexpr static int m_output_width = 700;
		constexpr static int m_output_height = 700;

		DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_depthstencil_format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;

		com_ptr<IDXGISwapChain1> m_swapchain = nullptr;
		com_ptr<ID3D12CommandQueue> m_cmd_queue = nullptr;
		com_ptr<ID3D12GraphicsCommandList> m_graphics_cmdlist = nullptr;
		com_ptr<ID3D12CommandAllocator> m_cmd_allocator = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_rtv_heap = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_dsv_heap = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_srv_heap = nullptr;
		com_ptr<ID3D12Resource> m_swapchain_buffer[m_swapchain_buffer_count];
		com_ptr<ID3D12Resource> m_depthstencil_buffer;
		com_ptr<ID3D12RootSignature> m_rootsig = nullptr;
		com_ptr<ID3D12Fence> m_gpu_fence = nullptr;
		com_ptr<ID3D12Device> m_device = nullptr;
		com_ptr<IDXGIFactory4> m_dxgi_factory = nullptr;

		// pipeline states
		com_ptr<ID3D12PipelineState> m_points_pso = nullptr;
		com_ptr<ID3D12PipelineState> m_triangles_pso = nullptr;
		com_ptr<ID3D12PipelineState> m_lines_pso = nullptr;

		std::unique_ptr<upload_buffer<object_constants>> m_object_cb;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_basic_input_layout;

		void create_factory();
		void create_device();
		void create_fence();
		void create_cmd_objects();
		void create_descriptor_heaps();
		void create_depthstencil_buffer();
		void create_swapchain_xaml(Windows::UI::Xaml::Controls::SwapChainPanel target_swapchain);
		void create_render_targets();
		void create_constant_buffers();
		void create_rootsignature();
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> get_static_samplers();
		void init_psos();
		void create_basic_input_layout();
		void create_pso(com_ptr<ID3D10Blob> vertex_shader, 
			com_ptr<ID3D10Blob> pixel_shader, 
			D3D12_PRIMITIVE_TOPOLOGY_TYPE topolgy_type, 
			com_ptr<ID3D12PipelineState> m_pso);

		std::pair<graphics::compilation_result, com_ptr<ID3D10Blob>> compile_shader(hstring const& version, const std::vector<unsigned char>& file_bytes, hstring const& entry_point);
		std::unordered_map<std::string, com_ptr<ID3DBlob>> m_shaders;
	};
}

namespace winrt::graphics::factory_implementation
{
	struct renderer : rendererT<renderer, implementation::renderer>
	{
	};
}
