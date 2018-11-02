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

		m_ui_thread = winrt::apartment_context();

		swapChainPanel().PointerMoved([this](IInspectable sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs args) {

			auto current_point = args.GetCurrentPoint(sender.try_as<winrt::Windows::UI::Xaml::Controls::SwapChainPanel>());

			if (current_point.Properties().IsLeftButtonPressed())
			{
				auto point = current_point.Position();
				m_graphics_resources.last_mouse_position = m_graphics_resources.current_mouse_position;
				m_graphics_resources.current_mouse_position = point;
			}
			else
			{
				m_graphics_resources.last_mouse_position = { 0.0f, 0.0f };
				m_graphics_resources.current_mouse_position = { 0.0f, 0.0f };
			}
		});

		swapChainPanel().PointerReleased([this](IInspectable sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs args) {
			m_graphics_resources.last_mouse_position = { 0.0f, 0.0f };
			m_graphics_resources.current_mouse_position = { 0.0f, 0.0f };
		});

		swapChainPanel().PointerExited([this](IInspectable sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs args) {
			m_graphics_resources.last_mouse_position = { 0.0f, 0.0f };
			m_graphics_resources.current_mouse_position = { 0.0f, 0.0f };
		});

		m_graphics_resources.enable_debug_layer();
		m_graphics_resources.create_factory();
		m_graphics_resources.create_device();
		m_graphics_resources.create_fence();
		m_graphics_resources.create_cmd_objects();
		m_graphics_resources.create_descriptor_heaps();
		m_graphics_resources.create_depthstencil_buffer();
		m_graphics_resources.create_swapchain_xamlpanel(swapChainPanel());
		m_graphics_resources.create_render_targets();
		m_graphics_resources.create_constant_buffers();
		m_graphics_resources.create_rootsignature();
		m_graphics_resources.init_dynamic_buffer();

		m_render_loop_work_item = WorkItemHandler([this](Windows::Foundation::IAsyncAction action)
		{
			m_timer.Reset();

			while (m_running)
			{
				m_timer.Tick();

				if (m_window.Visible())
				{
					m_graphics_resources.update();
					m_graphics_resources.render();
				}
			}
		});
		m_window = Window::Current().CoreWindow().GetForCurrentThread();
	}

	wzrd_editor::GeometryViewModel MainPage::GeometryViewModel()
	{
		return m_geometryViewModel;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_create_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		auto pos_x = m_geometryViewModel.Geometry().Position().x();
		auto pos_y = m_geometryViewModel.Geometry().Position().y();
		auto pos_z = m_geometryViewModel.Geometry().Position().z();

		m_geometryViewModel.Geometry().Positions().Append(m_geometryViewModel.Geometry().Position());

		m_vertex_generator.push_vertex(pos_x, pos_y, pos_z, 0.0f, 0.0f);

		set_vertices_list_visibility();

		m_graphics_resources.update_vbv_content(m_vertex_generator.vertices());
		co_return;
	}

	void MainPage::set_vertices_list_visibility()
	{
		if (m_vertex_generator.vertices().size() != 0)
		{
			vertices_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else
		{
			vertices_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}

	void MainPage::set_shaders_list_visibility()
	{
		if (m_graphics_resources.m_shaders.size() != 0)
		{
			shaders_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else
		{
			shaders_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_clear_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_vertex_generator.vertices().clear();
		m_geometryViewModel.Geometry().Positions().Clear();
		m_graphics_resources.update_vbv_content(m_vertex_generator.vertices());
		set_vertices_list_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::menuflyout_clear_shaders_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_shaders.clear();
		set_shaders_list_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_texture_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		auto file_buffer = co_await Utilities::pick_file_buffer(hstring(L".dds"), pick_modes::single_file_async);
		if (file_buffer == nullptr)
		{
			return;
		}
		auto file_bytes = co_await Utilities::read_file_bytes(file_buffer);

		auto woodcrate_texture = m_graphics_resources.create_texture(file_bytes, file_buffer.Length(), "woodCrateTexture");
		m_textures["woodCrateTexture"] = std::move(woodcrate_texture);
		m_graphics_resources.create_shader_resources(m_textures["woodCrateTexture"]->Resource.get());
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_pixelshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		auto shader_file_bytes = co_await Utilities::pick_shader_file();

		co_await winrt::resume_background();
		m_graphics_resources.m_shaders["woodCratePS"] = Utilities::compile_shader("ps_5_0", shader_file_bytes, "PS");

		co_await m_ui_thread;
		wzrd_editor::Shader new_shader = winrt::make<wzrd_editor::implementation::Shader>(hstring(L"woodCratePS"));
		m_geometryViewModel.Shaders().Append(new_shader);
		set_shaders_list_visibility();
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_vertexshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		auto shader_file_bytes = co_await Utilities::pick_shader_file();

		co_await winrt::resume_background();
		m_graphics_resources.m_shaders["woodCrateVS"] = Utilities::compile_shader("vs_5_0", shader_file_bytes, "VS");

		co_await m_ui_thread;
		wzrd_editor::Shader new_shader = winrt::make<wzrd_editor::implementation::Shader>(hstring(L"woodCrateVS"));
		m_geometryViewModel.Shaders().Append(new_shader);
		set_shaders_list_visibility();
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_pointlist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::points;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_trianglelist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::triangles;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_lineslist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::lines;
		co_return;
	}


	Windows::Foundation::IAsyncAction MainPage::onclick_build_linestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::linestrips;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_trianglestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::trianglestrips;
		co_return;
	}

	void MainPage::start_render_loop()
	{
		if (!m_running)
		{
			m_graphics_resources.init_psos();
			m_running = true;
			ThreadPool::RunAsync(m_render_loop_work_item);
		}
	}
}
