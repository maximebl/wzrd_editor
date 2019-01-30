#pragma once
class utilities
{
public:
	utilities();
	~utilities();
	static winrt::com_ptr<ID3D12Resource> create_static_buffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);

	static winrt::com_ptr<ID3D12Resource> create_static_texture(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		D3D12_RESOURCE_DESC texture_desc,
		const void* initData,
		UINT texture_width,
		UINT texture_height,
		UINT texture_pixel_size,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);
};

