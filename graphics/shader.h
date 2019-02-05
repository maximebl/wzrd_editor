//
// Declaration of the shader class.
//

#pragma once

#include "shader.g.h"

namespace winrt::graphics::implementation
{
    struct shader : shaderT<shader>
    {
        shader() = delete;
        shader(hstring const& shader_name, graphics::shader_type const& shader_type);

        hstring shader_name();
        void shader_name(hstring const& value);

        hstring file_name();
        void file_name(hstring const& value);

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

	private:

		hstring m_shader_name;
		hstring m_file_name;
		hstring m_type_glyph;
		graphics::shader_type m_shader_type;
		bool m_is_loading = false;
		bool m_is_error = false;

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
    struct shader : shaderT<shader, implementation::shader>
    {
    };
}
