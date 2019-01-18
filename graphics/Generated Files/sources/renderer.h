#pragma once

#include "renderer.g.h"

namespace winrt::graphics::implementation
{
    struct renderer : rendererT<renderer>
    {
        renderer() = default;

        void enable_debug_layer();
        void initialize(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain);
        void start_render_loop();
        void stop_render_loop();
        bool is_rendering();
        graphics::primitive_types current_topology();
        void current_topology(graphics::primitive_types const& value);
        Windows::Foundation::IAsyncOperation<graphics::compilation_result> pick_and_compile_shader(hstring const& shader_name, hstring const& entry_point, hstring const& version);
        graphics::buffer current_buffer();
        void current_buffer(graphics::buffer const& value);
    };
}

namespace winrt::graphics::factory_implementation
{
    struct renderer : rendererT<renderer, implementation::renderer>
    {
    };
}
