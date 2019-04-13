#include "pch.h"
#include "enum_to_int.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	Windows::Foundation::IInspectable enum_to_int::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		int32_t new_value = 1;
		winrt::hstring class_name = winrt::get_class_name(value);

		if (class_name == L"Windows.Foundation.IReference`1<graphics.filtering_method>")
		{
			new_value = static_cast<int32_t>(unbox_value<graphics::filtering_method>(value));
		}
		if (class_name == L"Windows.Foundation.IReference`1<graphics.filter_reduction_type>")
		{
			new_value = static_cast<int32_t>(unbox_value<graphics::filter_reduction_type>(value));
		}
		if (class_name == L"Windows.Foundation.IReference`1<graphics.filter_comparison_function>")
		{
			int32_t old_value = static_cast<int32_t>(unbox_value<graphics::filter_comparison_function>(value));
			new_value = --old_value;
		}
		if (class_name == L"Windows.Foundation.IReference`1<graphics.texture_address_mode>")
		{
			int32_t old_value = static_cast<int32_t>(unbox_value<graphics::texture_address_mode>(value));
			new_value = --old_value;
		}
		if (class_name == L"Windows.Foundation.IReference`1<graphics.sampling_function>")
		{
			new_value = static_cast<int32_t>(unbox_value<graphics::sampling_function>(value));
		}

		return box_value(new_value);
	}

	Windows::Foundation::IInspectable enum_to_int::ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		int32_t new_value = 1;

		hstring target_type_name = targetType.Name;

		if (target_type_name == L"graphics.filtering_method")
		{
			new_value = static_cast<int32_t>(unbox_value<graphics::filtering_method>(value));
		}
		if (target_type_name == L"graphics.filter_reduction_type")
		{
			new_value = static_cast<int32_t>(unbox_value<graphics::filter_reduction_type>(value));
		}
		if (target_type_name == L"graphics.filter_comparison_function")
		{
			int32_t old_value = static_cast<int32_t>(unbox_value<graphics::filter_comparison_function>(value));
			new_value = ++old_value;
		}
		if (target_type_name == L"graphics.texture_address_mode")
		{
			int32_t old_value = static_cast<int32_t>(unbox_value<graphics::texture_address_mode>(value));
			new_value = ++old_value;
		}
		if (target_type_name == L"graphics.sampling_function")
		{
			new_value = static_cast<int32_t>(unbox_value<graphics::sampling_function>(value));
		}

		return box_value(new_value);
	}
}
