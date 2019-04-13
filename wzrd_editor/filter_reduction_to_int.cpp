#include "pch.h"
#include "filter_reduction_to_int.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	Windows::Foundation::IInspectable filter_reduction_to_int::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		graphics::filter_reduction_type old_value = unbox_value<graphics::filter_reduction_type>(value);
		int32_t new_value = static_cast<int32_t>(old_value);
		return box_value(new_value);
	}

	Windows::Foundation::IInspectable filter_reduction_to_int::ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language)
	{
		graphics::filter_reduction_type old_value = unbox_value<graphics::filter_reduction_type>(value);
		int32_t new_value = static_cast<int32_t>(old_value);
		return box_value(new_value);
	}
}
