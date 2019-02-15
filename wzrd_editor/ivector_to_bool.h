//
// Declaration of the ivector_to_bool class.
//

#pragma once

#include "ivector_to_bool.g.h"

namespace winrt::wzrd_editor::implementation
{
	using namespace Windows::Foundation;

	struct ivector_to_bool : ivector_to_boolT<ivector_to_bool>
	{
		ivector_to_bool() = default;
		IInspectable Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language);
		IInspectable ConvertBack(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, IInspectable const& parameter, hstring const& language);
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct ivector_to_bool : ivector_to_boolT<ivector_to_bool, implementation::ivector_to_bool>
	{
	};
}
