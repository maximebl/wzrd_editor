#pragma once

#include "GeometryViewModel.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct GeometryViewModel : GeometryViewModelT<GeometryViewModel>
    {
        GeometryViewModel();

        wzrd_editor::Geometry Geometry();
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Shaders();

	private:
		wzrd_editor::Geometry m_geometry;
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_shaders = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
    };
}
