#include "pch.h"
#include "utilities.h"


utilities::utilities()
{
}


utilities::~utilities()
{
}

winrt::com_ptr<ID3D12Resource> utilities::create_static_buffer(
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
			//winrt::guid_of<ID3D12Resource>(),
			__uuidof(ID3D12Resource),
			default_buffer.put_void()));

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			//winrt::guid_of<ID3D12Resource>(),
			__uuidof(ID3D12Resource),
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

winrt::com_ptr<ID3D12Resource> utilities::create_static_texture_resource(
	ID3D12Device* device, 
	ID3D12GraphicsCommandList* cmdList, 
	D3D12_RESOURCE_DESC texture_desc,
	const void* initData, 
	UINT texture_width, 
	UINT texture_height, 
	UINT texture_pixel_size, 
	winrt::com_ptr<ID3D12Resource>& uploadBuffer)
{
	winrt::com_ptr<ID3D12Resource> default_buffer = nullptr;

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texture_desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			//winrt::guid_of<ID3D12Resource>(),
			__uuidof(ID3D12Resource),
			default_buffer.put_void()));

	auto upload_buffer_size = GetRequiredIntermediateSize(default_buffer.get(), 0, 1);

	winrt::check_hresult(
		device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			//winrt::guid_of<ID3D12Resource>(),
			__uuidof(ID3D12Resource),
			uploadBuffer.put_void()));

	D3D12_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pData = initData;
	subresource_data.RowPitch = texture_width * texture_pixel_size;
	subresource_data.SlicePitch = subresource_data.RowPitch * texture_height;

	UpdateSubresources(cmdList, default_buffer.get(), uploadBuffer.get(), 0, 0, 1, &subresource_data);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	return default_buffer;
	// Keep the uploadBuffer reference alive until the command list has performed the copy
}

