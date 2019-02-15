#include "pch.h"
#include "ivector_to_bool.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	IInspectable ivector_to_bool::Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
	{
		auto res = get_class_name(value);
		if (unbox_value<uint32_t>(value) > 0)
		{
			return box_value(true);
		}
		else
		{
			return box_value(false);
		}
	}

	IInspectable ivector_to_bool::ConvertBack(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
	{
		if (unbox_value<uint32_t>(value) > 0)
		{
			return box_value(true);
		}
		else
		{
			return box_value(false);
		}
	}
}
