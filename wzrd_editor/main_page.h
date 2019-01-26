//
// Declaration of the main_page class.
//

#pragma once

#include "main_page.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct main_page : main_pageT<main_page>
	{
		main_page();
		void navigation_view_invoked(Windows::Foundation::IInspectable const & /* sender */, Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const & args);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct main_page : main_pageT<main_page, implementation::main_page>
	{
	};
}
