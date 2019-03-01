#include "pch.h"
#include "subresource.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::graphics::implementation
{
	Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource subresource::mipmap_bitmap_source()
	{
		return m_mipmap_bitmap_source;
	}

	void subresource::mipmap_bitmap_source(Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource const& value)
	{
		update_value(L"mipmap_bitmap_source", m_mipmap_bitmap_source, value);
	}

	uint8_t subresource::mip_level()
	{
		return m_mip_level;
	}

	void subresource::mip_level(uint8_t value)
	{
		update_value(L"mip_level", m_mip_level, value);
	}

	uint64_t subresource::width()
	{
		return m_width;
	}

	void subresource::width(uint64_t value)
	{
		update_value(L"width", m_width, value);
	}

	uint64_t subresource::height()
	{
		return m_height;
	}

	void subresource::height(uint64_t value)
	{
		update_value(L"height", m_height, value);
	}

	uint64_t subresource::row_pitch()
	{
		return m_row_pitch;
	}

	void subresource::row_pitch(uint64_t value)
	{
		update_value(L"row_pitch", m_row_pitch, value);
	}

	uint64_t subresource::slice_pitch()
	{
		return m_slice_pitch;
	}

	void subresource::slice_pitch(uint64_t value)
	{
		update_value(L"slice_pitch", m_slice_pitch, value);
	}

	winrt::event_token subresource::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void subresource::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
