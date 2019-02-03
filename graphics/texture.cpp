#include "pch.h"
#include "texture.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::graphics::implementation
{
	texture::texture()
	{
	}

	hstring texture::texture_name()
	{
		return m_texture_name;
	}

	void texture::texture_name(hstring const& value)
	{
		update_value(L"texture_name", m_texture_name, value);
	}

	bool texture::is_loading()
	{
		return m_is_loading;
	}

	void texture::is_loading(bool value)
	{
		update_value(L"is_loading", m_is_loading, value);
	}

	bool texture::is_error()
	{
		return m_is_error;
	}

	void texture::is_error(bool value)
	{
		update_value(L"is_error", m_is_error, value);
	}

	Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource texture::bitmap_source()
	{
		return m_bitmap_source;
	}

	void texture::bitmap_source(Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource const& value)
	{
		update_value(L"bitmap_source", m_bitmap_source, value);
	}

	winrt::event_token texture::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void texture::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
