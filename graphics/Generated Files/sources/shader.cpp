#include "pch.h"
#include "shader.h"

namespace winrt::graphics::implementation
{
    shader::shader(hstring const& name, graphics::shader_type const& type)
    {
        throw hresult_not_implemented();
    }

    hstring shader::shader_name()
    {
        throw hresult_not_implemented();
    }

    void shader::shader_name(hstring const& value)
    {
        throw hresult_not_implemented();
    }

    hstring shader::type_glyph()
    {
        throw hresult_not_implemented();
    }

    void shader::type_glyph(hstring const& value)
    {
        throw hresult_not_implemented();
    }

    bool shader::is_loading()
    {
        throw hresult_not_implemented();
    }

    void shader::is_loading(bool value)
    {
        throw hresult_not_implemented();
    }

    bool shader::is_error()
    {
        throw hresult_not_implemented();
    }

    void shader::is_error(bool value)
    {
        throw hresult_not_implemented();
    }

    graphics::shader_type shader::shader_type()
    {
        throw hresult_not_implemented();
    }

    void shader::shader_type(graphics::shader_type const& value)
    {
        throw hresult_not_implemented();
    }

    winrt::event_token shader::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        throw hresult_not_implemented();
    }

    void shader::PropertyChanged(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
}
