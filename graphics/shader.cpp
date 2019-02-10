#include "pch.h"
#include "shader.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::graphics::implementation
{
	shader::shader(hstring const& shader_name, graphics::shader_type const& shader_type) : m_shader_name(shader_name), m_shader_type(shader_type)
	{
		switch (shader_type)
		{
		case shader_type::vertex:
			m_type_glyph = L"Play";
			break;
		case shader_type::pixel:
			m_type_glyph = L"SelectAll";
			break;
		default:
			break;
		}
	}

	hstring shader::shader_name()
	{
		return m_shader_name;
	}

	void shader::shader_name(hstring const& value)
	{
		update_value(L"shader_name", m_shader_name, value);
	}

	hstring shader::file_name()
	{
		return m_file_name;
	}

	void shader::file_name(hstring const & value)
	{
		update_value(L"file_name", m_file_name, value);
	}

	hstring shader::type_glyph()
	{
		return m_type_glyph;
	}

	void shader::type_glyph(hstring const& value)
	{
		update_value(L"type_glyph", m_type_glyph, value);
	}

	bool shader::is_loading()
	{
		return m_is_loading;
	}

	void shader::is_loading(bool value)
	{
		update_value(L"is_loading", m_is_loading, value);
	}

	bool shader::is_error()
	{
		return m_is_error;
	}

	void shader::is_error(bool value)
	{
		update_value(L"is_error", m_is_error, value);
	}

	graphics::shader_type shader::shader_type()
	{
		return m_shader_type;
	}

	void shader::shader_type(graphics::shader_type const& value)
	{
		update_value(L"shader_type", m_shader_type, value);
	}

	hstring shader::compiler()
	{
		return m_compiler;
	}

	void shader::compiler(hstring const& value)
	{
		update_value(L"compiler", m_compiler, value);
	}

	uint64_t shader::instruction_count()
	{
		return m_instruction_count;
	}

	void shader::instruction_count(uint64_t value)
	{
		update_value(L"instruction_count", m_instruction_count, value);
	}

	uint64_t shader::version()
	{
		return m_version;
	}

	void shader::version(uint64_t value)
	{
		update_value(L"version", m_version, value);
	}

	uint64_t shader::constant_buffer_count()
	{
		return m_constant_buffer_count;
	}

	void shader::constant_buffer_count(uint64_t value)
	{
		update_value(L"constant_buffer_count", m_constant_buffer_count, value);
	}

	uint64_t shader::bound_resources_count()
	{
		return m_bound_resources_count;
	}

	void shader::bound_resources_count(uint64_t value)
	{
		update_value(L"bound_resources_count", m_bound_resources_count, value);
	}

	uint64_t shader::texture_writes_count()
	{
		return m_texture_writes_count;
	}

	void shader::texture_writes_count(uint64_t value)
	{
		update_value(L"texture_writes_count", m_texture_writes_count, value);
	}

	uint64_t shader::texture_load_instructions_count()
	{
		return m_texture_load_instructions_count;
	}

	void shader::texture_load_instructions_count(uint64_t value)
	{
		update_value(L"texture_load_instructions_count", m_texture_load_instructions_count, value);
	}

	uint64_t shader::texture_normal_instructions_count()
	{
		return m_texture_normal_instructions_count;
	}

	void shader::texture_normal_instructions_count(uint64_t value)
	{
		update_value(L"texture_normal_instructions_count", m_texture_normal_instructions_count, value);
	}

	uint64_t shader::texture_comparison_instructions_count()
	{
		return m_texture_comparison_instructions_count;
	}

	void shader::texture_comparison_instructions_count(uint64_t value)
	{
		update_value(L"texture_comparison_instructions_count", m_texture_comparison_instructions_count, value);
	}

	uint64_t shader::texture_bias_instructions_count()
	{
		return m_texture_bias_instructions_count;
	}

	void shader::texture_bias_instructions_count(uint64_t value)
	{
		update_value(L"texture_bias_instructions_count", m_texture_bias_instructions_count, value);
	}

	void shader::input_parameters_count(uint64_t value)
	{
		update_value(L"input_parameters_count", m_input_parameters_count, value);
	}

	uint64_t shader::input_parameters_count()
	{
		return m_input_parameters_count;
	}

	void shader::output_parameters_count(uint64_t value)
	{
		update_value(L"output_parameters_count", m_output_parameters_count, value);
	}

	uint64_t shader::output_parameters_count()
	{
		return m_output_parameters_count;
	}

	void shader::texture_gradient_instructions_count(uint64_t value)
	{
		update_value(L"texture_gradient_instructions_count", m_texture_gradient_instructions_count, value);
	}

	uint64_t shader::texture_gradient_instructions_count()
	{
		return m_texture_gradient_instructions_count;
	}

	winrt::event_token shader::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void shader::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}

	com_ptr<ID3DBlob> shader::byte_code()
	{
		return m_byte_code;
	}

	void shader::byte_code(com_ptr<ID3DBlob> value)
	{
		update_value(L"byte_code", m_byte_code, value);
	}

}
