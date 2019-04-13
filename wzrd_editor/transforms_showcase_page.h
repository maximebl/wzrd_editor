//
// Declaration of the transforms_showcase_page class.
//

#pragma once

#include "transforms_showcase_page.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct transforms_showcase_page : transforms_showcase_pageT<transforms_showcase_page>
    {
        transforms_showcase_page();
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct transforms_showcase_page : transforms_showcase_pageT<transforms_showcase_page, implementation::transforms_showcase_page>
    {
    };
}
