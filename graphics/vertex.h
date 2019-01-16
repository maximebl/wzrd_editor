//
// Declaration of the vertex class.
//

#pragma once

#include "vertex.g.h"

using namespace DirectX;

struct xm_vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 tex_coord;
};

namespace winrt::graphics::implementation
{
    struct vertex : vertexT<vertex>
    {
        vertex() = default;
		vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v);

		float x();
		void x(float value);
		float y();
		void y(float value);
		float z();
		void z(float value);
		float r();
		void r(float value);
		float g();
		void g(float value);
		float b();
		void b(float value);
		float a();
		void a(float value);
		float u();
		void u(float value);
		float v();
		void v(float value);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

		xm_vertex m_vertex;

	private:
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
    struct vertex : vertexT<vertex, implementation::vertex>
    {
    };
}
