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

		hstring file_name();
		void file_name(hstring const& value);

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

		float forced_miplevel();
		void forced_miplevel(float value);

		float sample_comparison_value();
		void sample_comparison_value(float value);

		uint32_t max_anisotropy();
		void max_anisotropy(uint32_t value);

		bool is_forced_mip_level();
		void is_forced_mip_level(bool value);

		bool is_screen_coords();
		void is_screen_coords(bool value);

		int32_t x_pixel_offset();
		void x_pixel_offset(int32_t value);

		int32_t y_pixel_offset();
		void y_pixel_offset(int32_t value);

		bool is_anisotropic();
		void is_anisotropic(bool value);

		graphics::filtering_method minification_filter();
		void minification_filter(graphics::filtering_method const& value);
		graphics::filtering_method magnification_filter();
		void magnification_filter(graphics::filtering_method const& value);
		graphics::filtering_method miplevel_sampling_filter();
		void miplevel_sampling_filter(graphics::filtering_method const& value);

		graphics::filter_reduction_type reduction();
		void reduction(graphics::filter_reduction_type const& value);

		graphics::filter_comparison_function comparison_function();
		void comparison_function(graphics::filter_comparison_function const& value);

		graphics::sampling_function sampling_function();
		void sampling_function(graphics::sampling_function const& value);

		graphics::alpha_mode alpha_mode();
		void alpha_mode(graphics::alpha_mode const& value);

		uint32_t dds_array_size();
		void dds_array_size(uint32_t value);

		uint32_t dds_array_index();
		void dds_array_index(uint32_t value);

		Windows::Foundation::Collections::IObservableVector<IInspectable> mipmaps();
		void mipmaps(Windows::Foundation::Collections::IObservableVector<IInspectable> const& value);

		graphics::texture_address_mode u_address_mode();
		void u_address_mode(graphics::texture_address_mode const& value);
		graphics::texture_address_mode v_address_mode();
		void v_address_mode(graphics::texture_address_mode const& value);
		graphics::texture_address_mode w_address_mode();
		void w_address_mode(graphics::texture_address_mode const& value);

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
		hstring m_texture_name;
		hstring m_file_name;
		Windows::UI::Xaml::Media::Imaging::SoftwareBitmapSource m_bitmap_source;
		Windows::UI::Color m_sampler_border_color;
		float m_sampler_minLOD = 0.0f;
		float m_sampler_maxLOD = 10.f;
		float m_forced_miplevel = 0.0f;
		bool m_is_forced_mip_level = false;
		bool m_is_screen_coords = false;
		int32_t m_x_pixel_offset = 0;
		int32_t m_y_pixel_offset = 0;
		float m_sample_comparison_value = 0.0f;
		uint32_t m_max_anisotropy = 16;

		bool m_is_anisotropic = false;

		filtering_method m_minification_filter = filtering_method::linear_interpolation;
		filtering_method m_magnification_filter = filtering_method::linear_interpolation;
		filtering_method m_miplevel_sampling_filter = filtering_method::linear_interpolation;
		graphics::sampling_function m_sampling_function = graphics::sampling_function::sample;

		filter_reduction_type m_reduction = filter_reduction_type::standard;
		filter_comparison_function m_comparison_function = filter_comparison_function::always;

		texture_address_mode m_u_address_mode = texture_address_mode::wrap;
		texture_address_mode m_v_address_mode = texture_address_mode::wrap;
		texture_address_mode m_w_address_mode = texture_address_mode::wrap;
		graphics::alpha_mode m_alpha_mode = alpha_mode::unknown;
		uint32_t m_dds_array_size = 0;
		uint32_t m_dds_array_index = 0;

		Windows::Foundation::Collections::IObservableVector<IInspectable> m_mipmaps = single_threaded_observable_vector<IInspectable>();

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
