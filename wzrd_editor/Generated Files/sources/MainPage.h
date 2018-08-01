#pragma once

#include "MainPage.g.h"

namespace winrt::wzrd_editor::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::wzrd_editor::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
