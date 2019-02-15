#include "pch.h"
#include "generic_attribute.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::graphics::implementation
{
	hstring generic_attribute::attribute_description()
	{
		return m_attribute_description;
	}

	void generic_attribute::attribute_description(hstring const& value)
	{
		update_value(L"attribute_description", m_attribute_description, value);
	}

	hstring generic_attribute::attribute_value()
	{
		return m_attribute_value;
	}

	void generic_attribute::attribute_value(hstring const& value)
	{
		update_value(L"attribute_value", m_attribute_value, value);
	}

	hstring generic_attribute::attribute_name()
	{
		return m_attribute_name;
	}

	void generic_attribute::attribute_name(hstring const& value)
	{
		update_value(L"attribute_name", m_attribute_name, value);
	}

	winrt::event_token generic_attribute::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void generic_attribute::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
