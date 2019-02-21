//
// Declaration of the generic_attribute class.
//

#pragma once

#include "generic_attribute.g.h"

namespace winrt::graphics::implementation
{
	struct generic_attribute : generic_attributeT<generic_attribute>
	{
		generic_attribute() = default;

		hstring attribute_description();
		void attribute_description(hstring const& value);
		hstring attribute_value();
		void attribute_value(hstring const& value);
		hstring attribute_name();
		void attribute_name(hstring const& value);
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:

		hstring m_attribute_name;
		hstring m_attribute_value;
		hstring m_attribute_description;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;

		void raise_property_changed(hstring const& property_name)
		{
			m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(property_name));
		}

		template <class T>
		void update_value(hstring const& property_name, T & var, T value)
		{
			if (var != value)
			{
				var = value;
				raise_property_changed(property_name);
			}
		}
	};
}

namespace winrt::graphics::factory_implementation
{
    struct generic_attribute : generic_attributeT<generic_attribute, implementation::generic_attribute>
    {
    };
}
