#include "pch.h"
#include "bool_to_visibility_inverted.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	Windows::Foundation::IInspectable bool_to_visibility_inverted::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		if (unbox_value<bool>(value))
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
		else
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
	}

	Windows::Foundation::IInspectable bool_to_visibility_inverted::ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		if (unbox_value<bool>(value))
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
		else
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
	}
}
