#include "pch.h"
#include "bool_invert.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	IInspectable bool_invert::Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
	{
		bool result = unbox_value<bool>(value);
		return box_value(!result);
	}

	IInspectable bool_invert::ConvertBack(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language)
	{
		bool result = unbox_value<bool>(value);
		return box_value(!result);
	}
}
