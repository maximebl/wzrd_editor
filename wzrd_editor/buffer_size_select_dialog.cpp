#include "pch.h"
#include "buffer_size_select_dialog.h"

namespace winrt::wzrd_editor::implementation
{
	buffer_size_select_dialog::buffer_size_select_dialog(int32_t current_buffer_size)
	{
		InitializeComponent();

		if (current_buffer_size > 0)
		{
			m_minimum_buffer_size = current_buffer_size;
		}
	}

	winrt::Windows::Foundation::IAsyncAction buffer_size_select_dialog::onclick_allocate(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const& args)
	{
		co_return;
	}

	bool buffer_size_select_dialog::is_auto_resizeable()
	{
		return m_is_auto_resizeable;
	}

	void buffer_size_select_dialog::is_auto_resizeable(bool value)
	{
		m_is_auto_resizeable = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"is_auto_resizeable" });
	}

	int32_t buffer_size_select_dialog::buffer_size()
	{
		return m_buffer_size;
	}

	int32_t buffer_size_select_dialog::minimum_buffer_size()
	{
		return m_minimum_buffer_size;
	}

	void buffer_size_select_dialog::buffer_size(int32_t value)
	{
		m_buffer_size = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"buffer_size" });
	}

	int32_t buffer_size_select_dialog::buffer_increment_size()
	{
		return m_buffer_increment_size;
	}

	void buffer_size_select_dialog::buffer_increment_size(int32_t value)
	{
		m_buffer_increment_size = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"buffer_increment_size" });
	}

	winrt::event_token buffer_size_select_dialog::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_propertyChanged.add(handler);
	}

	void buffer_size_select_dialog::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_propertyChanged.remove(token);
	}
}
