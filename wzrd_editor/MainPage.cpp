#include "pch.h"
#include "MainPage.h"
#include "buffer_size_select_dialog.h"

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

		m_window = Window::Current().CoreWindow().GetForCurrentThread();
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

	Windows::Foundation::IAsyncAction MainPage::onclick_new_index(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto new_index = m_geometryViewModel.Geometry().Index();
		m_geometryViewModel.Geometry().Indices().Append(winrt::box_value(new_index));
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_clear_indices(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_geometryViewModel.Geometry().Indices().Clear();
		co_return;
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

		auto new_vertex = winrt::make<winrt::wzrd_editor::implementation::vertex2>(
			pos_x, pos_y, pos_z,
			color_r, color_g, color_b, color_a,
			tex_u, tex_v);

		m_geometryViewModel.Geometry().Vertices().Append(new_vertex);

		//if (m_running)
		//{
			m_vertex_buffer->add_to_view(new_vertex);
		//}

		//auto new_vertex = winrt::make<winrt::wzrd_editor::implementation::Vertex>(
		//	pos_x, pos_y, pos_z,
		//	color_r, color_g, color_b, color_a,
		//	tex_u, tex_v);
		//m_geometryViewModel.Geometry().Vertices().Append(new_vertex);

		//if (m_running)
		//{
		//	if (m_geometryViewModel.Geometry().Vertices().Size() <= current_buffer_capacity)
		//	{
		//		m_vertex_generator.push_vertex(pos_x, pos_y, pos_z, color_r, color_g, color_b, color_a, tex_u, tex_v);
		//		m_graphics_resources.update_current_buffer(m_vertex_generator.vertices());

		//		if (!m_graphics_resources.m_dynamic_vertex_buffer->m_is_auto_resize && m_geometryViewModel.Geometry().Vertices().Size() == current_buffer_capacity)
		//		{
		//			VisualStateManager().GoToState(*this, L"buffer_full", false);
		//		}
		//	}
		//	else {
		//		if (m_graphics_resources.m_dynamic_vertex_buffer->m_is_auto_resize)
		//		{
		//			// recreate the upload_heap committed resource
		//			current_buffer_capacity += m_buffer_resize_increment;
		//			m_graphics_resources.swap_upload_buffer(current_buffer_capacity, m_vertex_generator.vertices());
		//			// update the swapped buffer with the newly added vertex
		//			m_vertex_generator.push_vertex(pos_x, pos_y, pos_z, color_r, color_g, color_b, color_a, tex_u, tex_v);
		//			m_graphics_resources.update_current_buffer(m_vertex_generator.vertices());
		//		}
		//		else {
		//			VisualStateManager().GoToState(*this, L"buffer_full", false);
		//		}
		//	}
			auto current_capacity_percentage = (m_vertex_buffer->current_size() * 100) / m_vertex_buffer->max_size();
			m_geometryViewModel.Geometry().BufferCapacity(current_capacity_percentage);
		//}
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_clear_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_vertex_generator.vertices().clear();
		m_geometryViewModel.Geometry().Vertices().Clear();
		m_graphics_resources.update_current_buffer(m_vertex_generator.vertices());

		if (m_is_buffer_dynamic)
		{
			auto current_capacity_percentage = (m_geometryViewModel.Geometry().Vertices().Size() * 100) / current_buffer_capacity;
			m_geometryViewModel.Geometry().BufferCapacity(current_capacity_percentage);
		}
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::menuflyout_clear_shaders_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_shaders.clear();
		m_geometryViewModel.Shaders().Clear();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::menuflyout_clear_textures_click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_textures.clear();
		m_geometryViewModel.Textures().Clear();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_texture_picker(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&)
	{
		auto texture_file_bytes = co_await Utilities::pick_file(winrt::hstring(L".dds"));

		wzrd_editor::Texture new_texture = winrt::make<wzrd_editor::implementation::Texture>(winrt::hstring(L"woodCrateTexture"));
		m_geometryViewModel.Textures().Append(new_texture);
		new_texture.Loading(true);

		co_await winrt::resume_background();
		auto woodcrate_texture = m_graphics_resources.create_texture(texture_file_bytes, texture_file_bytes.size(), "woodCrateTexture");
		m_graphics_resources.m_textures["woodCrateTexture"] = std::move(woodcrate_texture);
		m_graphics_resources.create_shader_resources(m_graphics_resources.m_textures["woodCrateTexture"]->Resource.get());

		co_await m_ui_thread;
		new_texture.Loading(false);
	}

	Windows::Foundation::IAsyncAction MainPage::show_error_dialog(LPVOID error_buffer_ptr)
	{
		LPCSTR errorMessagePtr = (const char*)error_buffer_ptr;
		std::string errorMessage(errorMessagePtr);
		std::wstring title = L"Shader compilation error.";
		std::wstring message(errorMessage.begin(), errorMessage.end());

		OutputDebugStringA(errorMessagePtr);

		auto dialog = winrt::Windows::UI::Xaml::Controls::ContentDialog();
		dialog.Title(winrt::box_value(title));
		dialog.Content(winrt::box_value(message));
		dialog.CloseButtonText(L"Ok");
		co_await dialog.ShowAsync();
	}

	Windows::Foundation::IAsyncAction MainPage::pick_and_compile_shader(const std::string shader_name, wzrd_editor::ShaderType shader_type, const std::string entry_point, const std::string version)
	{
		auto shader_file_bytes = co_await Utilities::pick_file(winrt::hstring(L".hlsl"));

		wzrd_editor::Shader new_shader = winrt::make<wzrd_editor::implementation::Shader>(winrt::to_hstring(shader_name), shader_type);
		new_shader.Loading(true);
		m_geometryViewModel.Shaders().Append(new_shader);

		co_await winrt::resume_background();
		auto compilation_result = Utilities::compile_shader(version, shader_file_bytes, entry_point);

		if (compilation_result.is_success)
		{
			m_graphics_resources.m_shaders[shader_name] = compilation_result.result_blob;
		}
		else {
			co_await m_ui_thread;
			new_shader.is_error(true);
			show_error_dialog(compilation_result.result_blob.get()->GetBufferPointer());
		}

		co_await m_ui_thread;
		new_shader.Loading(false);
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_pixelshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		co_await pick_and_compile_shader("woodCratePS", wzrd_editor::ShaderType::pixel, "PS", "ps_5_0");
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_vertexshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		co_await pick_and_compile_shader("woodCrateVS", wzrd_editor::ShaderType::vertex, "VS", "vs_5_0");
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_pointlist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::points;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_trianglelist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::triangles;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_lineslist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::lines;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_linestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::linestrips;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_trianglestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_mode::trianglestrips;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_render_as_static(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		VisualStateManager().GoToState(*this, L"static_buffer_selected", false);
		m_is_buffer_dynamic = false;
		m_vertex_generator.regenerate_vertices_from_model(m_geometryViewModel.Geometry().Vertices());
		m_graphics_resources.init_static_buffer(m_vertex_generator.vertices());
		start_render_loop();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_render_as_dynamic(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::RoutedEventArgs const &)
	{
		auto dialog = winrt::make<wzrd_editor::implementation::buffer_size_select_dialog>(m_geometryViewModel.Geometry().Vertices().Size());
		auto dialog_result = co_await dialog.ShowAsync();

		switch (dialog_result)
		{
		case winrt::Windows::UI::Xaml::Controls::ContentDialogResult::None:
			co_return;

		case winrt::Windows::UI::Xaml::Controls::ContentDialogResult::Primary:
			VisualStateManager().GoToState(*this, L"dynamic_buffer_selected", false);

			m_vertex_buffer = winrt::make_self<winrt::wzrd_editor::implementation::vertex_buffer>(
				wzrd_editor::buffer_type::dynamic_buffer,
				dialog.buffer_size(),
				dialog.buffer_increment_size(),
				dialog.is_auto_resizeable());
			//m_is_buffer_dynamic = true;
			//current_buffer_capacity = dialog.buffer_size();
			//m_buffer_resize_increment = dialog.buffer_increment_size();
			//m_graphics_resources.init_dynamic_buffer(dialog.buffer_size(), dialog.is_auto_resizeable());
			//m_vertex_generator.regenerate_vertices_from_model(m_geometryViewModel.Geometry().Vertices());
			//m_graphics_resources.update_current_buffer(m_vertex_generator.vertices());

			//start_render_loop();
			co_return;
		default:
			break;
		}
	}

	Windows::Foundation::IAsyncAction MainPage::onchanged_vertex_input(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::TextChangedEventArgs const& args)
	{
		auto current_textbox = sender.try_as<Windows::UI::Xaml::Controls::TextBox>();
		current_textbox.GetBindingExpression(current_textbox.TextProperty()).UpdateSource();

		for (uint32_t i = 0; i < m_geometryViewModel.Geometry().Vertices().Size(); i++)
		{
			m_vertex_generator.vertices()[i].Pos.x = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->x();
			m_vertex_generator.vertices()[i].Pos.y = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->y();
			m_vertex_generator.vertices()[i].Pos.z = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->z();

			m_vertex_generator.vertices()[i].Color.x = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->r();
			m_vertex_generator.vertices()[i].Color.y = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->g();
			m_vertex_generator.vertices()[i].Color.z = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->b();
			m_vertex_generator.vertices()[i].Color.w = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->a();

			m_vertex_generator.vertices()[i].TexC.x = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->u();
			m_vertex_generator.vertices()[i].TexC.y = m_geometryViewModel.Geometry().Vertices().GetAt(i).try_as<wzrd_editor::implementation::Vertex>()->v();
		}

		m_graphics_resources.update_current_buffer(m_vertex_generator.vertices());
		co_return;
	}

	winrt::Windows::Foundation::IAsyncAction MainPage::start_render_loop()
	{
		if (!m_running)
		{
			m_graphics_resources.init_psos();
			m_running = true;

			co_await winrt::resume_background();
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

		}
	}
}
