#pragma once

#include "dds_creation_page.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct dds_creation_page : dds_creation_pageT<dds_creation_page>
    {
        dds_creation_page();

		uint32_t width();
        void width(uint32_t value);
        uint32_t height();
        void height(uint32_t value);
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> alpha_modes();
        graphics::alpha_mode alpha_mode();
        void alpha_mode(graphics::alpha_mode const& value);
        bool is_texture_array();
        void is_texture_array(bool value);
        hstring texture_name();
        void texture_name(hstring const& value);
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

		Windows::Foundation::IAsyncAction onclick_create_dds(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args);

		void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs e);
		void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs e);

	private:
		
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		graphics::alpha_mode m_alpha_mode = graphics::alpha_mode::unknown;
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_alpha_modes = winrt::single_threaded_observable_vector<IInspectable>();
		bool m_is_texture_array = false;
		hstring m_texture_name = L"";

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;

		void raise_property_changed(hstring const& property_name)
		{
			m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(property_name));
		}

		template <class T>
		void update_value(hstring const& property_name, T & var, T value)
		{
			if (var != value)
			{
				var = value;
				raise_property_changed(property_name);
			}
		}
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct dds_creation_page : dds_creation_pageT<dds_creation_page, implementation::dds_creation_page>
    {
    };
}
