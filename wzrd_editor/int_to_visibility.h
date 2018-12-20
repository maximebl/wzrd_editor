#pragma once

#include "int_to_visibility.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct int_to_visibility : int_to_visibilityT<int_to_visibility>
	{
		int_to_visibility() = default;

		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct int_to_visibility : int_to_visibilityT<int_to_visibility, implementation::int_to_visibility>
	{
	};
}
