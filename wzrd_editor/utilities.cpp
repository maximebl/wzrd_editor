#include "pch.h"
#include "utilities.h"

Utilities::Utilities()
{

}

concurrency::task<std::vector<unsigned char>> Utilities::read_shader_file(winrt::Windows::Storage::Streams::IBuffer fileBuffer)
{
	auto dataReader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer);

	return concurrency::create_task([fileBuffer, dataReader] () -> std::vector<unsigned char>
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
			__uuidof(default_buffer),
			default_buffer.put_void()));

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(uploadBuffer),
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

