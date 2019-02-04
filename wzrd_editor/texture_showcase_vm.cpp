#include "pch.h"
#include "texture_showcase_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	IObservableVector<IInspectable> texture_showcase_vm::textures()
	{
		return m_textures;
	}

	IObservableVector<IInspectable> texture_showcase_vm::shaders()
	{
		return m_textures;
	}
}
