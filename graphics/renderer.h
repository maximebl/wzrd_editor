#pragma once

#include "renderer.g.h"
#include "upload_buffer.h"
#include "buffer.h"
#include "utilities.h"
#include "texture.h"
#include "shader.h"
#include "../wzrd_editor/DDSTextureLoader.h"

namespace winrt::graphics::implementation
{
	struct object_constants
	{
		DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 texture_transform = MathHelper::Identity4x4();
	};

	struct renderer : rendererT<renderer>
	{
		renderer();

		void enable_debug_layer();
		void initialize_buffers_showcase(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain);
		void initialize_textures_showcase(
			Windows::Foundation::Collections::IMap<hstring,
			Windows::Foundation::IInspectable> const& ui_items,
			Windows::Foundation::Collections::IMap<hstring, float> const& ui_item_values);
		void start_render_loop();
		void stop_render_loop();
		void clear_shaders();
		void clear_textures();
		void remove_texture(hstring name);
		void remove_shader(hstring name);
		bool is_rendering();
		graphics::buffer current_buffer();
		void current_buffer(graphics::buffer const& value);
		Windows::Foundation::IAsyncOperation<graphics::compilation_result> pick_and_compile_shader(graphics::shader new_shader);

		Windows::Foundation::IAsyncOperation<graphics::texture> pick_texture(graphics::texture new_texture, hstring name);
		graphics::primitive_types current_topology();
		void current_topology(graphics::primitive_types const& value);

		static ID3D12Device* g_device;
		static ID3D12GraphicsCommandList* g_cmd_list;

	private:
		Windows::UI::Xaml::Controls::SwapChainPanel m_swapchain_panel;
		Windows::Foundation::Collections::IMap<hstring, float> m_ui_item_values = nullptr;
		winrt::apartment_context m_ui_thread;

		bool m_is_rendering = false;
		com_ptr<buffer> m_current_buffer = nullptr;
		constexpr static int m_swapchain_buffer_count = 2;
		int m_current_backbuffer = 0;
		UINT m_rtv_descriptor_size = 0;

		constexpr static int m_output_width = 700;
		constexpr static int m_output_height = 700;
		D3D12_VIEWPORT m_screen_viewport;
		D3D12_RECT m_scissor_rect;

		DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
		float m_theta = 1.5f * DirectX::XM_PI;
		float m_phi = 0.4f * DirectX::XM_PIDIV4;
		float m_radius = 5.0f;

		DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_depthstencil_format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		graphics::primitive_types m_current_topology = graphics::primitive_types::triangle_strips;

		com_ptr<IDXGISwapChain1> m_swapchain = nullptr;
		com_ptr<ID3D12CommandQueue> m_cmd_queue = nullptr;
		com_ptr<ID3D12GraphicsCommandList> m_graphics_cmdlist = nullptr;
		com_ptr<ID3D12CommandAllocator> m_cmd_allocator = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_rtv_heap = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_dsv_heap = nullptr;
		com_ptr<ID3D12DescriptorHeap> m_srv_heap = nullptr;

		com_ptr<ID3D12DescriptorHeap> m_sampler_heap = nullptr;
		D3D12_SAMPLER_DESC m_sampler_desc = {};

		com_ptr<ID3D12Resource> m_swapchain_buffer[m_swapchain_buffer_count];
		com_ptr<ID3D12Resource> m_depthstencil_buffer;
		com_ptr<ID3D12Resource> m_checkerboard_texture = nullptr;
		com_ptr<ID3D12Resource> m_crate_texture = nullptr;
		com_ptr<ID3D12RootSignature> m_rootsig = nullptr;
		com_ptr<ID3D12Device> m_device = nullptr;
		com_ptr<IDXGIFactory4> m_dxgi_factory = nullptr;

		// pipeline states
		com_ptr<ID3D12PipelineState> m_points_pso = nullptr;
		com_ptr<ID3D12PipelineState> m_triangles_pso = nullptr;
		com_ptr<ID3D12PipelineState> m_lines_pso = nullptr;
		com_ptr<ID3D12PipelineState> m_billboard_pso = nullptr;

		com_ptr<ID3D12Resource> m_texture_upload_buffer = nullptr;
		com_ptr<ID3D12Resource> m_cb_texcoord_upload_buffer = nullptr;
		unsigned char* m_mapped_texcoord_data = nullptr;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_basic_input_layout;

		void create_factory();
		void create_device();
		void create_fence();
		void create_cmd_objects();
		void create_dsv_heap();
		void create_rtv_heap();
		void create_srv_heap();
		void create_sampler_heap();
		void create_depthstencil_buffer();
		void create_swapchain_xaml(Windows::UI::Xaml::Controls::SwapChainPanel target_swapchain);
		void create_render_targets();
		void create_empty_rootsignature(std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers);
		void create_texture_rootsignature(std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers);
		void create_simple_triangle();
		void create_point();
		void create_texture_srv();
		void create_cb_texcoord();
		std::vector<UINT8> generate_texture_data(UINT texture_width, UINT texture_height, UINT texture_pixel_size);
		std::vector<CD3DX12_STATIC_SAMPLER_DESC> get_static_samplers();
		void init_psos();
		void execute_cmd_list();
		void create_basic_input_layout();
		void select_primitive_topology();
		void create_pso(com_ptr<ID3D10Blob> vertex_shader,
			com_ptr<ID3D10Blob> pixel_shader,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE topolgy_type,
			com_ptr<ID3D12PipelineState>& m_pso);
		void create_billboard_pso(com_ptr<ID3DBlob> vertex_shader, com_ptr<ID3DBlob> pixel_shader, com_ptr<ID3DBlob> geometry_shader, com_ptr<ID3D12PipelineState>& m_pso);
		void reflect_shader(com_ptr<implementation::shader> target_shader);
		D3D12_CPU_DESCRIPTOR_HANDLE current_backbuffer_view() const;
		void create_crate_texture(std::vector<unsigned char> bytes, uint32_t file_size, hstring texture_name, uint32_t width, uint32_t height, uint32_t pixel_size);
		Windows::Foundation::IAsyncAction main_loop();

		// synchronization
		com_ptr<ID3D12Fence> m_gpu_fence;
		UINT64 m_cpu_fence = 0;
		void flush_cmd_queue();
		void update_samplers();

		void render_1();
		void render_2();

		std::unordered_map<hstring, com_ptr<ID3DBlob>> m_shaders;
		std::unordered_map<hstring, graphics::texture> m_textures;
	};
}

namespace winrt::graphics::factory_implementation
{
	struct renderer : rendererT<renderer, implementation::renderer>
	{
	};
}
