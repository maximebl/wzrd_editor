#pragma once

#include "buffer.g.h"

namespace winrt::graphics::implementation
{
    struct buffer : bufferT<buffer>
    {
        buffer() = default;
        buffer(graphics::buffer_type const& type, Windows::Foundation::Collections::IObservableVector<graphics::vertex> const& initial_data, int32_t max_size, int32_t resize_increment, bool is_auto_resize);

        graphics::view get_view();
        int32_t get_capacity_percentage();
        void add_to_view(graphics::vertex const& new_vertex);
        void clear();
        int32_t max_size();
        void max_size(int32_t value);
        int32_t current_size();
        void current_size(int32_t value);
        int32_t resize_increment();
        void resize_increment(int32_t value);
        bool is_auto_resize();
        bool is_buffer_full();
    };
}

namespace winrt::graphics::factory_implementation
{
    struct buffer : bufferT<buffer, implementation::buffer>
    {
    };
}
