//
// Declaration of the texture_showcase_page class.
//

#pragma once

#include "texture_showcase_page.g.h"
#include "winrt\graphics.h"

namespace winrt::wzrd_editor::implementation
{
	using namespace winrt::Windows::Foundation;

    struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page>
    {
        texture_showcase_page();

		IAsyncAction render_onclick(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_vertexshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_pixelshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction menuflyout_clear_shaders_click(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction menuflyout_clear_textures_click(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_menuflyout_pick_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseenter_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseexit_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

	private:
		graphics::renderer m_renderer;
		Windows::UI::Composition::SpringVector3NaturalMotionAnimation m_spring_animation = nullptr;
		void play_spring_animation(float target_value, IInspectable const & sender);
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page, implementation::texture_showcase_page>
    {
    };
}
