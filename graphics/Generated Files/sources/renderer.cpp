#include "pch.h"
#include "renderer.h"

namespace winrt::graphics::implementation
{
    void renderer::enable_debug_layer()
    {
        throw hresult_not_implemented();
    }

    void renderer::initialize(Windows::UI::Xaml::Controls::SwapChainPanel const& target_swapchain)
    {
        throw hresult_not_implemented();
    }

    void renderer::clear_shaders()
    {
        throw hresult_not_implemented();
    }

    void renderer::start_render_loop()
    {
        throw hresult_not_implemented();
    }

    void renderer::stop_render_loop()
    {
        throw hresult_not_implemented();
    }

    bool renderer::is_rendering()
    {
        throw hresult_not_implemented();
    }

    graphics::primitive_types renderer::current_topology()
    {
        throw hresult_not_implemented();
    }

    void renderer::current_topology(graphics::primitive_types const& value)
    {
        throw hresult_not_implemented();
    }

    Windows::Foundation::IAsyncOperation<graphics::compilation_result> renderer::pick_and_compile_shader(hstring const& shader_name, hstring const& entry_point, hstring const& version)
    {
        throw hresult_not_implemented();
    }

    graphics::buffer renderer::current_buffer()
    {
        throw hresult_not_implemented();
    }

    void renderer::current_buffer(graphics::buffer const& value)
    {
        throw hresult_not_implemented();
    }
}
