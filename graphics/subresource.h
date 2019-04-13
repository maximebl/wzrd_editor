//
// Declaration of the subresource class.
//

#pragma once

#include "subresource.g.h"

namespace winrt::graphics::implementation
{
	struct subresource : subresourceT<subresource>
	{
		subresource() = default;

		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource mipmap_bitmap_source();
		void mipmap_bitmap_source(Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource const& value);
		Windows::UI::Xaml::Media::Imaging::BitmapImage mipmap_bitmap_source2();
		void mipmap_bitmap_source2(Windows::UI::Xaml::Media::Imaging::BitmapImage const& value);
		uint8_t mip_level();
		void mip_level(uint8_t value);
		uint64_t width();
		void width(uint64_t value);
		uint64_t height();
		void height(uint64_t value);
		uint64_t row_pitch();
		void row_pitch(uint64_t value);
		uint64_t slice_pitch();
		void slice_pitch(uint64_t value);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource m_mipmap_bitmap_source = nullptr;
		Windows::UI::Xaml::Media::Imaging::BitmapImage m_mipmap_bitmap_source2 = nullptr;
		uint8_t m_mip_level = 0;
		uint64_t m_width = 0;
		uint64_t m_height = 0;
		uint64_t m_row_pitch = 0;
		uint64_t m_slice_pitch = 0;

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
	struct subresource : subresourceT<subresource, implementation::subresource>
	{
	};
}
