#pragma once

#include "wzrd_vec3.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct wzrd_vec3 : wzrd_vec3T<wzrd_vec3>
    {
        wzrd_vec3() = delete;
		wzrd_vec3(float const& x, float const& y, float const& z);

        float x();
        void x(float value);

		float y();
		void y(float value);

		float z();
		void z(float value);

        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		float m_x;
		float m_y;
		float m_z;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

    };
}
