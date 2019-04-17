#include "pch.h"
#include "texture_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	graphics::texture texture_vm::current_texture()
	{
		return m_current_texture;
	}

	void texture_vm::current_texture(graphics::texture const& value)
	{
		update_value(L"current_texture", m_current_texture, value);
	}

	bool texture_vm::is_error()
	{
		return m_is_error;
	}

	void texture_vm::is_error(bool value)
	{
		update_value(L"is_error", m_is_error, value);
	}

	bool texture_vm::is_loading()
	{
		return m_is_loading;
	}

	void texture_vm::is_loading(bool value)
	{
		update_value(L"is_loading", m_is_loading, value);
	}

	winrt::event_token texture_vm::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void texture_vm::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
