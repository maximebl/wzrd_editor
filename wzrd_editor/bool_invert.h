//
// Declaration of the bool_invert class.
//

#pragma once

#include "bool_invert.g.h"

namespace winrt::wzrd_editor::implementation
{
	using namespace Windows::Foundation;

	struct bool_invert : bool_invertT<bool_invert>
	{
		bool_invert() = default;
		IInspectable Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language);
		IInspectable ConvertBack(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct bool_invert : bool_invertT<bool_invert, implementation::bool_invert>
	{
	};
}
