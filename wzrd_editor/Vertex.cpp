#include "pch.h"
#include "Vertex.h"

namespace winrt::wzrd_editor::implementation
{
	Vertex::Vertex(
		float x, float y, float z,
		float r, float g, float b, float a,
		float u, float v
	) : 
		m_x(x), m_y(y), m_z(z),
		m_r(r), m_g(g), m_b(b), m_a(a),
		m_u(u), m_v(v)
	{}

	float Vertex::x()
	{
		return m_x;
	}

	void Vertex::x(float value)
	{
		m_x = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"x" });
	}

	float Vertex::y()
	{
		return m_y;
	}

	void Vertex::y(float value)
	{
		m_y = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"y" });
	}

	float Vertex::z()
	{
		return m_z;
	}

	void Vertex::z(float value)
	{
		m_z = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"z" });
	}

	float Vertex::r()
	{
		return m_r;
	}

	void Vertex::r(float value)
	{
		m_r = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"r" });
	}

	float Vertex::g()
	{
		return m_g;
	}

	void Vertex::g(float value)
	{
		m_g = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"g" });
	}

	float Vertex::b()
	{
		return m_b;
	}

	void Vertex::b(float value)
	{
		m_b = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"b" });
	}

	float Vertex::a()
	{
		return m_a;
	}

	void Vertex::a(float value)
	{
		m_a = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"a" });
	}

	float Vertex::u()
	{
		return m_u;
	}

	void Vertex::u(float value)
	{
		m_u = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"u" });
	}

	float Vertex::v()
	{
		return m_v;
	}

	void Vertex::v(float value)
	{
		m_v = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"v" });
	}

	winrt::event_token Vertex::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_propertyChanged.add(handler);
	}

	void Vertex::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_propertyChanged.remove(token);
	}
}
