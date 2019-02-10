//
// Declaration of the texture_showcase_page class.
//

#pragma once

#include "winrt\graphics.h"
#include "texture_showcase_page.g.h"
#include "texture_showcase_vm.h"
#include "../os_utilities/os_utilities.h"
#include "bool_invert.h"

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
		IAsyncAction onclick_pick_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseenter_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseexit_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		wzrd_editor::texture_showcase_vm texture_showcase_vm();

	private:
		graphics::renderer m_renderer;
		Windows::UI::Composition::SpringVector3NaturalMotionAnimation m_spring_animation = nullptr;
		void play_spring_animation(float target_value, IInspectable const & sender);

		wzrd_editor::texture_showcase_vm m_texture_showcase_vm = winrt::make<wzrd_editor::implementation::texture_showcase_vm>();
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page, implementation::texture_showcase_page>
    {
    };
}
