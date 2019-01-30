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
    };
}

namespace winrt::graphics::factory_implementation
{
    struct texture : textureT<texture, implementation::texture>
    {
    };
}
