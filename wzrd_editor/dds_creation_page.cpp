#include "pch.h"
#include "dds_creation_page.h"
#include "utilities.h"
#include "texture_showcase_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
    dds_creation_page::dds_creation_page()
    {
        InitializeComponent();

		auto alpha_modes_cpy = alpha_modes();
		Utilities::generate_alpha_modes_attributes(alpha_modes_cpy);
    }

	uint32_t dds_creation_page::width()
    {
		return m_width;
    }

    void dds_creation_page::width(uint32_t value)
    {
		update_value(L"width", m_width, value);
    }

    uint32_t dds_creation_page::height()
    {
		return m_height;
    }

    void dds_creation_page::height(uint32_t value)
    {
		update_value(L"height", m_height, value);
    }

    Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> dds_creation_page::alpha_modes()
    {
		return m_alpha_modes;
    }

    graphics::alpha_mode dds_creation_page::alpha_mode()
    {
		return m_alpha_mode;
    }

    void dds_creation_page::alpha_mode(graphics::alpha_mode const& value)
    {
		update_value(L"alpha_mode", m_alpha_mode, value);
    }

    bool dds_creation_page::is_texture_array()
    {
		return m_is_texture_array;
    }

    void dds_creation_page::is_texture_array(bool value)
    {
		update_value(L"is_texture_array", m_is_texture_array, value);
    }

    hstring dds_creation_page::texture_name()
    {
		return m_texture_name;
    }

    void dds_creation_page::texture_name(hstring const& value)
    {
		update_value(L"texture_name", m_texture_name, value);
    }

	winrt::event_token dds_creation_page::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
		return m_property_changed.add(handler);
    }

    void dds_creation_page::PropertyChanged(winrt::event_token const& token) noexcept
    {
		m_property_changed.remove(token);
    }

	IAsyncAction dds_creation_page::onclick_create_dds(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::RoutedEventArgs const & args)
	{
		co_return;
	}

	void dds_creation_page::OnNavigatedTo(Navigation::NavigationEventArgs e)
	{
		using namespace Windows::UI::Xaml::Media::Animation;

		auto anim = ConnectedAnimationService::GetForCurrentView().GetAnimation(L"forward");

		if (anim)
		{
			anim.TryStart(dds_creation_form());
		}
	}

	void dds_creation_page::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs e)
	{
		Windows::UI::Xaml::Media::Animation::ConnectedAnimationService::GetForCurrentView().PrepareToAnimate(L"backward", dds_creation_form());
	}
}
