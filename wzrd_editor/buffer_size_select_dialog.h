#pragma once

#include "buffer_size_select_dialog.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct buffer_size_select_dialog : buffer_size_select_dialogT<buffer_size_select_dialog>
	{
		buffer_size_select_dialog(int32_t current_buffer_size);
		winrt::Windows::Foundation::IAsyncAction onclick_allocate(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const& args);

		bool is_auto_resizeable();
		void is_auto_resizeable(bool value);

        int32_t buffer_size();
        void buffer_size(int32_t value);

        int32_t buffer_increment_size();
        void buffer_increment_size(int32_t value);

		int32_t minimum_buffer_size();

		void PropertyChanged(winrt::event_token const& token) noexcept;
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);

	private:
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
		bool m_is_auto_resizeable = false;
		int32_t m_buffer_size = 0;
		int32_t m_buffer_increment_size = 0;
		int32_t m_minimum_buffer_size = 1;
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct buffer_size_select_dialog : buffer_size_select_dialogT<buffer_size_select_dialog, implementation::buffer_size_select_dialog>
	{
	};
}
