#include "pch.h"
#include "dds_creation_dialog.h"
#include "utilities.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	dds_creation_dialog::dds_creation_dialog()
	{
		InitializeComponent();

		auto alpha_modes_cpy = alpha_modes();
		Utilities::generate_alpha_modes_attributes(alpha_modes_cpy);
	}

	winrt::Windows::Foundation::IAsyncAction dds_creation_dialog::onclick_create(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const & args)
	{
		co_return;
	}

	uint32_t dds_creation_dialog::width()
	{
		return m_width;
	}

	void dds_creation_dialog::width(uint32_t value)
	{
		update_value(L"width", m_width, value);
	}

	uint32_t dds_creation_dialog::height()
	{
		return m_height;
	}

	void dds_creation_dialog::height(uint32_t value)
	{
		update_value(L"height", m_height, value);
	}

	graphics::alpha_mode dds_creation_dialog::alpha_mode()
	{
		return m_alpha_mode;
	}

	void dds_creation_dialog::alpha_mode(graphics::alpha_mode const& value)
	{
		update_value(L"alpha_mode", m_alpha_mode, value);
	}

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> dds_creation_dialog::alpha_modes()
	{
		return m_alpha_modes;
	}

	winrt::event_token dds_creation_dialog::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void dds_creation_dialog::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
