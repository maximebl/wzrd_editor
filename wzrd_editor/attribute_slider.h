//
// Declaration of the attribute_slider class.
//

#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "attribute_slider.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct attribute_slider : attribute_sliderT<attribute_slider>
	{
		attribute_slider();

		static Windows::UI::Xaml::DependencyProperty LabelProperty();
		static Windows::UI::Xaml::DependencyProperty value_property();
		static void value_property(Windows::UI::Xaml::DependencyProperty const& value);

		hstring Label();
		void Label(hstring const& value);

		float value_prop();
		void value_prop(float const& value);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		static Windows::UI::Xaml::DependencyProperty m_labelProperty;
		static Windows::UI::Xaml::DependencyProperty m_value_property;
		hstring m_label = L"";
		float m_value = 0.0f;

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

namespace winrt::wzrd_editor::factory_implementation
{
	struct attribute_slider : attribute_sliderT<attribute_slider, implementation::attribute_slider>
	{
	};
}
