#pragma once
class utilities
{
public:
	utilities();
	~utilities();
	static winrt::com_ptr<ID3D12Resource> create_default_buffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);
};

