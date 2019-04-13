//
// Declaration of the bool_to_visibility_inverted class.
//

#pragma once

#include "bool_to_visibility_inverted.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct bool_to_visibility_inverted : bool_to_visibility_invertedT<bool_to_visibility_inverted>
	{
		bool_to_visibility_inverted() = default;

		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct bool_to_visibility_inverted : bool_to_visibility_invertedT<bool_to_visibility_inverted, implementation::bool_to_visibility_inverted>
	{
	};
}
