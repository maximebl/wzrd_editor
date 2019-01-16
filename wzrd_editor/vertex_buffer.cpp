#include "pch.h"
#include "vertex_buffer.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation::Collections;

namespace winrt::wzrd_editor::implementation
{
	vertex_buffer::vertex_buffer(
		wzrd_editor::buffer_type const& type,
		IVector<IInspectable> const initial_data,
		int32_t max_size,
		int32_t resize_increment,
		bool is_auto_resize)
		: m_type(type), m_vertices(initial_data), m_max_size(max_size), m_resize_increment(resize_increment), m_is_auto_resize(is_auto_resize)
	{
		m_vertices = initial_data;
		if (type == wzrd_editor::buffer_type::dynamic_buffer)
		{
			m_buffer = std::make_unique<upload_buffer<xm_vertex>>(Utilities::device, m_max_size, false, m_is_auto_resize);

			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_max_size * m_buffer_element_size;
			m_view.buffer_location = m_buffer->get_resource()->GetGPUVirtualAddress();

			m_swap_view.stride_in_bytes = m_buffer_element_size;

			update_current_buffer();
		}

		if (type == wzrd_editor::buffer_type::static_buffer)
		{
			//auto first_elem = m_vertices.GetAt(0).as<wzrd_editor::implementation::vertex2>()->m_vertex;
			//auto second_elem = m_vertices.GetAt(1).as<wzrd_editor::implementation::vertex2>()->m_vertex;

			//Windows::Foundation::Collections::IVector<int> coll{ winrt::single_threaded_vector<int>() };
			//coll.Append(1);
			//coll.Append(2);
			//coll.Append(3);
			//m_verts(m_vertices);
			auto res = m_verts.get_container().data();


			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_max_size * m_buffer_element_size;

			//m_view.buffer_location = Utilities::create_default_buffer(
			//	Utilities::device,
			//	Utilities::cmd_list,
			//	&first_elem,
			//	m_view.size_in_bytes,
			//	static_buffer_uploader
			//)->GetGPUVirtualAddress();
		}
	}

	wzrd_editor::view vertex_buffer::get_view()
	{
		return m_is_using_swap_buffer ? m_swap_view : m_view;
	}

	int32_t vertex_buffer::get_capacity_percentage()
	{
		return (m_current_size * 100) / m_max_size;
	}

	void vertex_buffer::add_to_view(wzrd_editor::vertex2 new_vertex)
	{
		if (!is_buffer_full())
		{
			update_current_buffer();
		}
		else
		{
			if (m_type == wzrd_editor::buffer_type::dynamic_buffer && m_is_auto_resize)
			{
				m_max_size += m_resize_increment;
				swap_upload_buffer();
				update_current_buffer();
			}
		}
		return;
	}

	void vertex_buffer::clear()
	{
		m_vertices.Clear();
		update_current_buffer();
	}

	void vertex_buffer::update_current_buffer()
	{
		if (m_is_using_swap_buffer)
		{
			update(m_swap_buffer, m_swap_view);
		}
		else
		{
			update(m_buffer, m_view);
		}
	}

	void vertex_buffer::update(std::unique_ptr<upload_buffer<xm_vertex>>& buffer, wzrd_editor::view& view)
	{
		m_current_size = m_vertices.Size();

		if (m_current_size == 0)
		{
			view.size_in_bytes = 0;
			buffer->clear_data();
		}
		else
		{
			view.size_in_bytes = m_current_size * m_buffer_element_size;

			for (size_t i = 0; i < m_current_size; ++i)
			{
				buffer->copy_data(i, m_vertices.GetAt(i).as<wzrd_editor::implementation::vertex2>()->m_vertex);
			}
		}
	}

	void vertex_buffer::swap_upload_buffer()
	{
		if (m_is_using_swap_buffer)
		{
			m_buffer.reset(
				new upload_buffer<xm_vertex>(Utilities::device, m_max_size, false, true)
			);

			m_view.buffer_location = m_buffer->get_resource()->GetGPUVirtualAddress();
			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_vertices.Size() * m_buffer_element_size;

			m_is_using_swap_buffer = false;
		}
		else {
			m_swap_buffer.reset(
				new upload_buffer<xm_vertex>(Utilities::device, m_max_size, false, true)
			);

			m_swap_view.buffer_location = m_swap_buffer->get_resource()->GetGPUVirtualAddress();
			m_swap_view.stride_in_bytes = m_buffer_element_size;
			m_swap_view.size_in_bytes = m_vertices.Size() * m_buffer_element_size;

			m_is_using_swap_buffer = true;
		}
		return;
	}

	int32_t vertex_buffer::max_size()
	{
		return m_max_size;
	}

	void vertex_buffer::max_size(int32_t value)
	{
		m_max_size = value;
	}

	int32_t vertex_buffer::current_size()
	{
		return m_current_size;
	}

	void vertex_buffer::current_size(int32_t value)
	{
		m_current_size = value;
	}

	int32_t vertex_buffer::resize_increment()
	{
		return m_resize_increment;
	}

	void vertex_buffer::resize_increment(int32_t value)
	{
		m_resize_increment = value;
	}

	bool vertex_buffer::is_auto_resize()
	{
		return m_is_auto_resize;
	}

	bool vertex_buffer::is_buffer_full()
	{
		return m_vertices.Size() >= m_max_size;
	}
}
