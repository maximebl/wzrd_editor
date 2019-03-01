//
// Declaration of the float_to_int class.
//

#pragma once

#include "float_to_int.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct float_to_int : float_to_intT<float_to_int>
	{
		float_to_int() = default;

		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct float_to_int : float_to_intT<float_to_int, implementation::float_to_int>
	{
	};
}
