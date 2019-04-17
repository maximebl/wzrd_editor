#include "pch.h"
#include "texture_showcase_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	texture_showcase_vm::texture_showcase_vm()
	{
	}

	IObservableVector<IInspectable> texture_showcase_vm::textures()
	{
		return m_textures;
	}

	void texture_showcase_vm::textures(IObservableVector<IInspectable> const& value)
	{
		update_value(L"textures", m_textures, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::shaders()
	{
		return m_shaders;
	}

	void texture_showcase_vm::shaders(IObservableVector<IInspectable> const& value)
	{
		update_value(L"shaders", m_shaders, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::address_modes()
	{
		return m_address_modes;
	}

	void texture_showcase_vm::address_modes(IObservableVector<IInspectable> const & value)
	{
		update_value(L"address_modes", m_address_modes, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::mipmaps()
	{
		return m_mipmaps;
	}

	void texture_showcase_vm::mipmaps(IObservableVector<IInspectable> const& value)
	{
		update_value(L"mipmaps", m_mipmaps, value);
	}

	wzrd_editor::texture_vm texture_showcase_vm::current_texture_vm()
	{
		return m_current_texture_vm;
	}

	void texture_showcase_vm::current_texture_vm(wzrd_editor::texture_vm const& value)
	{
		update_value(L"current_texture", m_current_texture_vm, value);
	}

	graphics::renderer texture_showcase_vm::current_renderer()
	{
		return m_current_renderer;
	}

	void texture_showcase_vm::current_renderer(graphics::renderer const& value)
	{
		update_value(L"current_renderer", m_current_renderer, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::minification_filters()
	{
		return m_minification_filters;
	}

	void texture_showcase_vm::minification_filters(IObservableVector<IInspectable> const& value)
	{
		update_value(L"minification_filters", m_minification_filters, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::magnification_filters()
	{
		return m_magnification_filters;
	}

	void texture_showcase_vm::magnification_filters(IObservableVector<IInspectable> const& value)
	{
		update_value(L"magnification_filters", m_magnification_filters, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::miplevel_sampling_filters()
	{
		return m_miplevel_sampling_filters;
	}

	void texture_showcase_vm::miplevel_sampling_filters(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& value)
	{
		update_value(L"miplevel_sampling_filters", m_miplevel_sampling_filters, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::filter_reduction_types()
	{
		return m_filter_reduction_types;
	}

	void texture_showcase_vm::filter_reduction_types(IObservableVector<IInspectable> const& value)
	{
		update_value(L"filter_reduction_types", m_filter_reduction_types, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::filter_comparisonfuncs()
	{
		return m_filter_comparisonfuncs;
	}

	void texture_showcase_vm::filter_comparisonfuncs(IObservableVector<IInspectable> const& value)
	{
		update_value(L"filter_comparisonfuncs", m_filter_comparisonfuncs, value);
	}

	IObservableVector<IInspectable> texture_showcase_vm::sampling_functions()
	{
		return m_sampling_functions;
	}

	void texture_showcase_vm::sampling_functions(IObservableVector<IInspectable> const& value)
	{
		update_value(L"sampling_functions", m_sampling_functions, value);
	}

	wzrd_editor::dds_creation_vm texture_showcase_vm::dds_creation_vm()
	{
		return m_dds_creation_vm;
	}

	void texture_showcase_vm::dds_creation_vm(wzrd_editor::dds_creation_vm const& value)
	{
		update_value(L"dds_creation_vm", m_dds_creation_vm, value);
	}

	winrt::event_token texture_showcase_vm::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void texture_showcase_vm::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
