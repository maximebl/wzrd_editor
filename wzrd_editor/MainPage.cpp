﻿#include "pch.h"
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

		m_window = Window::Current().CoreWindow().GetForCurrentThread();

		m_render_loop_work_item = WorkItemHandler([this](Windows::Foundation::IAsyncAction action)
		{
			m_timer.Reset();
			ui_thread_work();

			while (m_windowVisible)
			{
				m_timer.Tick();
				if (m_windowVisible)
				{
					m_graphics_resources.update();
					m_graphics_resources.render();
				}
			}
		});
	}

	Windows::Foundation::IAsyncAction MainPage::ui_thread_work()
	{
		co_await m_ui_thread;
		m_windowVisible = m_window.Visible();
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

		auto selected_color = color_picker().Color();

		auto color_r = static_cast<float>(selected_color.R) / 255;
		auto color_g = static_cast<float>(selected_color.G) / 255;
		auto color_b = static_cast<float>(selected_color.B) / 255;
		auto color_a = static_cast<float>(selected_color.A) / 255;

		auto tex_u = 0.0f;
		auto tex_v = 0.0f;
		// just add the new_vertex to m_vertices and pass m_vertices to Vertices()
		auto new_vertex = winrt::make<winrt::wzrd_editor::implementation::Vertex>(
			pos_x, pos_y, pos_z,
			color_r, color_g, color_b, color_a,
			tex_u, tex_v
			);
		m_geometryViewModel.Geometry().Vertices().Append(new_vertex);
		m_vertex_generator.push_vertex(pos_x, pos_y, pos_z, color_r, color_g, color_b, color_a, tex_u, tex_v);

		//auto stv = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>(std::move(m_vertex_generator.vertices()));
		//auto lUlZ = m_vertex_generator.vertices());

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
		if (m_geometryViewModel.Shaders().Size() > 0)
		{
			shaders_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else
		{
			shaders_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}

	void MainPage::set_textures_visibility()
	{
		if (m_geometryViewModel.Textures().Size() > 0)
		{
			textures_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else
		{
			textures_list().Visibility(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_clear_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_vertex_generator.vertices().clear();
		m_geometryViewModel.Geometry().Vertices().Clear();
		m_graphics_resources.update_vbv_content(m_vertex_generator.vertices());
		set_vertices_list_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::menuflyout_clear_shaders_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_shaders.clear();
		m_geometryViewModel.Shaders().Clear();
		set_shaders_list_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::menuflyout_clear_textures_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_textures.clear();
		m_geometryViewModel.Textures().Clear();
		set_textures_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_texture_picker(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		auto texture_file_bytes = co_await Utilities::pick_file(winrt::hstring(L".dds"));

		wzrd_editor::Texture new_texture = winrt::make<wzrd_editor::implementation::Texture>(winrt::hstring(L"woodCrateTexture"));
		m_geometryViewModel.Textures().Append(new_texture);
		new_texture.Loading(true);

		set_textures_visibility();

		co_await winrt::resume_background();
		auto woodcrate_texture = m_graphics_resources.create_texture(texture_file_bytes, texture_file_bytes.size(), "woodCrateTexture");
		m_graphics_resources.m_textures["woodCrateTexture"] = std::move(woodcrate_texture);
		m_graphics_resources.create_shader_resources(m_graphics_resources.m_textures["woodCrateTexture"]->Resource.get());

		co_await m_ui_thread;
		new_texture.Loading(false);
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_pixelshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		auto shader_file_bytes = co_await Utilities::pick_file(winrt::hstring(L".hlsl"));

		wzrd_editor::Shader new_shader = winrt::make<wzrd_editor::implementation::Shader>(hstring(L"woodCratePS"), wzrd_editor::ShaderType::pixel);
		m_geometryViewModel.Shaders().Append(new_shader);
		new_shader.Loading(true);

		set_shaders_list_visibility();

		co_await winrt::resume_background();
		m_graphics_resources.m_shaders["woodCratePS"] = Utilities::compile_shader("ps_5_0", shader_file_bytes, "PS");

		co_await m_ui_thread;
		new_shader.Loading(false);
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_vertexshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		auto shader_file_bytes = co_await Utilities::pick_file(winrt::hstring(L".hlsl"));

		wzrd_editor::Shader new_shader = winrt::make<wzrd_editor::implementation::Shader>(hstring(L"woodCrateVS"), wzrd_editor::ShaderType::vertex);
		m_geometryViewModel.Shaders().Append(new_shader);
		new_shader.Loading(true);

		set_shaders_list_visibility();

		co_await winrt::resume_background();
		m_graphics_resources.m_shaders["woodCrateVS"] = Utilities::compile_shader("vs_5_0", shader_file_bytes, "VS");

		co_await m_ui_thread;
		new_shader.Loading(false);
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
