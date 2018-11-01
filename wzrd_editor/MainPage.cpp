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
				ui_thread_work();
				m_timer.Tick();

				if (m_windowVisible)
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

	wzrd_editor::Shader MainPage::Shader()
	{
		return m_shaderViewModel;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_create_vertex(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		auto pos_x = m_geometryViewModel.Geometry().Position().x();
		auto pos_y = m_geometryViewModel.Geometry().Position().y();
		auto pos_z = m_geometryViewModel.Geometry().Position().z();

		auto positions_collection = m_geometryViewModel.Geometry().Positions();
		positions_collection.Append(m_geometryViewModel.Geometry().Position());

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
		Utilities::pick_file_buffer(hstring(L"aaa"), pick_modes::single_file_async);
		co_return;
		// Pick and read the file data
		//auto file_buffer = co_await Utilities::pick_file_buffer(L".dds", pick_modes::single_file_async);

		//winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		//filePicker.FileTypeFilter().Append(L".dds");
		//auto file = co_await filePicker.PickSingleFileAsync();
		//if (file == nullptr)
		//{
		//	return;
		//}
		//auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);

		//// abstract into Utilities::read_texture_file(fileBuffer) same as read_shader_file? rename it?
		//auto dataReader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer);
		//std::vector<unsigned char> file_bytes;
		//int fileSize = fileBuffer.Length();
		//file_bytes.assign(fileSize, 0);
		//dataReader.ReadBytes(file_bytes);

		//auto woodcrate_texture = m_graphics_resources.create_texture(file_bytes, fileSize, "woodCrateTexture");
		//m_textures["woodCrateTexture"] = std::move(woodcrate_texture);
		//m_graphics_resources.create_shader_resources(m_textures["woodCrateTexture"]->Resource.get());
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_pixelshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		// Pick and read the shader data
	/*
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".hlsl");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}
		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto file_bytes = Utilities::read_shader_file(fileBuffer).get();
*/

// shader struct to contain the info, put it in shader.idl
/*auto current_shader_model = winrt::make<wzrd_editor::implementation::Shader>(shader_name, shader_type);

auto shader_infos = pick_shader_async();
m_graphics_resources.m_shaders[shader_name] = Utilities::compile_shader("ps_5_0", shader_bytes, "PS");

m_geometryViewModel.Shaders().Append(current_shader_model);
set_shaders_list_visibility();*/
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_vertexshader_picker(IInspectable const&, RoutedEventArgs const&)
	{
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::texturePicker_Click(IInspectable const&, RoutedEventArgs const&)
	{

		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".dds");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}
		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);

		auto dataReader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer);
		std::vector<unsigned char> file_bytes;
		int fileSize = fileBuffer.Length();
		file_bytes.assign(fileSize, 0);
		dataReader.ReadBytes(file_bytes);

		auto woodcrate_texture = m_graphics_resources.create_texture(file_bytes, fileSize, "woodCrateTexture");
		m_textures["woodCrateTexture"] = std::move(woodcrate_texture);
		m_graphics_resources.create_shader_resources(m_textures["woodCrateTexture"]->Resource.get());
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::pixelShaderPicker_Click(IInspectable const&, RoutedEventArgs const&)
	{
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".hlsl");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}
		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto file_bytes = Utilities::read_shader_file(fileBuffer).get();

		m_graphics_resources.m_shaders["woodCratePS"] = Utilities::compile_shader("ps_5_0", file_bytes, "PS");

		auto shaders_collection = m_geometryViewModel.Shaders();
		shaders_collection.Append(m_shaderViewModel);

		set_shaders_list_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::vertexShaderPicker_Click(IInspectable const&, RoutedEventArgs const&)
	{
		// Pick and read the shader data
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(L".hlsl");
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file == nullptr)
		{
			return;
		}
		auto fileBuffer = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		auto file_bytes = Utilities::read_shader_file(fileBuffer).get();

		m_graphics_resources.m_shaders["woodCrateVS"] = Utilities::compile_shader("vs_5_0", file_bytes, "VS");

		auto shaders_collection = m_geometryViewModel.Shaders();
		shaders_collection.Append(m_shaderViewModel);

		set_shaders_list_visibility();
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_pointlist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_modes::points;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_trianglelist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_modes::triangles;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_lineslist(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_modes::lines;
		co_return;
	}


	Windows::Foundation::IAsyncAction MainPage::onclick_build_linestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_modes::linestrips;
		co_return;
	}

	Windows::Foundation::IAsyncAction MainPage::onclick_build_trianglestrips(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{
		start_render_loop();
		m_graphics_resources.m_current_rendering_mode = GraphicsResources::rendering_modes::trianglestrips;
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

	Windows::Foundation::IAsyncAction MainPage::ui_thread_work()
	{
		// Switch to the UI thread
		co_await m_ui_thread;

		m_windowVisible = m_window.Visible();
		m_current_slider_x = slider_x().Value();
		m_current_slider_y = slider_y().Value();
		m_current_slider_z = slider_z().Value();
		// Leaving this function switches back to the rendering thread
	}
}
