#include "pch.h"
#include "attribute_slider.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	Windows::UI::Xaml::DependencyProperty attribute_slider::m_labelProperty =
		Windows::UI::Xaml::DependencyProperty::Register(
			L"Label",
			winrt::xaml_typename<winrt::hstring>(),
			winrt::xaml_typename<wzrd_editor::attribute_slider>(),
			Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	Windows::UI::Xaml::DependencyProperty attribute_slider::m_value_property =
		Windows::UI::Xaml::DependencyProperty::Register(
			L"value_prop",
			winrt::xaml_typename<winrt::hstring>(),
			winrt::xaml_typename<wzrd_editor::attribute_slider>(),
			Windows::UI::Xaml::PropertyMetadata{ nullptr }
	);

	attribute_slider::attribute_slider()
	{
		InitializeComponent();
	}

	hstring attribute_slider::Label()
	{
		return m_label;
	}

	void attribute_slider::Label(hstring const& value)
	{
		update_value(L"Label", m_label, value);
	}

	float attribute_slider::value_prop()
	{
		return m_value;
	}

	void attribute_slider::value_prop(float const& value)
	{
		update_value(L"value_prop", m_value, value);
	}

	Windows::UI::Xaml::DependencyProperty attribute_slider::LabelProperty()
	{
		return m_labelProperty;
	}

	Windows::UI::Xaml::DependencyProperty attribute_slider::value_property()
	{
		return m_value_property;
	}

    void attribute_slider::value_property(Windows::UI::Xaml::DependencyProperty const& value)
    {
		m_value_property = value;
    }

	winrt::event_token attribute_slider::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void attribute_slider::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
