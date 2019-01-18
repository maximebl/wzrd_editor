#pragma once

#include "Geometry.g.h"
#include "utilities.h"
#include "winrt\graphics.h"

namespace winrt::wzrd_editor::implementation
{
	struct Geometry : GeometryT<Geometry>
	{
		Geometry() = delete;
		Geometry(wzrd_editor::wzrd_vec3 const& position);
		
		wzrd_editor::wzrd_vec3 Position();
		int32_t Index();
		void Index(int32_t value);

		int32_t BufferCapacity();
		void BufferCapacity(int32_t value);

		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Vertices();
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Indices();

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:

		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_indices = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_vertices = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
		int32_t m_index = 0;
		int32_t m_buffer_capacity = 0;
		wzrd_editor::wzrd_vec3 m_position;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
	};
}
