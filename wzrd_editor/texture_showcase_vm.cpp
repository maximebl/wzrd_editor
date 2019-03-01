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

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> texture_showcase_vm::mipmaps()
	{
		return m_mipmaps;
	}

	void texture_showcase_vm::mipmaps(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& value)
	{
		update_value(L"mipmaps", m_mipmaps, value);
	}

	graphics::texture texture_showcase_vm::current_texture()
	{
		return m_current_texture;
	}

	void texture_showcase_vm::current_texture(graphics::texture const& value)
	{
		update_value(L"current_texture", m_current_texture, value);
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
