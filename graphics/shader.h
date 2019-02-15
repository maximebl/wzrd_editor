//
// Declaration of the shader class.
//

#pragma once

#include "shader.g.h"

namespace winrt::graphics::implementation
{

	using namespace Windows::Foundation::Collections;

	struct shader : shaderT<shader>
	{
		shader() = delete;
		shader(hstring const& shader_name, graphics::shader_type const& shader_type);

		hstring shader_name();
		void shader_name(hstring const& value);
		hstring file_name();
		void file_name(hstring const& value);
		hstring type_glyph();
		void type_glyph(hstring const& value);
		bool is_loading();
		void is_loading(bool value);
		bool is_error();
		void is_error(bool value);
		graphics::shader_type shader_type();
		void shader_type(graphics::shader_type const& value);

		hstring compiler();
		void compiler(hstring const& value);
		uint64_t instruction_count();
		void instruction_count(uint64_t value);
		uint64_t version();
		void version(uint64_t value);
		uint64_t constant_buffer_count();
		void constant_buffer_count(uint64_t value);
		uint64_t bound_resources_count();
		void bound_resources_count(uint64_t value);
		uint64_t texture_writes_count();
		void texture_writes_count(uint64_t value);
		uint64_t texture_load_instructions_count();
		void texture_load_instructions_count(uint64_t value);
		uint64_t texture_normal_instructions_count();
		void texture_normal_instructions_count(uint64_t value);
		uint64_t texture_comparison_instructions_count();
		void texture_comparison_instructions_count(uint64_t value);
		uint64_t texture_bias_instructions_count();
		void texture_bias_instructions_count(uint64_t value);
		uint64_t texture_gradient_instructions_count();
		void texture_gradient_instructions_count(uint64_t value);

		void input_parameters_count(uint64_t value);
		uint64_t input_parameters_count();
		void output_parameters_count(uint64_t value);
		uint64_t output_parameters_count();

		IObservableVector<IInspectable> shader_reflection_data();
		void shader_reflection_data(IObservableVector<IInspectable> const & values);

		IObservableVector<IInspectable> shader_bound_resources();
		void shader_bound_resources(IObservableVector<IInspectable> const & values);

		winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
		void PropertyChanged(winrt::event_token const& token) noexcept;

		// implementation only (not exposed as .winmd metadata)
		com_ptr<ID3DBlob> byte_code();
		void byte_code(com_ptr<ID3DBlob> value);

	private:

		hstring m_shader_name;
		hstring m_file_name;
		hstring m_type_glyph;
		graphics::shader_type m_shader_type;
		bool m_is_loading = false;
		bool m_is_error = false;

		hstring m_compiler;
		uint64_t m_instruction_count;
		uint64_t m_version;
		uint64_t m_constant_buffer_count;
		uint64_t m_bound_resources_count;
		uint64_t m_texture_writes_count;
		uint64_t m_texture_load_instructions_count;
		uint64_t m_texture_normal_instructions_count;
		uint64_t m_texture_comparison_instructions_count;
		uint64_t m_texture_bias_instructions_count;
		uint64_t m_texture_gradient_instructions_count;
		uint64_t m_input_parameters_count;
		uint64_t m_output_parameters_count;

		IObservableVector<IInspectable> m_shader_reflection_data = single_threaded_observable_vector<IInspectable>();
		IObservableVector<IInspectable> m_shader_bound_resources = single_threaded_observable_vector<IInspectable>();

		com_ptr<ID3DBlob> m_byte_code;

		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_property_changed;

		void raise_property_changed(hstring const& property_name)
		{
			m_property_changed(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(property_name));
		}

		template <class T>
		void update_value(hstring const& property_name, T & var, T value)
		{
			if (var != value)
			{
				var = value;
				raise_property_changed(property_name);
			}
		}
	};
}

namespace winrt::graphics::factory_implementation
{
	struct shader : shaderT<shader, implementation::shader>
	{
	};
}
