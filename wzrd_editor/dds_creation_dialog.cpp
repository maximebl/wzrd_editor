#include "pch.h"
#include "dds_creation_dialog.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	dds_creation_dialog::dds_creation_dialog()
	{
		InitializeComponent();
	}

	winrt::Windows::Foundation::IAsyncAction dds_creation_dialog::onclick_create(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const & args)
	{
		co_return;
	}

	void dds_creation_dialog::ClickHandler(IInspectable const&, RoutedEventArgs const&)
	{
		Button().Content(box_value(L"Clicked"));
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
