#pragma once

#include "Texture.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct Texture : TextureT<Texture>
	{
		Texture() = delete;
		Texture(winrt::hstring name);

		hstring Name();
		void Name(hstring const& value);
		bool Loading();
		void Loading(bool value);
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		winrt::hstring m_name;
		bool m_loading = false;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
	};
}
