#include "pch.h"
#include "alphamode_to_string.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	Windows::Foundation::IInspectable alphamode_to_string::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		graphics::alpha_mode new_alpha_mode = unbox_value<graphics::alpha_mode>(value);

		hstring converted_alpha_mode = L"";

		switch (new_alpha_mode)
		{
		case winrt::graphics::alpha_mode::unknown:
			converted_alpha_mode = L"unknown";
			break;
		case winrt::graphics::alpha_mode::straight:
			converted_alpha_mode = L"straight";
			break;
		case winrt::graphics::alpha_mode::premultiplied:
			converted_alpha_mode = L"premultiplied";
			break;
		case winrt::graphics::alpha_mode::opaque:
			converted_alpha_mode = L"opaque";
			break;
		case winrt::graphics::alpha_mode::custom:
			converted_alpha_mode = L"custom";
			break;
		default:
			converted_alpha_mode = L"unknown";
			break;
		}

		return box_value(converted_alpha_mode);
	}

	Windows::Foundation::IInspectable alphamode_to_string::ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		return value;
	}
}
