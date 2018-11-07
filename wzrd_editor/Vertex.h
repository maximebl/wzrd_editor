#pragma once

#include "Vertex.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct Vertex : VertexT<Vertex>
	{
		Vertex() = delete;

		Vertex(
			float m_x, float m_y, float m_z,
			float m_r, float m_g, float m_b, float m_a,
			float m_u, float m_v
		);

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

	private:
		float m_x;
		float m_y;
		float m_z;

		float m_r;
		float m_g;
		float m_b;
		float m_a;

		float m_u;
		float m_v;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
	};
}
