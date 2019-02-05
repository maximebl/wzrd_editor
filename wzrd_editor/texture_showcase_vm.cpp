#include "pch.h"
#include "texture_showcase_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
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

	winrt::event_token texture_showcase_vm::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void texture_showcase_vm::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
