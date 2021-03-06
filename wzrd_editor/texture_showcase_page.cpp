﻿#include "pch.h"
#include "texture_showcase_page.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using Windows::Foundation::IInspectable;

namespace winrt::wzrd_editor::implementation
{
	texture_showcase_page::texture_showcase_page()
	{
		InitializeComponent();

		m_texture_showcase_vm.current_renderer(m_renderer);

		m_renderer.enable_debug_layer();

		VisualStateManager().GoToState(*this, L"valid_shader_not_selected", false);

		auto address_modes = m_texture_showcase_vm.address_modes();
		Utilities::generate_address_modes_attributes(address_modes);

		auto minification_filter = m_texture_showcase_vm.minification_filters();
		Utilities::generate_filtering_methods_attributes(minification_filter);

		auto magnification_filter = m_texture_showcase_vm.magnification_filters();
		Utilities::generate_filtering_methods_attributes(magnification_filter);

		auto miplevel_sampling_filter = m_texture_showcase_vm.miplevel_sampling_filters();
		Utilities::generate_filtering_methods_attributes(miplevel_sampling_filter);

		auto filter_reduction_types = m_texture_showcase_vm.filter_reduction_types();
		Utilities::generate_filter_reduction_types_attributes(filter_reduction_types);

		auto filter_comparisonfuncs = m_texture_showcase_vm.filter_comparisonfuncs();
		Utilities::generate_filter_comparisonfuncs_attributes(filter_comparisonfuncs);

		auto sampling_functions = m_texture_showcase_vm.sampling_functions();
		Utilities::generate_sampling_functions_attributes(sampling_functions);

		auto alpha_modes_cpy = m_texture_showcase_vm.dds_creation_vm().alpha_modes();
		Utilities::generate_alpha_modes_attributes(alpha_modes_cpy);

		m_renderer.initialize_textures_showcase(swapchain_panel());
	}

	texture_showcase_page::~texture_showcase_page()
	{
	}

	wzrd_editor::texture_showcase_vm texture_showcase_page::texture_showcase_vm()
	{
		return m_texture_showcase_vm;
	}

	void texture_showcase_page::texture_showcase_vm(wzrd_editor::texture_showcase_vm const& value)
	{
		m_texture_showcase_vm = value;
	}

	IAsyncAction texture_showcase_page::onclick_vertexshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto new_shader = graphics::shader(hstring(L"default_vs"), graphics::shader_type::vertex);
		texture_showcase_vm().shaders().Append(new_shader);

		new_shader.is_loading(true);
		auto result = co_await m_renderer.pick_and_compile_shader(new_shader);
		new_shader.is_loading(false);

		switch (result.status)
		{
		case graphics::operation_status::error:
			new_shader.is_error(true);
			co_await os_utilities::show_error_dialog(result.error_message, hstring{ L"Vertex shader compilation error" });
			break;

		case graphics::operation_status::cancelled:
			texture_showcase_vm().shaders().RemoveAtEnd();

		default:
			break;
		}

		co_return;
	}

	IAsyncAction texture_showcase_page::onclick_pixelshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto new_shader = graphics::shader(hstring(L"default_ps"), graphics::shader_type::pixel);
		texture_showcase_vm().shaders().Append(new_shader);

		new_shader.is_loading(true);
		auto result = co_await m_renderer.pick_and_compile_shader(new_shader);
		new_shader.is_loading(false);

		switch (result.status)
		{
		case graphics::operation_status::error:
			new_shader.is_error(true);
			co_await os_utilities::show_error_dialog(result.error_message, hstring{ L"Pixel shader compilation error" });
			break;

		case graphics::operation_status::cancelled:
			texture_showcase_vm().shaders().RemoveAtEnd();

		default:
			break;
		}

		co_return;
	}

	IAsyncAction texture_showcase_page::onclick_geometryshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto new_shader = graphics::shader(hstring(L"default_gs"), graphics::shader_type::geometry);
		texture_showcase_vm().shaders().Append(new_shader);

		new_shader.is_loading(true);
		auto result = co_await m_renderer.pick_and_compile_shader(new_shader);
		new_shader.is_loading(false);

		switch (result.status)
		{
		case graphics::operation_status::error:
			new_shader.is_error(true);
			co_await os_utilities::show_error_dialog(result.error_message, hstring{ L"Geometry shader compilation error" });
			break;

		case graphics::operation_status::cancelled:
			texture_showcase_vm().shaders().RemoveAtEnd();
			break;

		default:
			break;
		}

		co_return;
	}

	IAsyncAction texture_showcase_page::delete_selected_shader(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		//auto _shaders_list = shaders_list();

		//auto items_source = _shaders_list.ItemsSource();
		//auto items_list = winrt::unbox_value<IObservableVector<IInspectable>>(items_source);
		//auto selected_index = _shaders_list.SelectedIndex();
		//auto current_shader_selection = winrt::unbox_value<graphics::shader>(items_list.GetAt(selected_index));

		//m_renderer.remove_shader(current_shader_selection.shader_name());
		//items_list.RemoveAt(selected_index);

		co_return;
	}

	IAsyncAction texture_showcase_page::delete_selected_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_renderer.stop_render_loop();
		//auto _textures_list = textures_list();

		//auto items_source = _textures_list.ItemsSource();
		//auto items_list = winrt::unbox_value<IObservableVector<IInspectable>>(items_source);

		//auto selected_index = _textures_list.SelectedIndex();
		//auto current_texture_selection = winrt::unbox_value<graphics::texture>(items_list.GetAt(selected_index));

		//m_renderer.remove_texture(current_texture_selection.texture_name());
		//items_list.RemoveAt(selected_index);

		co_return;
	}

	IAsyncAction texture_showcase_page::onclick_pick_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		wzrd_editor::texture_vm new_texture_vm = make<wzrd_editor::implementation::texture_vm>();
		new_texture_vm.is_loading(true);

		texture_showcase_vm().current_texture_vm(new_texture_vm);
		texture_showcase_vm().textures().Append(new_texture_vm);

		graphics::texture new_texture = nullptr;
		auto pick_texture_async = m_renderer.pick_texture(new_texture, L"default_texture");

		pick_texture_async.Progress([](auto const& /* sender */, hstring progress)
		{
			hstring msg = L"\n" + progress + L"\n";
			OutputDebugStringW(msg.c_str());
		});

		graphics::operation_result result = co_await pick_texture_async;

		new_texture_vm.current_texture(new_texture);
		new_texture_vm.is_loading(false);

		this->Bindings->Update();

		hstring error_msg;
		IObservableVector<graphics::subresource> new_mipmaps;

		switch (result.status)
		{
		case graphics::operation_status::success:
			new_texture_vm.is_loading(false);
			break;
		case graphics::operation_status::error:
			os_utilities::show_error_dialog(result.error_message, L"Failed to parse the DDS file");
			texture_showcase_vm().textures().RemoveAtEnd();
			break;

		case graphics::operation_status::cancelled:
			error_msg = result.error_message;
			texture_showcase_vm().textures().RemoveAtEnd();
			break;

		default:
			break;
		}
	}

	IAsyncAction texture_showcase_page::onclick_create_texture(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		VisualStateManager().GoToState(*this, L"creating_dds_texture", true);
		co_return;
	}

	IAsyncAction texture_showcase_page::shader_selection_changed(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto selected_items_count = unbox_value<Windows::UI::Xaml::Controls::ListView>(sender).SelectedItems().Size();

		if (selected_items_count > 0)
		{
			VisualStateManager().GoToState(*this, L"valid_shader_selected", false);
		}
		else
		{
			VisualStateManager().GoToState(*this, L"valid_shader_not_selected", false);
		}
		co_return;
	}

	IAsyncAction texture_showcase_page::texture_selection_changed(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto listview = unbox_value<Windows::UI::Xaml::Controls::ListView>(sender);
		auto selected_items_count = listview.SelectedItems().Size();

		if (selected_items_count > 0)
		{
			VisualStateManager().GoToState(*this, L"valid_texture_selected", false);

			auto current_texture_vm = unbox_value<wzrd_editor::texture_vm>(listview.SelectedItem());
			m_texture_showcase_vm.current_texture_vm(current_texture_vm);
			m_renderer.current_texture(current_texture_vm.current_texture());
			this->Bindings->Update();
		}
		else
		{
			VisualStateManager().GoToState(*this, L"valid_texture_not_selected", false);
		}
		co_return;
	}

	IAsyncAction texture_showcase_page::onclick_create_dds(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		using namespace std::chrono;

		Windows::Storage::Pickers::FileOpenPicker picker;
		picker.FileTypeFilter().Append(L".bmp");
		picker.FileTypeFilter().Append(L".jpg");
		picker.FileTypeFilter().Append(L".png");

		Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile> files = co_await picker.PickMultipleFilesAsync();

		if (files.Size() == 0)
		{
			co_return;
		}

		if (files.Size() + m_texture_showcase_vm.textures().Size() >= 10)
		{
			co_await os_utilities::show_error_dialog(L"There is a limit of 10 textures.", L"DDS texture creation error.");
			co_return;
		}

		for (auto file : files)
		{
			wzrd_editor::texture_vm new_texture_vm = make<wzrd_editor::implementation::texture_vm>();
			new_texture_vm.is_loading(true);
			m_texture_showcase_vm.textures().Append(new_texture_vm);

			graphics::texture new_dds_texture = nullptr;

			auto result = co_await m_renderer.create_dds_texture(
				file,
				m_texture_showcase_vm.dds_creation_vm().texture_name(),
				m_texture_showcase_vm.dds_creation_vm().width(),
				m_texture_showcase_vm.dds_creation_vm().height(),
				m_texture_showcase_vm.dds_creation_vm().alpha_mode(),
				m_texture_showcase_vm.dds_creation_vm().is_generating_mipmaps(),
				m_texture_showcase_vm.dds_creation_vm().is_saving_to_file(),
				new_dds_texture
			);

			switch (result.status)
			{
			case graphics::operation_status::success:
				new_texture_vm.current_texture(new_dds_texture);
				new_texture_vm.is_loading(false);
				break;
			case graphics::operation_status::cancelled:
				texture_showcase_vm().textures().RemoveAtEnd();
				co_return;
				break;
			case graphics::operation_status::error:
				co_await os_utilities::show_error_dialog(result.error_message, L"DDS texture creation error.");
				break;
			default:
				break;
			}

		}
		this->Bindings->Update();
		co_return;
	}

	IAsyncAction texture_showcase_page::render_onclick(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_renderer.start_render_loop();
		split_pane().IsPaneOpen(true);
		co_return;
	}
}
