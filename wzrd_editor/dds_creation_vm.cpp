#include "pch.h"
#include "dds_creation_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	uint32_t dds_creation_vm::width()
	{
		return m_width;
	}

	void dds_creation_vm::width(uint32_t value)
	{
		update_value(L"width", m_width, value);
	}

	uint32_t dds_creation_vm::height()
	{
		return m_height;
	}

	void dds_creation_vm::height(uint32_t value)
	{
		update_value(L"height", m_height, value);
	}

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> dds_creation_vm::alpha_modes()
	{
		return m_alpha_modes;
	}

	graphics::alpha_mode dds_creation_vm::alpha_mode()
	{
		return m_alpha_mode;
	}

	void dds_creation_vm::alpha_mode(graphics::alpha_mode const& value)
	{
		update_value(L"alpha_mode", m_alpha_mode, value);
	}

	bool dds_creation_vm::is_texture_array()
	{
		return m_is_texture_array;
	}

	void dds_creation_vm::is_texture_array(bool value)
	{
		update_value(L"is_texture_array", m_is_texture_array, value);
	}

	hstring dds_creation_vm::texture_name()
	{
		return m_texture_name;
	}

	void dds_creation_vm::texture_name(hstring const& value)
	{
		update_value(L"texture_name", m_texture_name, value);
	}

	winrt::event_token dds_creation_vm::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);

	}

	void dds_creation_vm::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
