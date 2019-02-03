//
// Declaration of the texture_showcase_vm class.
//

#pragma once

#include "texture_showcase_vm.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct texture_showcase_vm : texture_showcase_vmT<texture_showcase_vm>
    {
        texture_showcase_vm() = default;

		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> textures();

	private:
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_textures = winrt::single_threaded_observable_vector<Windows::Foundation::IInspectable>();

    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct texture_showcase_vm : texture_showcase_vmT<texture_showcase_vm, implementation::texture_showcase_vm>
    {
    };
}
