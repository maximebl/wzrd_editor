#include "pch.h"
#include "utilities.h"

using winrt::Windows::Foundation::Collections::IObservableVector;
using winrt::Windows::Foundation::IInspectable;

Utilities::Utilities()
{
}

ID3D12Device* Utilities::device = nullptr;
ID3D12GraphicsCommandList* Utilities::cmd_list = nullptr;

UINT Utilities::constant_buffer_byte_size(UINT byte_size)
{
	return (byte_size + 255) & ~255;
}

concurrency::task<std::vector<unsigned char>> Utilities::read_file_bytes(winrt::Windows::Storage::Streams::IBuffer fileBuffer)
{
	auto dataReader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer);

	return concurrency::create_task([fileBuffer, dataReader]() -> std::vector<unsigned char>
	{
		std::vector<unsigned char> file_bytes;
		int fileSize = fileBuffer.Length();
		file_bytes.assign(fileSize, 0);
		dataReader.ReadBytes(file_bytes);
		return file_bytes;
	});
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> Utilities::pick_file_buffer(winrt::hstring file_extension, pick_modes pick_mode)
{
	winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
	filePicker.FileTypeFilter().Append(file_extension);

	switch (pick_mode)
	{
	case pick_modes::single_file_async:
	{
		auto file = co_await filePicker.PickSingleFileAsync();
		if (file != nullptr)
		{
			return co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
		}
	}
	break;

	case pick_modes::multiple_files_async:
	{
		//TODO: add support for selecting multiple files
		throw winrt::hresult_not_implemented();
	}
	break;
	}
}

concurrency::task<std::vector<unsigned char>> Utilities::pick_file(winrt::hstring file_extension)
{
	auto file_buffer = co_await pick_file_buffer(file_extension, pick_modes::single_file_async);
	return co_await read_file_bytes(file_buffer);
}

Utilities::compilation_result Utilities::compile_shader(const std::string shaderType, const std::vector<unsigned char>& file_bytes, const std::string entryPoint)
{
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	winrt::com_ptr<ID3DBlob> byteCode = nullptr;
	winrt::com_ptr<ID3DBlob> errors = nullptr;
	compilation_result result;

	D3DCompile(
		&file_bytes.front(),
		file_bytes.size(),
		nullptr,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(),
		shaderType.c_str(),
		compileFlags,
		0,
		byteCode.put(),
		errors.put()
	);

	if (errors != nullptr)
	{
		result.is_success = false;
		result.result_blob = errors;
		return result;
	}
	result.is_success = true;
	result.result_blob = byteCode;
	return result;
}

winrt::com_ptr<ID3D12Resource> Utilities::create_default_buffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData,
	UINT64 byteSize,
	winrt::com_ptr<ID3D12Resource>& uploadBuffer)
{
	winrt::com_ptr<ID3D12Resource> default_buffer = nullptr;

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			winrt::guid_of<ID3D12Resource>(),
			//__uuidof(ID3D12Resource),
			default_buffer.put_void()));

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			winrt::guid_of<ID3D12Resource>(),
			//__uuidof(ID3D12Resource),
			uploadBuffer.put_void()));

	D3D12_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pData = initData;
	subresource_data.RowPitch = byteSize;
	subresource_data.SlicePitch = subresource_data.RowPitch;

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(cmdList, default_buffer.get(), uploadBuffer.get(), 0, 0, 1, &subresource_data);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	return default_buffer;
	// Keep the uploadBuffer reference alive until the command list has performed the copy
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Utilities::get_static_samplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC point_wrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC point_clamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linear_wrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linear_clamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropic_wrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropic_clamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		point_wrap, point_clamp,
		linear_wrap, linear_clamp,
		anisotropic_wrap, anisotropic_clamp };
}

void Utilities::print_coordinates(float x, float y)
{
	std::wstringstream wstringstream;
	wstringstream << L"\n" << L"(" << x << L", " << y << L")" << L"\n" << std::endl;
	OutputDebugStringW(wstringstream.str().c_str());
}

void Utilities::wait(DWORD duration)
{
	HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
	WaitForSingleObject(event_handle, duration);
	CloseHandle(event_handle);
}

auto set_values = [](winrt::hstring name, winrt::hstring value, winrt::hstring description, IObservableVector<IInspectable>& attributes_vector) {

	winrt::graphics::generic_attribute new_attribute;
	new_attribute.attribute_name(name);
	new_attribute.attribute_description(description);
	new_attribute.attribute_value(value);
	attributes_vector.Append(new_attribute);
};

void Utilities::generate_filtering_methods_attributes(IObservableVector<IInspectable> & filtering_methods)
{
	winrt::hstring description;
	winrt::hstring value;
	winrt::hstring name;

	{
		name = L"Point sampling";
		description = L"";
		value = winrt::to_hstring(D3D12_FILTER_TYPE::D3D12_FILTER_TYPE_POINT);
		set_values(name, value, description, filtering_methods);
	}

	{
		name = L"Linear interpolation";
		description = L"";
		value = winrt::to_hstring(D3D12_FILTER_TYPE::D3D12_FILTER_TYPE_LINEAR);
		set_values(name, value, description, filtering_methods);
	}
}

void Utilities::generate_filter_reduction_types_attributes(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable>& filter_reduction_types)
{
	winrt::hstring description;
	winrt::hstring value;
	winrt::hstring name;

	{
		name = L"Standard";
		description = L"";
		value = winrt::to_hstring(D3D12_FILTER_REDUCTION_TYPE::D3D12_FILTER_REDUCTION_TYPE_STANDARD);
		set_values(name, value, description, filter_reduction_types);
	}

	{
		name = L"Comparison";
		description = L"";
		value = winrt::to_hstring(D3D12_FILTER_REDUCTION_TYPE::D3D12_FILTER_REDUCTION_TYPE_COMPARISON);
		set_values(name, value, description, filter_reduction_types);
	}

	{
		name = L"Minimum";
		description = L"";
		value = winrt::to_hstring(D3D12_FILTER_REDUCTION_TYPE::D3D12_FILTER_REDUCTION_TYPE_MINIMUM);
		set_values(name, value, description, filter_reduction_types);
	}

	{
		name = L"Maximum";
		description = L"";
		value = winrt::to_hstring(D3D12_FILTER_REDUCTION_TYPE::D3D12_FILTER_REDUCTION_TYPE_MAXIMUM);
		set_values(name, value, description, filter_reduction_types);
	}
}

void Utilities::generate_filter_comparisonfuncs_attributes(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable>& comparison_funcs)
{
	winrt::hstring description;
	winrt::hstring value;
	winrt::hstring name;

	{
		name = L"Never";
		description = L"Never pass the comparison.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Less";
		description = L"If the source data is less than the destination data, the comparison passes.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Equal";
		description = L"If the source data is equal to the destination data, the comparison passes.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Less or equal";
		description = L"If the source data is less than or equal to the destination data, the comparison passes.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Greater";
		description = L"If the source data is greater than the destination data, the comparison passes.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Not equal";
		description = L"If the source data is not equal to the destination data, the comparison passes.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Greater or equal";
		description = L"If the source data is greater than or equal to the destination data, the comparison passes.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL);
		set_values(name, value, description, comparison_funcs);
	}

	{
		name = L"Always";
		description = L"Always pass the comparison.";
		value = winrt::to_hstring(D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS);
		set_values(name, value, description, comparison_funcs);
	}
}

void Utilities::generate_sampling_functions_attributes(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable>& sampling_functions)
{
	winrt::hstring description;
	winrt::hstring value;
	winrt::hstring name;

	{
		name = L"Sample";
		description = L"Samples a texture using the provided SamplerSate.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::sample));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Load";
		description = L"Reads texture data and returns it's color directly without applying any filtering. We showcase how to use this function using the screen xy coordinates.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::load));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Gather red";
		description = L"Returns the red channel of the texture.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::gather_red));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Gather green";
		description = L"Returns the green channel of the texture.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::gather_green));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Gather blue";
		description = L"Returns the blue channel of the texture.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::gather_blue));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Gather alpha";
		description = L"Returns the alpha channel of the texture.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::gather_alpha));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Sample grad";
		description = L"Samples a texture using a gradient to influence the way the sample location is calculated.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::sample_grad));
		set_values(name, value, description, sampling_functions);
	}

	{
		name = L"Sample bias";
		description = L"Samples a texture after applying a bias to the mipmap level.";
		value = winrt::to_hstring(static_cast<uint32_t>(winrt::graphics::sampling_function::sample_bias));
		set_values(name, value, description, sampling_functions);
	}
}

void Utilities::generate_address_modes_attributes(IObservableVector<IInspectable> & address_modes)
{
	winrt::hstring description;
	winrt::hstring value;
	winrt::hstring name;

	{
		name = L"Wrap";
		description = L"Tile the texture at every (u,v) integer junction. For example, for u values between -1 and 3, the texture is repeated three times.";
		value = winrt::to_hstring(D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		set_values(name, value, description, address_modes);
	}

	{
		name = L"Mirror";
		description = L"Flip the texture at every (u,v) integer junction. For u values between -1 and 1, for example, the texture is addressed normally; between 1 and 2, the texture is flipped (mirrored); between 2 and 3, the texture is normal again; and so on.";
		value = winrt::to_hstring(D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
		set_values(name, value, description, address_modes);
	}

	{
		name = L"Clamp";
		description = L"Texture coordinates outside the range [-1.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively.";
		value = winrt::to_hstring(D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		set_values(name, value, description, address_modes);
	}

	{
		name = L"Border";
		description = L"Texture coordinates outside the range [-1.0, 1.0] are set to the border color specified in D3D12_SAMPLER_DESC or HLSL code.";
		value = winrt::to_hstring(D3D12_TEXTURE_ADDRESS_MODE_BORDER);
		set_values(name, value, description, address_modes);
	}

	{
		name = L"Mirror once";
		description = L"Similar to D3D12_TEXTURE_ADDRESS_MODE_MIRROR and D3D12_TEXTURE_ADDRESS_MODE_CLAMP. Takes the absolute value of the texture coordinate (thus, mirroring around -1), and then clamps to the maximum value.";
		value = winrt::to_hstring(D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
		set_values(name, value, description, address_modes);
	}
}