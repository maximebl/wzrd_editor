#pragma once

#include "Geometry.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct Geometry : GeometryT<Geometry>
	{
		Geometry() = delete;
		Geometry(wzrd_editor::wzrd_vec3 const& position);

		wzrd_editor::wzrd_vec3 Position();

		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Positions();

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		wzrd_editor::wzrd_vec3 m_position;
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_positions;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
	};
}
