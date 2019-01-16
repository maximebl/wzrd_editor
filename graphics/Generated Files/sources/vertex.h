#pragma once

#include "vertex.g.h"

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
    };
}

namespace winrt::graphics::factory_implementation
{
    struct vertex : vertexT<vertex, implementation::vertex>
    {
    };
}
