//
// Declaration of the buffer class.
//

#pragma once

#include "buffer.g.h"
#include "vertex.h"
#include "upload_buffer.h"

namespace winrt::graphics::implementation
{
	struct buffer : bufferT<buffer>
	{
		buffer() = default;
		buffer(graphics::buffer_type const& type, Windows::Foundation::Collections::IVector<graphics::vertex> const& initial_data, int32_t max_size, int32_t resize_increment, bool is_auto_resize);

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

	private:
		void update(std::unique_ptr<upload_buffer<xm_vertex>>& buffer, graphics::view& view);
		void update_current_buffer();
		void swap_upload_buffer();

		int m_buffer_element_size = sizeof(xm_vertex);
		graphics::view m_view;
		graphics::view m_swap_view;
		graphics::buffer_type m_type;
		int32_t m_max_size = 0;
		int32_t m_current_size = 0;
		int32_t m_resize_increment = 0;
		bool m_is_auto_resize = false;
		bool m_is_using_swap_buffer = false;

		std::unique_ptr<upload_buffer<xm_vertex>> m_buffer = nullptr;
		std::unique_ptr<upload_buffer<xm_vertex>> m_swap_buffer = nullptr;

		std::vector<xm_vertex> m_vertices;

		winrt::com_ptr<ID3D12Resource> static_buffer_uploader = nullptr;

		ID3D12Device* m_device = nullptr;
		ID3D12GraphicsCommandList* m_cmd_list = nullptr;
	};
}

namespace winrt::graphics::factory_implementation
{
	struct buffer : bufferT<buffer, implementation::buffer>
	{
	};
}
