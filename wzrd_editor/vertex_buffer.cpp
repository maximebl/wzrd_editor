#include "pch.h"
#include "vertex_buffer.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	vertex_buffer::vertex_buffer(
		wzrd_editor::buffer_type const& type,
		int32_t max_size,
		int32_t resize_increment,
		bool is_auto_resize)
		: m_type(type), m_max_size(max_size), m_resize_increment(resize_increment), m_is_auto_resize(is_auto_resize)
	{
		m_buffer = std::make_unique<upload_buffer<xm_vertex>>(Utilities::device, m_max_size, false, m_is_auto_resize);

		m_view.stride_in_bytes = m_buffer_element_size;
		m_view.size_in_bytes = m_max_size * m_buffer_element_size;
		m_view.buffer_location = m_buffer->get_resource()->GetGPUVirtualAddress();

		m_swap_view.stride_in_bytes = m_buffer_element_size;
	}

	wzrd_editor::view vertex_buffer::get_view()
	{
		return m_is_using_swap_buffer ? m_swap_view : m_view;
	}

	void vertex_buffer::add_to_view(wzrd_editor::vertex2 new_vertex)
	{
		if (m_vertices.size() < m_max_size)
		{
			auto impl_vertex = new_vertex.as<wzrd_editor::implementation::vertex2>();
			m_vertices.push_back(impl_vertex->m_vertex);

			update_current_buffer();
		}
		else
		{
			if (m_type == wzrd_editor::buffer_type::dynamic_buffer && m_is_auto_resize)
			{
				auto impl_vertex = new_vertex.as<wzrd_editor::implementation::vertex2>();
				m_vertices.push_back(impl_vertex->m_vertex);

				m_max_size += m_resize_increment;
				swap_upload_buffer();
				update_current_buffer();
			}
		}
		return;
	}

	void vertex_buffer::clear()
	{
		if (m_is_using_swap_buffer)
		{
			m_swap_buffer->clear_data();
		}
		else
		{
			m_buffer->clear_data();
		}
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
		m_current_size = m_vertices.size();

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
				buffer->copy_data(i, m_vertices[i]);
			}
		}
	}

	void vertex_buffer::swap_upload_buffer()
	{
		auto m_current_size = m_vertices.size();

		if (!m_is_using_swap_buffer)
		{
			m_swap_buffer.reset(
				new upload_buffer<xm_vertex>(Utilities::device, m_max_size, false, true)
			);

			m_swap_view.size_in_bytes = m_current_size * m_buffer_element_size;
			m_swap_view.stride_in_bytes = m_buffer_element_size;
			m_swap_view.buffer_location = m_swap_buffer->get_resource()->GetGPUVirtualAddress();

			//for (size_t i = 0; i < m_current_size; i++)
			//{
			//	m_swap_buffer->copy_data(i, m_vertices[i]);
			//}

			m_is_using_swap_buffer = true;
		}
		else {
			m_buffer.reset(
				new upload_buffer<xm_vertex>(Utilities::device, m_max_size, false, true)
			);

			//m_view.size_in_bytes = m_max_size * m_buffer_element_size;
			//m_view.stride_in_bytes = m_buffer_element_size;
			m_view.buffer_location = m_buffer->get_resource()->GetGPUVirtualAddress();
			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_current_size * m_buffer_element_size;
			//for (size_t i = 0; i < m_current_size; i++)
			//{
			//	m_buffer->copy_data(i, m_vertices[i]);
			//}

			m_is_using_swap_buffer = false;
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

	void vertex_buffer::is_auto_resize(bool value)
	{
		m_is_auto_resize = value;
	}
}
