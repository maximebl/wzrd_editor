//
// Declaration of the transforms_showcase_page class.
//

#pragma once

#include "transforms_showcase_page.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct transforms_showcase_page : transforms_showcase_pageT<transforms_showcase_page>
    {
        transforms_showcase_page();
		Windows::Foundation::IAsyncAction onclick_test(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);
		void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs e);
		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs e);
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct transforms_showcase_page : transforms_showcase_pageT<transforms_showcase_page, implementation::transforms_showcase_page>
    {
    };
}
