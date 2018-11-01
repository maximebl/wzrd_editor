#pragma once
#include "utilities.h"
#include "DDSTextureLoader.h"
#include "FrameResource.h"

class GraphicsResources
{
private:

	DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthstencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	constexpr static int m_swapchain_buffer_count = 3;
	constexpr static int m_output_width = 700;
	constexpr static int m_output_height = 700;
	D3D12_VIEWPORT m_screen_viewport;
	D3D12_RECT m_scissor_rect;
	
	// projection
	DirectX::XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 m_world = MathHelper::Identity4x4();
	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = 0.4f * DirectX::XM_PIDIV4;
	float m_radius = 5.0f;

	// device resources
	UINT m_rtv_descriptor_size = 0;
	int m_current_backbuffer = 0;
	IUnknown* m_window = nullptr;
	winrt::com_ptr<IDXGIFactory4> m_factory = nullptr;
	winrt::com_ptr<ID3D12Device> m_device = nullptr;
	winrt::com_ptr<IDXGISwapChain1> m_swapchain = nullptr;
	winrt::com_ptr<ID3D12CommandQueue> m_cmd_queue = nullptr;
	winrt::com_ptr<ID3D12GraphicsCommandList> m_graphics_cmdlist = nullptr;
	winrt::com_ptr<ID3D12CommandAllocator> m_cmd_allocator = nullptr;
	winrt::com_ptr<ID3D12DescriptorHeap> m_rtv_heap = nullptr;
	winrt::com_ptr<ID3D12DescriptorHeap> m_dsv_heap = nullptr;
	winrt::com_ptr<ID3D12DescriptorHeap> m_srv_heap = nullptr;
	winrt::com_ptr<ID3D12Resource> m_swapchain_buffer[m_swapchain_buffer_count];
	winrt::com_ptr<ID3D12Resource> m_depthstencil_buffer;
	winrt::com_ptr<ID3D12RootSignature> m_rootsig = nullptr;

	// pipeline states
	winrt::com_ptr<ID3D12PipelineState> m_points_pso = nullptr;
	winrt::com_ptr<ID3D12PipelineState> m_triangles_pso = nullptr;
	winrt::com_ptr<ID3D12PipelineState> m_lines_pso = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_basic_input_layout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_texture_input_layout;
	std::unique_ptr<MeshGeometry> m_box_geo = nullptr;
	std::unique_ptr<upload_buffer<object_constants>> m_object_cb;
	std::unique_ptr<upload_buffer<Vertex_tex>> m_dynamic_vertex_buffer;
	std::unique_ptr<upload_buffer<std::uint16_t>> m_dynamic_index_buffer;

	// synchronization
	int m_vertex_count = 0;
	winrt::com_ptr<ID3D12Fence> m_gpu_fence;
	UINT64 m_cpu_fence = 0;

public:
	GraphicsResources();
	~GraphicsResources();
	
	winrt::Windows::Foundation::Point last_mouse_position = winrt::Windows::Foundation::Point(0.0f, 0.0f);
	winrt::Windows::Foundation::Point current_mouse_position = winrt::Windows::Foundation::Point(0.0f, 0.0f);

	void enable_debug_layer();
	void create_factory();
	void create_device();
	void create_fence();
	void create_cmd_objects();
	void create_descriptor_heaps();
	void create_swapchain_corewindow(winrt::Windows::UI::Core::CoreWindow window);
	void create_swapchain_xamlpanel(winrt::Windows::UI::Xaml::Controls::SwapChainPanel xaml_swapchain_panel);
	void create_render_targets();
	void create_depthstencil_buffer();
	void create_constant_buffers();
	void create_rootsignature();
	std::unique_ptr<Texture> create_texture(std::vector<unsigned char> bytes, int file_size, std::string texture_name);
	void create_shader_resources(ID3D12Resource* resource);
	void create_points_pso(winrt::com_ptr<ID3D10Blob> vertex_shader, winrt::com_ptr<ID3D10Blob> pixel_shader);
	void create_triangles_pso(winrt::com_ptr<ID3D10Blob> vertex_shader, winrt::com_ptr<ID3D10Blob> pixel_shader);
	void create_lines_pso(winrt::com_ptr<ID3D10Blob> vertex_shader, winrt::com_ptr<ID3D10Blob> pixel_shader);

	void create_basic_input_layout();
	void create_texture_input_layout();
	void init_dynamic_buffer();

	void flush_cmd_queue();
	void execute_cmd_list();

	D3D12_CPU_DESCRIPTOR_HANDLE current_backbuffer_view() const;
	std::unordered_map<std::string, winrt::com_ptr<ID3DBlob>> m_shaders;

	void create_texture_geometry(std::vector<Vertex_tex>& vertices);
	void create_vertex_colored_box_geometry();
	void update_vbv_content(std::vector<Vertex_tex>& vertices);
	void init_psos();

	void update();
	void render();

	enum class rendering_modes {
		points = 0,
		triangles = 1,
		lines = 2,
		trianglestrips = 3,
		linestrips = 4
	};

	rendering_modes m_current_rendering_mode{ rendering_modes::points };

	concurrency::task<Texture*> create_texture_from_file_async(std::string texture_name, winrt::Windows::Storage::StorageFile texture_file);
};

