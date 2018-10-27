#include "pch.h"
#include "Geometry.h"
#include "wzrd_vec3.h"

namespace winrt::wzrd_editor::implementation
{
	Geometry::Geometry(winrt::hstring const& title, winrt::hstring const& other_title, wzrd_editor::wzrd_vec3 const& position) : 
		m_title{title}, 
		m_other_title{other_title}, 
		m_position{position}
	{
		m_positions = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
	}

    winrt::hstring Geometry::Title()
    {
		return m_title;
    }

    void Geometry::Title(hstring const& value)
    {
		if (m_title != value)
		{
			m_title = value;
			m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{L"Title"});
		}
    }

	winrt::hstring Geometry::OtherTitle()
	{
		return m_other_title;
	}

	void Geometry::OtherTitle(hstring const& value)
	{
		if (m_other_title != value)
		{
			m_other_title = value;
			m_propertyChanged(*this, winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"OtherTitle" });
		}
	}

	wzrd_editor::wzrd_vec3 Geometry::Position()
	{
		return m_position;
	}
	
	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Geometry::Positions()
	{
		return m_positions;
	}

    winrt::event_token Geometry::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
		return m_propertyChanged.add(handler);
    }

    void Geometry::PropertyChanged(winrt::event_token const& token) noexcept
    {
		m_propertyChanged.remove(token);
    }
}
