//
// Declaration of the texture class.
//

#pragma once

#include "texture.g.h"

namespace winrt::graphics::implementation
{
    struct texture : textureT<texture>
    {
        texture() = default;

		hstring texture_name();
		void texture_name(hstring const& value);
		bool is_loading();
		void is_loading(bool value);
		bool is_error();
		void is_error(bool value);
    };
}

namespace winrt::graphics::factory_implementation
{
    struct texture : textureT<texture, implementation::texture>
    {
    };
}
