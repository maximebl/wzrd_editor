#pragma once

#include "Shader.g.h"
#include "GraphicsResources.h"

namespace winrt::wzrd_editor::implementation
{
	struct Shader : ShaderT<Shader>
	{
		Shader() = delete;
		//Shader(hstring name, GraphicsResources::shader_type type);
		Shader(hstring name);

		hstring Name();
		void Name(hstring const& value);

		GraphicsResources::shader_type Type();
		void Type(GraphicsResources::shader_type const& value);

		//hstring Type();
		//void Type(hstring const& value);

	private:
		hstring m_name;
		//hstring m_type;
		GraphicsResources::shader_type m_type;
	};
}
