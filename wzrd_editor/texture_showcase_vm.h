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
		texture_showcase_vm();

		IObservableVector<IInspectable> textures();
		void textures(IObservableVector<IInspectable> const& value);
		IObservableVector<IInspectable> shaders();
		void shaders(IObservableVector<IInspectable> const& value);
		IObservableVector<IInspectable> address_modes();
		void address_modes(IObservableVector<IInspectable> const& value);
		graphics::texture current_texture();
		void current_texture(graphics::texture const& value);
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> mipmaps();
		void mipmaps(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& value);
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:
		IObservableVector<IInspectable> m_textures = winrt::single_threaded_observable_vector<IInspectable>();
		IObservableVector<IInspectable> m_shaders = winrt::single_threaded_observable_vector<IInspectable>();
		IObservableVector<IInspectable> m_address_modes = winrt::single_threaded_observable_vector<IInspectable>();
		IObservableVector<IInspectable> m_mipmaps = winrt::single_threaded_observable_vector<IInspectable>();
		graphics::texture m_current_texture;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;

		void raise_property_changed(hstring const& property_name)
		{
			m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(property_name));
		}

		template <class T>
		void update_value(hstring const& property_name, T & var, T value)
		{
			if (var != value)
			{
				var = value;
				raise_property_changed(property_name);
			}
		}
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct texture_showcase_vm : texture_showcase_vmT<texture_showcase_vm, implementation::texture_showcase_vm>
	{
	};
}
