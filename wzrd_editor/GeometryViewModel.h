#pragma once

#include "GeometryViewModel.g.h"
#include "utilities.h"

namespace winrt::wzrd_editor::implementation
{	
    struct GeometryViewModel : GeometryViewModelT<GeometryViewModel>
    {
        GeometryViewModel();

        wzrd_editor::Geometry Geometry();
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Shaders();
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Textures();

	private:
		wzrd_editor::Geometry m_geometry;
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_shaders = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_textures = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();

    };
}
