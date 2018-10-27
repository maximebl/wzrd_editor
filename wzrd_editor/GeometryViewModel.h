#pragma once

#include "GeometryViewModel.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct GeometryViewModel : GeometryViewModelT<GeometryViewModel>
    {
        GeometryViewModel();

        wzrd_editor::Geometry Geometry();

	private:
		wzrd_editor::Geometry m_geometry;
    };
}
