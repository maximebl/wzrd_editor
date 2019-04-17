#pragma once

#include "texture_vm.g.h"

namespace winrt::wzrd_editor::implementation
{
	struct texture_vm : texture_vmT<texture_vm>
	{
		texture_vm() = default;
		graphics::texture current_texture();
		void current_texture(graphics::texture const& value);
		bool is_error();
		void is_error(bool value);
		bool is_loading();
		void is_loading(bool value);
		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

	private:

		graphics::texture m_current_texture;
		bool m_is_error = false;
		bool m_is_loading = false;

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
	struct texture_vm : texture_vmT<texture_vm, implementation::texture_vm>
	{
	};
}
