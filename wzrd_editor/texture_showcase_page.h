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
#include "float_to_int.h"
#include "enum_to_int.h"
#include "alphamode_to_string.h"
#include "bool_to_visibility_inverted.h"
#include "filter_reduction_to_int.h"
#include "utilities.h"
#include "texture_vm.h"
#include "../os_utilities/os_utilities.h"
#include <chrono>

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
		IAsyncAction onclick_create_texture(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction shader_selection_changed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction texture_selection_changed(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		IAsyncAction onclick_create_dds(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		wzrd_editor::texture_showcase_vm texture_showcase_vm();
		void texture_showcase_vm(wzrd_editor::texture_showcase_vm const& value);

	private:
		Windows::Foundation::Collections::IMap<hstring, IInspectable> m_ui_control_values = winrt::single_threaded_map<hstring, IInspectable>();
		graphics::renderer m_renderer;
		wzrd_editor::texture_showcase_vm m_texture_showcase_vm = winrt::make<wzrd_editor::implementation::texture_showcase_vm>();
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct texture_showcase_page : texture_showcase_pageT<texture_showcase_page, implementation::texture_showcase_page>
	{
	};
}
