#include "pch.h"
#include "texture_showcase_page.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	texture_showcase_page::texture_showcase_page()
	{
		InitializeComponent();

		m_renderer.enable_debug_layer();
		m_renderer.initialize_textures_showcase(swapchain_panel());
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

	IAsyncAction texture_showcase_page::onclick_vertexshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		graphics::shader new_shader = graphics::shader(hstring(L"default_vs"), graphics::shader_type::vertex);
		auto result = co_await m_renderer.pick_and_compile_shader(new_shader.shader_name(), hstring(L"VS"), hstring(L"vs_5_0"));
		co_return;
	}

	IAsyncAction texture_showcase_page::render_onclick(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		m_renderer.start_render_loop();
		co_return;
	}
}
