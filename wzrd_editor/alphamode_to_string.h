#pragma once

#include "alphamode_to_string.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct alphamode_to_string : alphamode_to_stringT<alphamode_to_string>
    {
        alphamode_to_string() = default;
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct alphamode_to_string : alphamode_to_stringT<alphamode_to_string, implementation::alphamode_to_string>
    {
    };
}
