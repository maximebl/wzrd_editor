#include "pch.h"
#include "wzrd_vec3.h"

namespace winrt::wzrd_editor::implementation
{
	wzrd_vec3::wzrd_vec3(float const& x, float const& y, float const& z) : m_x{x}, m_y{y}, m_z{z}
	{}

    float wzrd_vec3::x()
    {
		return m_x;
    }

    void wzrd_vec3::x(float value)
    {
		if (value != m_x)
		{
			m_x = value;
			m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"x" });
		}
    }

	float wzrd_vec3::y()
	{
		return m_y;
	}

	void wzrd_vec3::y(float value)
	{
		if (value != m_y)
		{
			m_y = value;
			m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"y" });
		}
	}

	float wzrd_vec3::z()
	{
		return m_z;
	}

	void wzrd_vec3::z(float value)
	{
		if (value != m_z)
		{
			m_z = value;
			m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"z" });
		}
	}

    winrt::event_token wzrd_vec3::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
		return m_propertyChanged.add(handler);
    }

    void wzrd_vec3::PropertyChanged(winrt::event_token const& token) noexcept
    {
		m_propertyChanged.remove(token);
    }
}
