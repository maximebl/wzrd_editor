#include "pch.h"
#include "Texture.h"

namespace winrt::wzrd_editor::implementation
{
    hstring Texture::Name()
    {
		return m_name;
    }

    void Texture::Name(hstring const& value)
    {
		m_name = value;
    }

    bool Texture::Loading()
    {
		return m_loading;
    }

    void Texture::Loading(bool value)
    {
		m_loading = value;
		m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Loading" });
    }

    winrt::event_token Texture::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
		return m_propertyChanged.add(handler);
    }

    void Texture::PropertyChanged(winrt::event_token const& token) noexcept
    {
		m_propertyChanged.remove(token);
    }
}
