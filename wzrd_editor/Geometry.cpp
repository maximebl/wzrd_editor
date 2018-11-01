#include "pch.h"
#include "Geometry.h"
#include "wzrd_vec3.h"

namespace winrt::wzrd_editor::implementation
{
	Geometry::Geometry(wzrd_editor::wzrd_vec3 const& position) : 
		m_position{position}
	{
		m_positions = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
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
