#pragma once

#include "shader.g.h"

namespace winrt::graphics::implementation
{
    struct shader : shaderT<shader>
    {
        shader() = delete;
        shader(hstring const& name, graphics::shader_type const& type);

        hstring shader_name();
        void shader_name(hstring const& value);
        hstring type_glyph();
        void type_glyph(hstring const& value);
        bool is_loading();
        void is_loading(bool value);
        bool is_error();
        void is_error(bool value);
        graphics::shader_type shader_type();
        void shader_type(graphics::shader_type const& value);
        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
    };
}

namespace winrt::graphics::factory_implementation
{
    struct shader : shaderT<shader, implementation::shader>
    {
    };
}
