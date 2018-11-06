#include "pch.h"
#include "GeometryViewModel.h"
#include "Geometry.h"
#include "wzrd_vec3.h"

namespace winrt::wzrd_editor::implementation
{
	GeometryViewModel::GeometryViewModel() : m_geometry{ winrt::make<wzrd_editor::implementation::Geometry>(winrt::make<wzrd_editor::implementation::wzrd_vec3>(0.0f, 0.0f, 0.0f)) }
	{
	}

    wzrd_editor::Geometry GeometryViewModel::Geometry()
    {
		return m_geometry;
    }

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> GeometryViewModel::Shaders()
	{
		return m_shaders;
	}

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> GeometryViewModel::Textures()
	{
		return m_textures;
	}
}
