//
// Declaration of the vertex_buffer class.
//

#pragma once

#include "vertex_buffer.g.h"
#include "vertex2.h"
#include "utilities.h"

namespace winrt::wzrd_editor::implementation
{
	using namespace winrt::Windows::Foundation::Collections;
	using namespace winrt::Windows::Foundation;

	struct MyVector :
		implements<MyVector, IVector<IInspectable>, IVectorView<IInspectable>, IIterable<IInspectable>>,
		winrt::vector_base<MyVector, IInspectable>
	{
		auto& get_container() const noexcept
		{
			return m_values;
		}

		auto& get_container() noexcept
		{
			return m_values;
		}

	private:
		std::vector<IInspectable> m_values{};
	};

	struct vertex_buffer : vertex_bufferT<vertex_buffer>
	{
		vertex_buffer() = default;
		vertex_buffer(wzrd_editor::buffer_type const& type, IVector<IInspectable> const initial_data, int32_t max_size, int32_t resize_increment, bool is_auto_resize);

		wzrd_editor::view get_view();
		int32_t get_capacity_percentage();
		void add_to_view(wzrd_editor::vertex2 new_vertex);
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
		void update(std::unique_ptr<upload_buffer<xm_vertex>>& buffer, wzrd_editor::view& view);
		void update_current_buffer();
		void swap_upload_buffer();

		int m_buffer_element_size = sizeof(xm_vertex);
		wzrd_editor::view m_view;
		wzrd_editor::view m_swap_view;
		wzrd_editor::buffer_type m_type;
		int32_t m_max_size = 0;
		int32_t m_current_size = 0;
		int32_t m_resize_increment = 0;
		bool m_is_auto_resize = false;
		bool m_is_using_swap_buffer = false;

		std::unique_ptr<upload_buffer<xm_vertex>> m_buffer = nullptr;
		std::unique_ptr<upload_buffer<xm_vertex>> m_swap_buffer = nullptr;

		IVector<IInspectable> m_vertices;
		MyVector m_verts;
		std::vector<xm_vertex> m_xm_vertices;

		winrt::com_ptr<ID3D12Resource> static_buffer_uploader = nullptr;
	};
}

namespace winrt::wzrd_editor::factory_implementation
{
	struct vertex_buffer : vertex_bufferT<vertex_buffer, implementation::vertex_buffer>
	{
	};
}
