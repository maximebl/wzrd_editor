//
// Declaration of the texture_showcase_page class.
//

#pragma once

#include "winrt\graphics.h"
#include "texture_showcase_page.g.h"
#include "texture_showcase_vm.h"
#include "../os_utilities/os_utilities.h"
#include "bool_invert.h"
#include "ivector_to_bool.h"

namespace winrt::wzrd_editor::implementation
{
	using namespace winrt::Windows::Foundation;

	struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page>
	{
		texture_showcase_page();

		IAsyncAction render_onclick(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_vertexshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_pixelshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_geometryshader_picker(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction delete_selected_shader(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction delete_selected_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_pick_texture(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseenter_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseexit_textures_list(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction shader_selection_changed(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction texture_selection_changed(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction topleft_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction topleft_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction topright_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction topright_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction bottomleft_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction bottomleft_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction bottomright_u_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction bottomright_v_valuechanged(IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		wzrd_editor::texture_showcase_vm texture_showcase_vm();

	private:
		std::map<hstring, Windows::Foundation::IInspectable> m_ui_items = {};
		Windows::Foundation::Collections::IMap<hstring, float> m_ui_control_values = winrt::single_threaded_map<hstring, float>();
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
