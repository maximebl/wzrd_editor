#include "pch.h"
#include "buffer.h"

namespace winrt::graphics::implementation
{
    buffer::buffer(graphics::buffer_type const& type, Windows::Foundation::Collections::IVector<graphics::vertex> const& initial_data, int32_t max_size, int32_t resize_increment, bool is_auto_resize)
    {
        throw hresult_not_implemented();
    }

    graphics::view buffer::get_view()
    {
        throw hresult_not_implemented();
    }

    int32_t buffer::get_capacity_percentage()
    {
        throw hresult_not_implemented();
    }

    void buffer::add_to_view(graphics::vertex const& new_vertex)
    {
        throw hresult_not_implemented();
    }

    void buffer::clear()
    {
        throw hresult_not_implemented();
    }

    int32_t buffer::max_size()
    {
        throw hresult_not_implemented();
    }

    void buffer::max_size(int32_t value)
    {
        throw hresult_not_implemented();
    }

    int32_t buffer::current_size()
    {
        throw hresult_not_implemented();
    }

    void buffer::current_size(int32_t value)
    {
        throw hresult_not_implemented();
    }

    int32_t buffer::resize_increment()
    {
        throw hresult_not_implemented();
    }

    void buffer::resize_increment(int32_t value)
    {
        throw hresult_not_implemented();
    }

    bool buffer::is_auto_resize()
    {
        throw hresult_not_implemented();
    }

    bool buffer::is_buffer_full()
    {
        throw hresult_not_implemented();
    }
}
