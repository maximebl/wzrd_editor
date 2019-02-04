//
// Declaration of the texture_showcase_vm class.
//

#pragma once

#include "texture_showcase_vm.g.h"

namespace winrt::wzrd_editor::implementation
{
	using namespace Windows::Foundation::Collections;
	using namespace Windows::Foundation;

	struct texture_showcase_vm : texture_showcase_vmT<texture_showcase_vm>
	{
		texture_showcase_vm() = default;

		IObservableVector<IInspectable> textures();
		IObservableVector<IInspectable> shaders();

	private:
		IObservableVector<IInspectable> m_textures = winrt::single_threaded_observable_vector<IInspectable>();
		IObservableVector<IInspectable> m_shaders = winrt::single_threaded_observable_vector<IInspectable>();
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct texture_showcase_vm : texture_showcase_vmT<texture_showcase_vm, implementation::texture_showcase_vm>
	{
	};
}
