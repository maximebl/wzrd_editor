//
// Declaration of the filter_reduction_to_int class.
//

#pragma once

#include "filter_reduction_to_int.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct filter_reduction_to_int : filter_reduction_to_intT<filter_reduction_to_int>
	{
		filter_reduction_to_int() = default;
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);

	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct filter_reduction_to_int : filter_reduction_to_intT<filter_reduction_to_int, implementation::filter_reduction_to_int>
	{
	};
}
