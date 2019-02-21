﻿//
// Declaration of the texture_showcase_page class.
//

#pragma once

#include "winrt\graphics.h"
#include "texture_showcase_page.g.h"
#include "texture_showcase_vm.h"
#include "../os_utilities/os_utilities.h"
#include "bool_invert.h"
#include "ivector_to_bool.h"
#include "utilities.h"

namespace winrt::wzrd_editor::implementation
{
	struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page>
	{
		texture_showcase_page();

		IAsyncAction render_onclick(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
		IAsyncAction onclick_vertexshader_picker(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_pixelshader_picker(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_geometryshader_picker(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction delete_selected_shader(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction delete_selected_texture(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_pick_texture(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseenter_textures_list(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onmouseexit_textures_list(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction shader_selection_changed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction texture_selection_changed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction topleft_u_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction topleft_v_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction topright_u_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction topright_v_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction bottomleft_u_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction bottomleft_v_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction bottomright_u_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction bottomright_v_valuechanged(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction sampler_maxLOD_changed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction sampler_minLOD_changed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		IAsyncAction sampler_addressmode_u_changed(Windows::Foundation::IInspectable const & sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const & args);
		IAsyncAction sampler_addressmode_v_changed(Windows::Foundation::IInspectable const & sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const & args);
		IAsyncAction sampler_addressmode_w_changed(Windows::Foundation::IInspectable const & sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const & args);

		IAsyncAction sampler_bordercolor_changed(Windows::Foundation::IInspectable const & sender, const winrt::Windows::UI::Xaml::Controls::ColorChangedEventArgs args);

		wzrd_editor::texture_showcase_vm texture_showcase_vm();

	private:
		std::map<hstring, Windows::Foundation::IInspectable> m_ui_items = {};
		Windows::Foundation::Collections::IMap<hstring, IInspectable> m_ui_control_values = winrt::single_threaded_map<hstring, IInspectable>();
		graphics::renderer m_renderer;
		Windows::UI::Composition::SpringVector3NaturalMotionAnimation m_spring_animation = nullptr;
		void play_spring_animation(float target_value, Windows::Foundation::IInspectable const & sender);

		wzrd_editor::texture_showcase_vm m_texture_showcase_vm = winrt::make<wzrd_editor::implementation::texture_showcase_vm>();
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page, implementation::texture_showcase_page>
	{
	};
}
