#include "pch.h"
#include "texture_showcase_page.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using Windows::Foundation::IInspectable;

namespace winrt::wzrd_editor::implementation
{
	texture_showcase_page::texture_showcase_page()
	{
		InitializeComponent();
		m_spring_animation = Window::Current().Compositor().CreateSpringVector3Animation();
		m_spring_animation.DampingRatio(0.65f);
		m_spring_animation.Period(std::chrono::milliseconds{ 50 });
		m_spring_animation.Target(hstring{ L"Scale" });

		m_renderer.enable_debug_layer();

		m_ui_items[hstring{ L"swapchain_panel" }] = box_value(swapchain_panel());

		m_ui_control_values.Insert(hstring{ L"topleft_u" }, 0.0f);
		m_ui_control_values.Insert(hstring{ L"topleft_v" }, 0.0f);

		m_ui_control_values.Insert(hstring{ L"topright_u" }, 1.0f);
		m_ui_control_values.Insert(hstring{ L"topright_v" }, 0.0f);

		m_ui_control_values.Insert(hstring{ L"bottomleft_u" }, 0.0f);
		m_ui_control_values.Insert(hstring{ L"bottomleft_v" }, 1.0f);

		m_ui_control_values.Insert(hstring{ L"bottomright_u" }, 1.0f);
		m_ui_control_values.Insert(hstring{ L"bottomright_v" }, 1.0f);

		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_r" }, 0.0f);
		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_g" }, 0.0f);
		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_b" }, 0.0f);
		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_a" }, 0.0f);

		m_ui_control_values.Insert(hstring{ L"sampler_minLOD" }, 0.0f);
		m_ui_control_values.Insert(hstring{ L"sampler_maxLOD" }, D3D12_FLOAT32_MAX);

		auto ui_items = winrt::single_threaded_map<hstring, IInspectable>(std::move(m_ui_items));

		VisualStateManager().GoToState(*this, L"valid_shader_not_selected", false);

		auto address_modes = m_texture_showcase_vm.address_modes();
		Utilities::generate_address_modes_attributes(address_modes);

		m_renderer.initialize_textures_showcase(ui_items, m_ui_control_values);
	}

	wzrd_editor::texture_showcase_vm texture_showcase_page::texture_showcase_vm()
	{
		return m_texture_showcase_vm;
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
		case graphics::compilation_status::error:
			new_shader.is_error(true);
			co_await os_utilities::show_error_dialog(result.error_message, hstring{ L"Vertex shader compilation error" });
			break;

		case graphics::compilation_status::cancelled:
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
		case graphics::compilation_status::error:
			new_shader.is_error(true);
			co_await os_utilities::show_error_dialog(result.error_message, hstring{ L"Pixel shader compilation error" });
			break;

		case graphics::compilation_status::cancelled:
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
		case graphics::compilation_status::error:
			new_shader.is_error(true);
			co_await os_utilities::show_error_dialog(result.error_message, hstring{ L"Geometry shader compilation error" });
			break;

		case graphics::compilation_status::cancelled:
			texture_showcase_vm().shaders().RemoveAtEnd();
			break;

		default:
			break;
		}

		co_return;
	}

	IAsyncAction texture_showcase_page::delete_selected_shader(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto _shaders_list = shaders_list();

		auto items_source = _shaders_list.ItemsSource();
		auto items_list = winrt::unbox_value<IObservableVector<IInspectable>>(items_source);
		auto selected_index = _shaders_list.SelectedIndex();
		auto current_shader_selection = winrt::unbox_value<graphics::shader>(items_list.GetAt(selected_index));

		m_renderer.remove_shader(current_shader_selection.shader_name());
		items_list.RemoveAt(selected_index);

		co_return;
	}

	IAsyncAction texture_showcase_page::delete_selected_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto _textures_list = textures_list();

		auto items_source = _textures_list.ItemsSource();
		auto items_list = winrt::unbox_value<IObservableVector<IInspectable>>(items_source);

		auto selected_index = _textures_list.SelectedIndex();
		auto current_texture_selection = winrt::unbox_value<graphics::texture>(items_list.GetAt(selected_index));

		m_renderer.remove_texture(current_texture_selection.texture_name());
		items_list.RemoveAt(selected_index);

		co_return;
	}

	IAsyncAction texture_showcase_page::onclick_pick_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		auto new_texture = graphics::texture();
		texture_showcase_vm().textures().Append(new_texture);

		new_texture.is_loading(true);
		new_texture = co_await m_renderer.pick_texture(new_texture, hstring{ L"default_texture" });
		new_texture.is_loading(false);
		co_return;
	}

	void texture_showcase_page::play_spring_animation(float target_value, IInspectable const & sender)
	{
		auto current_ui_element = sender.as<Windows::UI::Xaml::UIElement>();

		m_spring_animation.FinalValue(Numerics::float3{ target_value, target_value, 1.0f });
		auto child_item = Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(current_ui_element, 0);
		auto image_item = Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(child_item, 0);

		auto visual = Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(unbox_value<Windows::UI::Xaml::UIElement>(image_item));
		visual.StartAnimation(hstring{ L"Scale" }, m_spring_animation);
	}

	IAsyncAction texture_showcase_page::onmouseenter_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		//sender.as<Windows::UI::Xaml::FrameworkElement>().Height(200);
		//sender.as<Windows::UI::Xaml::FrameworkElement>().Width(1000);
		//play_spring_animation(2.0f, sender);

		co_return;
	}

	IAsyncAction texture_showcase_page::onmouseexit_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		//sender.as<Windows::UI::Xaml::FrameworkElement>().Height(100);
		//play_spring_animation(1.0f, sender);
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
		auto selected_items_count = unbox_value<Windows::UI::Xaml::Controls::ListView>(sender).SelectedItems().Size();

		if (selected_items_count > 0)
		{
			VisualStateManager().GoToState(*this, L"valid_texture_selected", false);
		}
		else
		{
			VisualStateManager().GoToState(*this, L"valid_texture_not_selected", false);
		}
		co_return;
	}

	IAsyncAction texture_showcase_page::topleft_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"topleft_u" }, (float)topleft_u().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::topleft_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"topleft_v" }, (float)topleft_v().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::topright_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"topright_u" }, (float)topright_u().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::topright_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"topright_v" }, (float)topright_v().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::bottomleft_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"bottomleft_u" }, (float)bottomleft_u().Value());
		co_return;
	}
	IAsyncAction texture_showcase_page::bottomleft_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"bottomleft_v" }, (float)bottomleft_v().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::bottomright_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"bottomright_u" }, (float)bottomright_u().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::bottomright_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"bottomright_v" }, (float)bottomright_v().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::sampler_maxLOD_changed(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"sampler_maxLOD" }, sampler_maxLOD().Value());
		co_return;
	}

	IAsyncAction texture_showcase_page::sampler_minLOD_changed(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_ui_control_values.Insert(hstring{ L"sampler_minLOD" }, sampler_minLOD().Value());
		return IAsyncAction();
	}

	IAsyncAction texture_showcase_page::sampler_addressmode_u_changed(IInspectable const & sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const & args)
	{
		auto current_combobox = unbox_value<Windows::UI::Xaml::Controls::ComboBox>(sender);
		auto selected_addressmode = unbox_value<graphics::generic_attribute>(current_combobox.SelectedItem());
		co_return;
	}

	IAsyncAction texture_showcase_page::sampler_addressmode_v_changed(IInspectable const & sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const & args)
	{
		auto current_combobox = unbox_value<Windows::UI::Xaml::Controls::ComboBox>(sender);
		auto selected_addressmode = unbox_value<graphics::generic_attribute>(current_combobox.SelectedItem());
		co_return;
	}

	IAsyncAction texture_showcase_page::sampler_bordercolor_changed(IInspectable const & sender, winrt::Windows::UI::Xaml::Controls::ColorChangedEventArgs const args)
	{
		auto selected_color = sampler_bordercolor().Color();

		auto color_r = static_cast<float>(selected_color.R) / 255;
		auto color_g = static_cast<float>(selected_color.G) / 255;
		auto color_b = static_cast<float>(selected_color.B) / 255;
		auto color_a = static_cast<float>(selected_color.A) / 255;

		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_r" }, color_r);
		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_g" }, color_g);
		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_b" }, color_b);
		m_ui_control_values.Insert(hstring{ L"sampler_bordercolor_a" }, color_a);
		co_return;
	}

	IAsyncAction texture_showcase_page::render_onclick(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_renderer.start_render_loop();
		split_pane().IsPaneOpen(true);
		co_return;
	}
}
