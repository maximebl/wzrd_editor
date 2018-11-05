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
#include "wzrd_vec3.h"
#include "VertexBufferGenerator.h"

namespace winrt::wzrd_editor::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();

	private:
		GraphicsResources m_graphics_resources;
		VertexBufferGenerator m_vertex_generator;

		wzrd_editor::GeometryViewModel m_geometryViewModel = winrt::make<wzrd_editor::implementation::GeometryViewModel>();

		void set_vertices_list_visibility();
		void set_shaders_list_visibility();
		void start_render_loop();

	public:

		wzrd_editor::GeometryViewModel GeometryViewModel();

		Windows::Foundation::IAsyncAction ui_thread_work();

		Windows::Foundation::IAsyncAction onclick_texture_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_pixelshader_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_vertexshader_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_create_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_clear_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction menuflyout_clear_shaders_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
 
		Windows::Foundation::IAsyncAction onclick_build_pointlist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_build_trianglelist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_build_lineslist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_build_linestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		Windows::Foundation::IAsyncAction onclick_build_trianglestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

		GameTimer m_timer;
		Windows::UI::Core::CoreWindow m_window = nullptr;

		Windows::System::Threading::WorkItemHandler m_render_loop_work_item = nullptr;
		bool m_windowVisible = false;
		double m_current_slider_x = 0;
		double m_current_slider_y = 0;
		double m_current_slider_z = 0;
		winrt::apartment_context m_ui_thread;

		std::vector<std::unique_ptr<render_item>> m_render_items;
		std::vector<render_item*> m_opaque_render_items;
		frame_resource* m_current_frame_resource = nullptr;

		std::unordered_map<std::string, std::unique_ptr<winrt::wzrd_editor::data::Texture>> m_textures;
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

		bool m_running = false;
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
