#pragma once 
#include "renderer.g.h"
#include "upload_buffer.h"
#include "buffer.h"
#include "utilities.h"
#include "texture.h"
#include "shader.h"
#include "../DirectXTK12/Inc/DDSTextureLoader.h"
#include <DirectXTex/DirectXTex/DirectXTex.h>
#include "../os_utilities/os_utilities.h"

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
                (static_cast<uint32_t>(static_cast<uint8_t>(ch0)) \
                | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) \
                | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) \
                | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24))

namespace winrt::graphics::implementation
{
	struct DDS_HEADER_DXT10
	{
		DXGI_FORMAT              dxgiFormat;
		D3D12_RESOURCE_DIMENSION resourceDimension;
		UINT                     miscFlag;
		UINT                     arraySize;
		UINT                     miscFlags2;
	};

	struct DDS_PIXELFORMAT
	{
		uint32_t    size;
		uint32_t    flags;
		uint32_t    fourCC;
		uint32_t    RGBBitCount;
		uint32_t    RBitMask;
		uint32_t    GBitMask;
		uint32_t    BBitMask;
		uint32_t    ABitMask;
	};

	struct DDS_HEADER
	{
		uint32_t        size;
		uint32_t        flags;
		uint32_t        height;
		uint32_t        width;
		uint32_t        pitchOrLinearSize;
		uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
		uint32_t        mipMapCount;
		uint32_t        reserved1[11];
		DDS_PIXELFORMAT ddspf;
		uint32_t        caps;
		uint32_t        caps2;
		uint32_t        caps3;
		uint32_t        caps4;
		uint32_t        reserved2;
	};

	struct object_constants
	{
		DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 texture_transform = MathHelper::Identity4x4();
	};

	struct shader_readback_data
	{
		float level_of_detail;
		uint32_t width;
		uint32_t height;
	};

	struct renderer : rendererT<renderer>
	{
		renderer();

		void enable_debug_layer();
		void initialize_buffers_showcase(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain);
		void initialize_textures_showcase(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain);
		void start_render_loop();
		void stop_render_loop();
		void clear_shaders();
		void clear_textures();
		void remove_texture(hstring name);
		void remove_shader(hstring name);
		bool is_rendering();
		graphics::buffer current_buffer();
		void current_buffer(graphics::buffer const& value);
		Windows::Foundation::IAsyncOperation<graphics::operation_result> pick_and_compile_shader(graphics::shader new_shader);

		Windows::Foundation::IAsyncOperationWithProgress<graphics::operation_result, hstring> pick_texture(graphics::texture& new_texture, hstring name);
		Windows::Foundation::IAsyncOperationWithProgress<graphics::operation_result, hstring> create_dds_textures(
			hstring name,
			uint64_t width,
			uint64_t height,
			graphics::alpha_mode const& alpha_mode,
			bool is_generating_mipmaps,
			bool is_saving_to_file,
			Windows::Foundation::Collections::IObservableVector<graphics::texture>& new_textures);
		graphics::primitive_types current_topology();
		void current_topology(graphics::primitive_types const& value);

		int32_t viewport_width();
		void viewport_width(int32_t value);

		int32_t viewport_height();
		void viewport_height(int32_t value);

		graphics::texture current_texture();
		void current_texture(graphics::texture const& value);

		int32_t current_texture_index();
		void current_texture_index(int32_t value);

		static ID3D12Device* g_device;
		static ID3D12GraphicsCommandList* g_cmd_list;

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		winrt::apartment_context m_ui_thread;

		bool m_is_rendering = false;
		com_ptr<buffer> m_current_buffer = nullptr;
		constexpr static int m_swapchain_buffer_count = 2;
		int m_current_backbuffer = 0;
		UINT m_rtv_descriptor_size = 0;
		UINT m_cbv_srv_uav_heap_descriptor_handle_size = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE m_texture_descriptor_handle = {};

		int32_t m_output_width = 512;
		int32_t m_output_height = 512;
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
		com_ptr<ID3D12Resource> m_cb_position_upload_buffer = nullptr;
		com_ptr<ID3D12Resource> m_uav_lod_readback_buffer = nullptr;
		com_ptr<ID3D12Resource> m_uav_lod_default_buffer = nullptr;

		unsigned char* m_mapped_texcoord_data = nullptr;
		unsigned char* m_mapped_position_data = nullptr;

		com_ptr<ID3D12Resource> intermediate_upload_resource = nullptr;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_basic_input_layout;

		Windows::Foundation::IAsyncAction update_ui(float new_val);
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
		void create_checkerboard_tex_srv();
		void create_cb_texcoord();
		void create_cb_billboard_pos();
		void create_uav();
		std::vector<UINT8> generate_texture_data(UINT texture_width, UINT texture_height, UINT texture_pixel_size);
		Windows::Foundation::IAsyncAction create_subresources_for_ui(std::vector<D3D12_SUBRESOURCE_DATA> & original_mipmaps, IObservableVector<IInspectable> & ui_mipmaps, DXGI_FORMAT format, uint32_t width);
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
		Windows::Foundation::IAsyncAction upload_to_gpu(graphics::texture& texture, std::vector<D3D12_SUBRESOURCE_DATA> bytes, DXGI_FORMAT texture_format);
		hstring get_dimension(D3D12_RESOURCE_DIMENSION dimension);
		Windows::Foundation::IAsyncAction main_loop();

		// synchronization
		com_ptr<ID3D12Fence> m_gpu_fence;
		UINT64 m_cpu_fence = 0;
		void flush_cmd_queue();
		void update_samplers();
		D3D12_FILTER select_filter();

		void render_1();
		void render_2();

		std::unordered_map<hstring, com_ptr<ID3DBlob>> m_shaders;
		std::unordered_map<hstring, graphics::texture> m_textures;
		graphics::texture m_current_texture = nullptr;
		int32_t m_current_texture_index = 0;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;

		void raise_property_changed(hstring const& property_name)
		{
			m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(property_name));
		}

		template <class T>
		void update_value(hstring const& property_name, T & var, T value)
		{
			if (var != value)
			{
				var = value;
				raise_property_changed(property_name);
			}
		}
	};
}

namespace winrt::graphics::factory_implementation
{
	struct renderer : rendererT<renderer, implementation::renderer>
	{
	};
}
