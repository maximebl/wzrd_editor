#include "pch.h"
#include "texture.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using Windows::Foundation::IInspectable; namespace winrt::graphics::implementation
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

	float texture::current_lod()
	{
		return m_current_lod;
	}

	void texture::current_lod(float value)
	{
		update_value(L"current_lod", m_current_lod, value);
	}

	float texture::scale()
	{
		return m_scale;
	}

	void texture::scale(float value)
	{
		update_value(L"scale", m_scale, value);
	}

	Windows::UI::Color texture::sampler_border_color()
	{
		return m_sampler_border_color;
	}

	void texture::sampler_border_color(Windows::UI::Color const & value)
	{
		update_value(L"sampler_border_color", m_sampler_border_color, value);
	}

	float texture::sampler_minLOD()
	{
		return m_sampler_minLOD;
	}

	void texture::sampler_minLOD(float value)
	{
		update_value(L"sampler_minLOD", m_sampler_minLOD, value);
	}

	float texture::sampler_maxLOD()
	{
		return m_sampler_maxLOD;
	}

	void texture::sampler_maxLOD(float value)
	{
		update_value(L"sampler_maxLOD", m_sampler_maxLOD, value);
	}

	float texture::forced_miplevel()
	{
		return m_forced_miplevel;
	}

	void texture::forced_miplevel(float value)
	{
		update_value(L"forced_miplevel", m_forced_miplevel, value);
	}

	bool texture::is_forced_mip_level()
	{
		return m_is_forced_mip_level;
	}

	void texture::is_forced_mip_level(bool value)
	{
		update_value(L"is_forced_mip_level", m_is_forced_mip_level, value);
	}

	bool texture::is_screen_coords()
	{
		return m_is_screen_coords;
	}

	void texture::is_screen_coords(bool value)
	{
		update_value(L"is_screen_coords", m_is_screen_coords, value);
	}

	float texture::sample_comparison_value()
	{
		return m_sample_comparison_value;
	}

	void texture::sample_comparison_value(float value)
	{
		update_value(L"sample_comparison_value", m_sample_comparison_value, value);
	}

	uint32_t texture::max_anisotropy()
	{
		return m_max_anisotropy;
	}

	void texture::max_anisotropy(uint32_t value)
	{
		update_value(L"max_anisotropy", m_max_anisotropy, value);
	}

	int32_t texture::x_pixel_offset()
	{
		return m_x_pixel_offset;
	}

	void texture::x_pixel_offset(int32_t value)
	{
		update_value(L"x_pixel_offset", m_x_pixel_offset, value);
	}

	int32_t texture::y_pixel_offset()
	{
		return m_y_pixel_offset;
	}

	void texture::y_pixel_offset(int32_t value)
	{
		update_value(L"y_pixel_offset", m_y_pixel_offset, value);
	}

	bool texture::is_anisotropic()
	{
		return m_is_anisotropic;
	}

	void texture::is_anisotropic(bool value)
	{
		update_value(L"is_anisotropic", m_is_anisotropic, value);
	}

	graphics::filtering_method texture::minification_filter()
	{
		return m_minification_filter;
	}

	void texture::minification_filter(graphics::filtering_method const& value)
	{
		update_value(L"minification_filter", m_minification_filter, value);
	}

	graphics::filtering_method texture::magnification_filter()
	{
		return m_magnification_filter;
	}

	void texture::magnification_filter(graphics::filtering_method const& value)
	{
		update_value(L"magnification_filter", m_magnification_filter, value);
	}

	graphics::filtering_method texture::miplevel_sampling_filter()
	{
		return m_miplevel_sampling_filter;
	}

	void texture::miplevel_sampling_filter(graphics::filtering_method const& value)
	{
		update_value(L"miplevel_sampling_filter", m_miplevel_sampling_filter, value);
	}

	graphics::filter_reduction_type texture::reduction()
	{
		return m_reduction;
	}

	void texture::reduction(graphics::filter_reduction_type const& value)
	{
		update_value(L"reduction", m_reduction, value);
	}

	graphics::filter_comparison_function texture::comparison_function()
	{
		return m_comparison_function;
	}

	void texture::comparison_function(graphics::filter_comparison_function const& value)
	{
		update_value(L"comparison_function", m_comparison_function, value);
	}

	graphics::sampling_function texture::sampling_function()
	{
		return m_sampling_function;
	}

	void texture::sampling_function(graphics::sampling_function const& value)
	{
		update_value(L"sampling_function", m_sampling_function, value);
	}

	graphics::alpha_mode texture::alpha_mode()
	{
		return m_alpha_mode;
	}

	void texture::alpha_mode(graphics::alpha_mode const& value)
	{
		update_value(L"alpha_mode", m_alpha_mode, value);
	}

	Windows::Foundation::Collections::IObservableVector<IInspectable> texture::mipmaps()
	{
		return m_mipmaps;
	}

	void texture::mipmaps(Windows::Foundation::Collections::IObservableVector<IInspectable> const& value)
	{
		update_value(L"mipmaps", m_mipmaps, value);
	}

	graphics::texture_address_mode texture::u_address_mode()
	{
		return m_u_address_mode;
	}

	void texture::u_address_mode(graphics::texture_address_mode const& value)
	{
		update_value(L"u_address_mode", m_u_address_mode, value);
	}

	graphics::texture_address_mode texture::v_address_mode()
	{
		return m_v_address_mode;
	}

	void texture::v_address_mode(graphics::texture_address_mode const& value)
	{
		update_value(L"v_address_mode", m_v_address_mode, value);
	}

	graphics::texture_address_mode texture::w_address_mode()
	{
		return m_w_address_mode;
	}

	void texture::w_address_mode(graphics::texture_address_mode const& value)
	{
		update_value(L"w_address_mode", m_w_address_mode, value);
	}

	float texture::topleft_u()
	{
		return m_topleft_u;
	}

	void texture::topleft_u(float value)
	{
		update_value(L"topleft_u", m_topleft_u, value);
	}

	float texture::topleft_v()
	{
		return m_topleft_v;
	}

	void texture::topleft_v(float value)
	{
		update_value(L"topleft_v", m_topleft_v, value);
	}

	float texture::topleft_x()
	{
		return m_topleft_x;
	}

	void texture::topleft_x(float value)
	{
		update_value(L"topleft_x", m_topleft_x, value);
	}

	float texture::topleft_y()
	{
		return m_topleft_y;
	}

	void texture::topleft_y(float value)
	{
		update_value(L"topleft_y", m_topleft_y, value);
	}

	float texture::topright_u()
	{
		return m_topright_u;
	}

	void texture::topright_u(float value)
	{
		update_value(L"topright_u", m_topright_u, value);
	}

	float texture::topright_v()
	{
		return m_topright_v;
	}

	void texture::topright_v(float value)
	{
		update_value(L"topright_v", m_topright_v, value);
	}

	float texture::topright_x()
	{
		return m_topright_x;
	}

	void texture::topright_x(float value)
	{
		update_value(L"topright_x", m_topright_x, value);
	}

	float texture::topright_y()
	{
		return m_topright_y;
	}

	void texture::topright_y(float value)
	{
		update_value(L"topright_y", m_topright_y, value);
	}

	float texture::bottomleft_u()
	{
		return m_bottomleft_u;
	}

	void texture::bottomleft_u(float value)
	{
		update_value(L"bottomleft_u", m_bottomleft_u, value);
	}

	float texture::bottomleft_v()
	{
		return m_bottomleft_v;
	}

	void texture::bottomleft_v(float value)
	{
		update_value(L"bottomleft_v", m_bottomleft_v, value);
	}

	float texture::bottomleft_x()
	{
		return m_bottomleft_x;
	}

	void texture::bottomleft_x(float value)
	{
		update_value(L"bottomleft_x", m_bottomleft_x, value);
	}

	float texture::bottomleft_y()
	{
		return m_bottomleft_y;
	}

	void texture::bottomleft_y(float value)
	{
		update_value(L"bottomleft_y", m_bottomleft_y, value);
	}

	float texture::bottomright_u()
	{
		return m_bottomright_u;
	}

	void texture::bottomright_u(float value)
	{
		update_value(L"bottomright_u", m_bottomright_u, value);
	}

	float texture::bottomright_v()
	{
		return m_bottomright_v;
	}

	void texture::bottomright_v(float value)
	{
		update_value(L"bottomright_v", m_bottomright_v, value);
	}

	float texture::bottomright_x()
	{
		return m_bottomright_x;
	}

	void texture::bottomright_x(float value)
	{
		update_value(L"bottomright_x", m_bottomright_x, value);
	}

	float texture::bottomright_y()
	{
		return m_bottomright_y;
	}

	void texture::bottomright_y(float value)
	{
		update_value(L"bottomright_y", m_bottomright_y, value);
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
