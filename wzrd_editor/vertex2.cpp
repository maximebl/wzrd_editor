#include "pch.h"
#include "vertex2.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	vertex2::vertex2(float x, float y, float z, float r, float g, float b, float a, float u, float v)
	{
		m_vertex = xm_vertex({ XMFLOAT3(x,y,z), XMFLOAT4(r,g,b,a), XMFLOAT2(u,v) });
	}

	float vertex2::x()
	{
		return m_vertex.pos.x;
	}

	void vertex2::x(float value)
	{
		update_value(L"x", m_vertex.pos.x, value);
	}

	float vertex2::y()
	{
		return m_vertex.pos.y;
	}

	void vertex2::y(float value)
	{
		update_value(L"y", m_vertex.pos.y, value);
	}

	float vertex2::z()
	{
		return m_vertex.pos.z;
	}

	void vertex2::z(float value)
	{
		update_value(L"z", m_vertex.pos.z, value);
	}

	float vertex2::r()
	{
		return m_vertex.color.x;
	}

	void vertex2::r(float value)
	{
		update_value(L"r", m_vertex.color.x, value);
	}

	float vertex2::g()
	{
		return m_vertex.color.y;
	}

	void vertex2::g(float value)
	{
		update_value(L"g", m_vertex.color.y, value);
	}

	float vertex2::b()
	{
		return m_vertex.color.z;
	}

	void vertex2::b(float value)
	{
		update_value(L"b", m_vertex.color.z, value);
	}

	float vertex2::a()
	{
		return m_vertex.color.w;
	}

	void vertex2::a(float value)
	{
		update_value(L"a", m_vertex.color.w, value);
	}

	float vertex2::u()
	{
		return m_vertex.tex_coord.x;
	}

	void vertex2::u(float value)
	{
		update_value(L"u", m_vertex.tex_coord.x, value);
	}

	float vertex2::v()
	{
		return m_vertex.tex_coord.y;
	}

	void vertex2::v(float value)
	{
		update_value(L"v", m_vertex.tex_coord.y, value);
	}

	winrt::event_token vertex2::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void vertex2::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
