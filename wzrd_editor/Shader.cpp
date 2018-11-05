#include "pch.h"
#include "Shader.h"

namespace winrt::wzrd_editor::implementation
{
	Shader::Shader(hstring name, wzrd_editor::ShaderType type) : m_name(name), m_type(type)
	{
		switch (type)
		{
		case winrt::wzrd_editor::ShaderType::vertex:
			m_type_glyph = L"Play";
			break;
		case winrt::wzrd_editor::ShaderType::pixel:
			m_type_glyph = L"SelectAll";
			break;
		default:
			break;
		}
	}

	hstring Shader::Name()
	{
		return m_name;
	}

	void Shader::Name(hstring const& value)
	{
		m_name = value;
	}

	hstring Shader::TypeGlyph()
	{
		return m_type_glyph;
	}

	void Shader::TypeGlyph(hstring const& value)
	{
		m_type_glyph = value;
	}


	wzrd_editor::ShaderType Shader::Type()
	{
		return m_type;
	}

	void Shader::Type(wzrd_editor::ShaderType const& value)
	{
		switch (m_type)
		{
		case winrt::wzrd_editor::ShaderType::vertex:
			m_type_glyph = L"&#xE768;";
			break;
		case winrt::wzrd_editor::ShaderType::pixel:
			m_type_glyph = L"&#xE74C;";
			break;
		default:
			break;
		}
		m_type = value;
	}

	bool Shader::Loading()
	{
		return m_loading;
	}

	void Shader::Loading(bool value)
	{
		m_loading = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Loading" });
	}

    winrt::event_token Shader::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
		return m_propertyChanged.add(handler);
    }

    void Shader::PropertyChanged(winrt::event_token const& token) noexcept
    {
		m_propertyChanged.remove(token);
    }
}
