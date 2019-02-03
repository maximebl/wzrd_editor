#include "pch.h"
#include "texture_showcase_page.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

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
		m_renderer.initialize_textures_showcase(swapchain_panel());
	}

	wzrd_editor::texture_showcase_vm texture_showcase_page::texture_showcase_vm()
	{
		return m_texture_showcase_vm;
	}

	IAsyncAction texture_showcase_page::onclick_pixelshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		graphics::shader new_shader = graphics::shader(hstring(L"default_ps"), graphics::shader_type::pixel);
		auto result = co_await m_renderer.pick_and_compile_shader(new_shader.shader_name(), hstring(L"PS"), hstring(L"ps_5_0"));
		co_return;
	}

	IAsyncAction texture_showcase_page::menuflyout_clear_shaders_click(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_renderer.clear_shaders();
		co_return;
	}

	IAsyncAction texture_showcase_page::menuflyout_clear_textures_click(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		return IAsyncAction();
	}

	IAsyncAction texture_showcase_page::onclick_pick_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		// receive the projected graphics::texture with the SoftwareBitmapSource already set

		auto new_texture = co_await m_renderer.pick_texture();

		new_texture.is_loading(false);

		if (new_texture != nullptr)
		{
			new_texture.is_error(true);
		}
		texture_showcase_vm().textures().Append(new_texture);
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
		sender.as<Windows::UI::Xaml::FrameworkElement>().Height(200);
		play_spring_animation(2.0f, sender);
		co_return;
	}

	IAsyncAction texture_showcase_page::onmouseexit_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		sender.as<Windows::UI::Xaml::FrameworkElement>().Height(100);
		play_spring_animation(1.0f, sender);
		co_return;
	}

	IAsyncAction texture_showcase_page::onclick_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		return IAsyncAction();
	}

	IAsyncAction texture_showcase_page::onclick_vertexshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		graphics::shader new_shader = graphics::shader(hstring(L"default_vs"), graphics::shader_type::vertex);
		auto result = co_await m_renderer.pick_and_compile_shader(new_shader.shader_name(), hstring(L"VS"), hstring(L"vs_5_0"));
		co_return;
	}

	IAsyncAction texture_showcase_page::render_onclick(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_renderer.start_render_loop();
		split_pane().IsPaneOpen(true);
		co_return;
	}
}
