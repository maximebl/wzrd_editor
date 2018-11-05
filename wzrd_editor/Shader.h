#pragma once

#include "Shader.g.h"
#include "GraphicsResources.h"

namespace winrt::wzrd_editor::implementation
{
	struct Shader : ShaderT<Shader>
	{
		Shader() = delete;
		Shader(hstring name, wzrd_editor::ShaderType type);

		hstring Name();
		void Name(hstring const& value);

		hstring TypeGlyph();
		void TypeGlyph(hstring const& value);

		wzrd_editor::ShaderType Type();
		void Type(wzrd_editor::ShaderType const& value);

		bool Loading();
		void Loading(bool value);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;
	private:
		hstring m_name;
		hstring m_type_glyph;
		bool m_loading;

		wzrd_editor::ShaderType m_type;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
	};
}
