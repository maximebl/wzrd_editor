#include "pch.h"
#include "vertex.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::graphics::implementation
{
	vertex::vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v)
	{
		m_vertex = xm_vertex({ XMFLOAT3(x,y,z), XMFLOAT4(r,g,b,a), XMFLOAT2(u,v) });
	}

	float vertex::x()
	{
		return m_vertex.pos.x;
	}

	void vertex::x(float value)
	{
		update_value(L"x", m_vertex.pos.x, value);
	}

	float vertex::y()
	{
		return m_vertex.pos.y;
	}

	void vertex::y(float value)
	{
		update_value(L"y", m_vertex.pos.y, value);
	}

	float vertex::z()
	{
		return m_vertex.pos.z;
	}

	void vertex::z(float value)
	{
		update_value(L"z", m_vertex.pos.z, value);
	}

	float vertex::r()
	{
		return m_vertex.color.x;
	}

	void vertex::r(float value)
	{
		update_value(L"r", m_vertex.color.x, value);
	}

	float vertex::g()
	{
		return m_vertex.color.y;
	}

	void vertex::g(float value)
	{
		update_value(L"g", m_vertex.color.y, value);
	}

	float vertex::b()
	{
		return m_vertex.color.z;
	}

	void vertex::b(float value)
	{
		update_value(L"b", m_vertex.color.z, value);
	}

	float vertex::a()
	{
		return m_vertex.color.w;
	}

	void vertex::a(float value)
	{
		update_value(L"a", m_vertex.color.w, value);
	}

	float vertex::u()
	{
		return m_vertex.tex_coord.x;
	}

	void vertex::u(float value)
	{
		update_value(L"u", m_vertex.tex_coord.x, value);
	}

	float vertex::v()
	{
		return m_vertex.tex_coord.y;
	}

	void vertex::v(float value)
	{
		update_value(L"v", m_vertex.tex_coord.y, value);
	}

	winrt::event_token vertex::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void vertex::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
