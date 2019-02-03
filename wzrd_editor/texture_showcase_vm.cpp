#include "pch.h"
#include "texture_showcase_vm.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> texture_showcase_vm::textures()
	{
		return m_textures;
	}
}
