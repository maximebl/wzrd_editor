//
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "utilities.h"
#include "DDSTextureLoader.h"
#include "FrameResource.h"
#include "GraphicsResources.h"
#include "GeometryViewModel.h"
#include "Geometry.h"
#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"
#include "wzrd_vec3.h"
#include "VertexBufferGenerator.h"
#include "int_to_visibility.h"
#include "vertex_buffer.h"
#include "vertex2.h"
#include "winrt\graphics.h"

namespace winrt::wzrd_editor::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();

	private:
		graphics::renderer m_renderer;

		com_ptr<wzrd_editor::implementation::vertex_buffer> m_vertex_buffer;
		GraphicsResources m_graphics_resources;
		VertexBufferGenerator m_vertex_generator;

		wzrd_editor::GeometryViewModel m_geometryViewModel = winrt::make<wzrd_editor::implementation::GeometryViewModel>();

		IAsyncAction start_render_loop();

	public:
		wzrd_editor::GeometryViewModel GeometryViewModel();

		IAsyncAction ui_thread_work();

		IAsyncAction onclick_texture_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_pixelshader_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_vertexshader_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_create_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_clear_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction menuflyout_clear_shaders_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction menuflyout_clear_textures_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		IAsyncAction onclick_build_pointlist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_build_trianglelist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_build_lineslist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_build_linestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_build_trianglestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		IAsyncAction onclick_render_as_static(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_render_as_dynamic(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		//IAsyncAction pick_and_compile_shader(const std::string shader_name, wzrd_editor::ShaderType shader_type, const std::string entry_point, const std::string version);
		IAsyncAction show_error_dialog(hstring error_message);

		GameTimer m_timer;
		Windows::UI::Core::CoreWindow m_window = nullptr;

		bool m_windowVisible = false;
		double m_current_slider_x = 0;
		double m_current_slider_y = 0;
		double m_current_slider_z = 0;
		winrt::apartment_context m_ui_thread;

		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;

		uint32_t current_buffer_capacity = 0;
		int32_t m_buffer_resize_increment = 0;
		bool m_running = false;
		int32_t m_size_limit = 0;
		int output_width = 700;
		int output_height = 700;
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}
