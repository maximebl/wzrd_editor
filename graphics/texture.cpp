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

	hstring texture::file_name()
	{
		return m_file_name;
	}

	void texture::file_name(hstring const & value)
	{
		update_value(L"file_name", m_file_name, value);
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

    uint64_t texture::mip_levels()
    {
		return m_mip_levels;
    }

    void texture::mip_levels(uint64_t value)
    {
		update_value(L"mip_levels", m_mip_levels, value);
    }

    uint64_t texture::width()
    {
		return m_width;
    }

    void texture::width(uint64_t value)
    {
		update_value(L"width", m_width, value);
    }

    uint64_t texture::height()
    {
		return m_height;
    }

    void texture::height(uint64_t value)
    {
		update_value(L"height", m_height, value);
    }

    hstring texture::dimension()
    {
		return m_dimension;
    }

    void texture::dimension(hstring const& value)
    {
		update_value(L"dimension", m_dimension, value);
    }

    uint64_t texture::row_pitch()
    {
		return m_row_pitch;
    }

    void texture::row_pitch(uint64_t value)
    {
		update_value(L"row_pitch", m_row_pitch, value);
    }

    uint64_t texture::slice_pitch()
    {
		return m_slice_pitch;
    }

    void texture::slice_pitch(uint64_t value)
    {
		update_value(L"slice_pitch", m_slice_pitch, value);
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
