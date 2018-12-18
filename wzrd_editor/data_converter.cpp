#include "pch.h"
#include "data_converter.h"
#include "MainPage.h"

using namespace winrt;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	data_converter::data_converter()
	{
	}

	Windows::Foundation::IInspectable data_converter::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		auto shaders_list_size = winrt::unbox_value<uint32_t>(value);
		if (shaders_list_size > 0)
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else {
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}

	Windows::Foundation::IInspectable data_converter::ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		auto shaders_list_size = winrt::unbox_value<uint32_t>(value);
		if (shaders_list_size > 0)
		{
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Visible);
		}
		else {
			return winrt::box_value(winrt::Windows::UI::Xaml::Visibility::Collapsed);
		}
	}
}
