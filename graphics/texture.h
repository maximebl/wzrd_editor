//
// Declaration of the texture class.
//

#pragma once

#include "texture.g.h"

namespace winrt::graphics::implementation
{
	struct texture : textureT<texture>
	{
		texture();

		hstring texture_name();
		void texture_name(hstring const& value);

		bool is_loading();
		void is_loading(bool value);

		bool is_error();
		void is_error(bool value);

		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource bitmap_source();
		void bitmap_source(Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource const& value);

        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

		com_ptr<ID3D12Resource> texture_upload_buffer = nullptr;
		com_ptr<ID3D12Resource> texture_default_buffer = nullptr;
	private:
		bool m_is_loading = false;
		bool m_is_error = false;
		hstring m_texture_name;
		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource m_bitmap_source;

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
	struct texture : textureT<texture, implementation::texture>
	{
	};
}
