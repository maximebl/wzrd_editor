#include "pch.h"
#include "Geometry.h"
#include "wzrd_vec3.h"

namespace winrt::wzrd_editor::implementation
{
	Geometry::Geometry(wzrd_editor::wzrd_vec3 const& position) :
		m_position{ position }
	{
	}

	wzrd_editor::wzrd_vec3 Geometry::Position()
	{
		return m_position;
	}

	int32_t Geometry::Index()
	{
		return m_index;
	}

	void Geometry::Index(int32_t value)
	{
		m_index = value;
	}

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Geometry::Vertices()
	{
		return m_vertices;
	}

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Geometry::Indices()
	{
		return m_indices;
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
