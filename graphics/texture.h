﻿//
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

		hstring file_name();
		void file_name(hstring const& value);

		bool is_loading();
		void is_loading(bool value);

		bool is_error();
		void is_error(bool value);

		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource bitmap_source();
		void bitmap_source(Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource const& value);

		uint64_t mip_levels();
		void mip_levels(uint64_t value);

		uint64_t width();
		void width(uint64_t value);

		uint64_t height();
		void height(uint64_t value);

		hstring dimension();
		void dimension(hstring const& value);

		uint64_t row_pitch();
		void row_pitch(uint64_t value);

		uint64_t slice_pitch();
		void slice_pitch(uint64_t value);

		float current_lod();
		void current_lod(float value);

		float scale();
		void scale(float value);

		Windows::UI::Color sampler_border_color();
		void sampler_border_color(Windows::UI::Color const& value);

		float sampler_minLOD();
		void sampler_minLOD(float value);

		float sampler_maxLOD();
		void sampler_maxLOD(float value);

		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> mipmaps();
		void mipmaps(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const value);

		float topleft_u();
		void topleft_u(float value);
		float topleft_v();
		void topleft_v(float value);
		float topleft_x();
		void topleft_x(float value);
		float topleft_y();
		void topleft_y(float value);
		float topright_u();
		void topright_u(float value);
		float topright_v();
		void topright_v(float value);
		float topright_x();
		void topright_x(float value);
		float topright_y();
		void topright_y(float value);
		float bottomleft_u();
		void bottomleft_u(float value);
		float bottomleft_v();
		void bottomleft_v(float value);
		float bottomleft_x();
		void bottomleft_x(float value);
		float bottomleft_y();
		void bottomleft_y(float value);
		float bottomright_u();
		void bottomright_u(float value);
		float bottomright_v();
		void bottomright_v(float value);
		float bottomright_x();
		void bottomright_x(float value);
		float bottomright_y();
		void bottomright_y(float value);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

		com_ptr<ID3D12Resource> texture_upload_buffer = nullptr;
		com_ptr<ID3D12Resource> texture_default_buffer = nullptr;

	private:
		bool m_is_loading = false;
		bool m_is_error = false;
		hstring m_texture_name;
		hstring m_file_name;
		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource m_bitmap_source;
		Windows::UI::Color m_sampler_border_color;
		float m_sampler_minLOD = 0.0f;
		float m_sampler_maxLOD = 10.f;

		Windows::Foundation::Collections::IObservableVector<IInspectable> m_mipmaps;

		uint64_t m_mip_levels = 1.0f;
		uint64_t m_width = 0.0f;
		uint64_t m_height = 0.0f;
		hstring m_dimension = hstring{ L"" };
		uint64_t m_row_pitch = 0.0f;
		uint64_t m_slice_pitch = 0.0f;
		float m_current_lod = 0.0f;
		float m_scale = 1.0f;

		float m_topleft_u = 0.0f;
		float m_topleft_v = 0.0f;
		float m_topleft_x = -0.3f;
		float m_topleft_y = 0.3f;
		float m_topright_u = 1.0f;
		float m_topright_v = 0.0f;
		float m_topright_x = 0.3f;
		float m_topright_y = 0.3f;
		float m_bottomleft_u = 0.0f;
		float m_bottomleft_v = 1.0f;
		float m_bottomleft_x = -0.3f;
		float m_bottomleft_y = -0.3f;
		float m_bottomright_u = 1.0f;
		float m_bottomright_v = 1.0f;
		float m_bottomright_x = 0.3f;
		float m_bottomright_y = -0.3f;

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
