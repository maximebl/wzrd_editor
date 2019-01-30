#include "pch.h"
#include "buffer.h"
#include "renderer.h"
#include "utilities.h"

namespace winrt::graphics::implementation
{
	buffer::buffer(
		graphics::buffer_type const& type,
		Windows::Foundation::Collections::IObservableVector<graphics::vertex> const& initial_data,
		int32_t max_size,
		int32_t resize_increment,
		bool is_auto_resize)
		: m_type(type), m_max_size(max_size), m_resize_increment(resize_increment), m_is_auto_resize(is_auto_resize)
	{
		for (auto vertex : initial_data)
		{
			m_vertices.push_back(vertex.as<graphics::implementation::vertex>()->m_vertex);
		}
		auto* test = m_vertices.data();

		if (type == graphics::buffer_type::dynamic_buffer)
		{
			m_buffer = std::make_unique<upload_buffer<xm_vertex>>(renderer::g_device, m_max_size, false, m_is_auto_resize);

			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_max_size * m_buffer_element_size;
			m_view.buffer_location = m_buffer->get_resource()->GetGPUVirtualAddress();

			m_swap_view.stride_in_bytes = m_buffer_element_size;

			update_current_buffer();
		}

		if (type == graphics::buffer_type::static_buffer)
		{
			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_max_size * m_buffer_element_size;
			m_current_size = m_vertices.size();

			m_static_default_buffer = utilities::create_static_buffer(
				renderer::g_device,
				renderer::g_cmd_list,
				m_vertices.data(),
				m_view.size_in_bytes,
				m_static_buffer_uploader);

			m_view.buffer_location = m_static_default_buffer->GetGPUVirtualAddress();
		}
	}

	graphics::view buffer::get_view()
	{
		return m_is_using_swap_buffer ? m_swap_view : m_view;
	}

	int32_t buffer::get_capacity_percentage()
	{
		return (m_current_size * 100) / m_max_size;
	}

	void buffer::add_to_view(graphics::vertex const& new_vertex)
	{

		if (m_type == graphics::buffer_type::dynamic_buffer && !is_buffer_full())
		{
			m_vertices.push_back(new_vertex.as<graphics::implementation::vertex>()->m_vertex);
			update_current_buffer();
		}
		else if (m_type == graphics::buffer_type::dynamic_buffer && m_is_auto_resize)
		{
			m_max_size += m_resize_increment;
			m_vertices.push_back(new_vertex.as<graphics::implementation::vertex>()->m_vertex);
			swap_upload_buffer();
			update_current_buffer();
		}
		return;
	}

	void buffer::clear()
	{
		m_vertices.clear();
		update_current_buffer();
	}

	void buffer::update_current_buffer()
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

	void buffer::update(std::unique_ptr<upload_buffer<xm_vertex>>& buffer, graphics::view& view)
	{
		m_current_size = m_vertices.size();

		if (m_current_size == 0)
		{
			view.size_in_bytes = 0;
			//buffer->clear_data();
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

	void buffer::swap_upload_buffer()
	{
		if (m_is_using_swap_buffer)
		{
			m_buffer.reset(
				new upload_buffer<xm_vertex>(renderer::g_device, m_max_size, false, true)
			);

			m_view.buffer_location = m_buffer->get_resource()->GetGPUVirtualAddress();
			m_view.stride_in_bytes = m_buffer_element_size;
			m_view.size_in_bytes = m_vertices.size() * m_buffer_element_size;

			m_is_using_swap_buffer = false;
		}
		else {
			m_swap_buffer.reset(
				new upload_buffer<xm_vertex>(renderer::g_device, m_max_size, false, true)
			);

			m_swap_view.buffer_location = m_swap_buffer->get_resource()->GetGPUVirtualAddress();
			m_swap_view.stride_in_bytes = m_buffer_element_size;
			m_swap_view.size_in_bytes = m_vertices.size() * m_buffer_element_size;

			m_is_using_swap_buffer = true;
		}
		return;
	}

	int32_t buffer::max_size()
	{
		return m_max_size;
	}

	void buffer::max_size(int32_t value)
	{
		m_max_size = value;
	}

	int32_t buffer::current_size()
	{
		return m_current_size;
	}

	void buffer::current_size(int32_t value)
	{
		m_current_size = value;
	}

	int32_t buffer::resize_increment()
	{
		return m_resize_increment;
	}

	void buffer::resize_increment(int32_t value)
	{
		m_resize_increment = value;
	}

	bool buffer::is_auto_resize()
	{
		return m_is_auto_resize;
	}

	bool buffer::is_buffer_full()
	{
		return m_vertices.size() >= m_max_size;
	}
}
