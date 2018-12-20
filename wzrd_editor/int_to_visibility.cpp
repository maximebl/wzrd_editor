#include "pch.h"
#include "int_to_visibility.h"

namespace winrt::wzrd_editor::implementation
{
	Windows::Foundation::IInspectable int_to_visibility::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		if (winrt::unbox_value<uint32_t>(value) > 0)
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else {
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}

	Windows::Foundation::IInspectable int_to_visibility::ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		if (winrt::unbox_value<uint32_t>(value) > 0)
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else {
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}
}
