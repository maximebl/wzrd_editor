//
// Declaration of the data_converter class.
//

#pragma once

#include "data_converter.g.h"
#include "GeometryViewModel.h"

namespace winrt::wzrd_editor::implementation
{
    struct data_converter : data_converterT<data_converter>
    {
        data_converter();

		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct data_converter : data_converterT<data_converter, implementation::data_converter>
	{
	};
}
