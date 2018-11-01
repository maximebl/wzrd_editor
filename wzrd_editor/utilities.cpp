#include "pch.h"
#include "utilities.h"

Utilities::Utilities()
{
}

UINT Utilities::constant_buffer_byte_size(UINT byte_size)
{
	return (byte_size + 255) & ~255;
}

concurrency::task<std::vector<unsigned char>> Utilities::read_shader_file(winrt::Windows::Storage::Streams::IBuffer fileBuffer)
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

winrt::com_ptr<ID3DBlob> Utilities::compile_shader(const std::string& shaderType, const std::vector<unsigned char>& file_bytes, const std::string& entryPoint)
{
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	winrt::com_ptr<ID3DBlob> byteCode = nullptr;
	winrt::com_ptr<ID3DBlob> errors = nullptr;

	D3DCompile(
		&file_bytes.front(),
		file_bytes.size(),
		nullptr,
		nullptr,
		//TODO: Replace this macro with custom ID3DInclude
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
		auto errorBufferPtr = errors.get()->GetBufferPointer();
		LPCSTR errorMessagePtr = (const char*)errorBufferPtr;
		std::wstring title = L"Shader compilation error.";

		//TODO: This is kind of ghetto string conversion, need to research this later if it's slow
		std::string errorMessage(errorMessagePtr);
		std::wstring message(errorMessage.begin(), errorMessage.end());

		OutputDebugStringA(errorMessagePtr);
		auto dialog = winrt::Windows::UI::Popups::MessageDialog(message, title);
		//co_await dialog.ShowAsync();
	}

	return byteCode;
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
			default_buffer.put_void()));

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			winrt::guid_of<ID3D12Resource>(),
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

winrt::Windows::Foundation::IAsyncAction Utilities::pick_file_buffer(winrt::hstring file_extension, pick_modes pick_mode)
{
	winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
	filePicker.FileTypeFilter().Append(file_extension);
	auto file = co_await filePicker.PickSingleFileAsync();
	co_return;
	//return winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
	//switch (pick_mode)
	//{
	//case pick_modes::single_file_async:
	//	auto file = co_await filePicker.PickSingleFileAsync();
	//	//if (file == nullptr)
	//	//{
	//	//	return;
	//	//}
	//	return winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
	//}
}