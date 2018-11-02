#include "pch.h"
#include "Shader.h"

namespace winrt::wzrd_editor::implementation
{
	//Shader::Shader(hstring name, GraphicsResources::shader_type type) : m_name(name)
	//{
	//	Type(type);
	//}
	
	Shader::Shader(hstring name) : m_name(name)
	{
	}

	hstring Shader::Name()
	{
		return m_name;
	}

	void Shader::Name(hstring const& value)
	{
		m_name = value;
	}

	GraphicsResources::shader_type Shader::Type()
	{
		return m_type;
	}

	void Shader::Type(GraphicsResources::shader_type const& value)
	{
		m_type = value;
	}
	//hstring Shader::Type()
	//{
	//	return m_type;
	//}

	//void Shader::Type(hstring const& value)
	//{
	//	m_type = value;
	//}
}
