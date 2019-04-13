//
// Declaration of the enum_to_int class.
//

#pragma once

#include "enum_to_int.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct enum_to_int : enum_to_intT<enum_to_int>
	{
		enum_to_int() = default;
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct enum_to_int : enum_to_intT<enum_to_int, implementation::enum_to_int>
	{
	};
}
