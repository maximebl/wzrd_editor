#include "pch.h"
#include "shader.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::graphics::implementation
{
	shader::shader(hstring const& shader_name, graphics::shader_type const& shader_type): m_shader_name(shader_name), m_shader_type(shader_type)
	{
		switch (shader_type)
		{
		case shader_type::vertex:
			m_type_glyph = L"Play";
			break;
		case shader_type::pixel:
			m_type_glyph = L"SelectAll";
			break;
		default:
			break;
		}
	}

	hstring shader::shader_name()
	{
		return m_shader_name;
	}

	void shader::shader_name(hstring const& value)
	{
		update_value(L"shader_name", m_shader_name, value);
	}

	hstring shader::type_glyph()
	{
		return m_type_glyph;
	}

	void shader::type_glyph(hstring const& value)
	{
		update_value(L"type_glyph", m_type_glyph, value);
	}

	bool shader::is_loading()
	{
		return m_is_loading;
	}

	void shader::is_loading(bool value)
	{
		update_value(L"is_loading", m_is_loading, value);
	}

	bool shader::is_error()
	{
		return m_is_error;
	}

	void shader::is_error(bool value)
	{
		update_value(L"is_error", m_is_error, value);
	}

	graphics::shader_type shader::shader_type()
	{
		return m_shader_type;
	}

	void shader::shader_type(graphics::shader_type const& value)
	{
		update_value(L"shader_type", m_shader_type, value);
	}

	winrt::event_token shader::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void shader::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
