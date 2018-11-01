#pragma once

#include "Shader.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct Shader : ShaderT<Shader>
	{
		Shader() = delete;
		Shader(hstring name);

		hstring Name();
		void Name(hstring const& value);

		hstring Type();
		void Type(hstring const& value);

	private:
		hstring m_name;
		hstring m_type;
	};
}
