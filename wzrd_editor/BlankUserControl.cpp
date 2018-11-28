#include "pch.h"
#include "BlankUserControl.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	BlankUserControl::BlankUserControl()
	{
		InitializeComponent();
	}

	winrt::Windows::UI::Xaml::Controls::ContentDialog BlankUserControl::get_dialog()
	{
		return dialog();
	}

	void BlankUserControl::MyProperty(int32_t /* value */)
	{
		throw hresult_not_implemented();
	}

	void BlankUserControl::ClickHandler(IInspectable const&, RoutedEventArgs const&)
	{
		Button().Content(box_value(L"Clicked"));

	}
}
