#pragma once
#include <string>
#include <d3d12.h>
#include "winrt/Windows.Foundation.h"
#include <DirectXCollision.h>
#include "MathHelper.h"

const int global_num_frame_resources = 3;

struct Texture
{
	std::string Name;
	std::wstring Filename;
	winrt::com_ptr<ID3D12Resource> Resource = nullptr;
	winrt::com_ptr<ID3D12Resource> UploadHeap = nullptr;
};

struct Material
{
	std::string name;
	int mat_cb_index = -1;
	int diffuse_srv_heap_index = -1;
	int normal_srv_heap_index = -1;
	int num_frames_dirty = global_num_frame_resources;
	DirectX::XMFLOAT4 diffuse_albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 fresnel_r0 = { 0.01f, 0.01f, 0.01f };
	float roughness = 0.25f;
	DirectX::XMFLOAT4X4 mat_transform = MathHelper::Identity4x4();
};

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	std::string Name;

	winrt::com_ptr<ID3DBlob> VertexBufferCPU = nullptr;
	winrt::com_ptr<ID3DBlob> IndexBufferCPU = nullptr;

	winrt::com_ptr<ID3D12Resource> VertexBufferGPU = nullptr;
	winrt::com_ptr<ID3D12Resource> IndexBufferGPU = nullptr;

	winrt::com_ptr<ID3D12Resource> VertexBufferUploader = nullptr;
	winrt::com_ptr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	UINT IndexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;

	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;
		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

class Utilities
{
public:
	Utilities();
	static concurrency::task<std::vector<unsigned char>> read_shader_file(winrt::Windows::Storage::Streams::IBuffer fileBuffer);
	static winrt::com_ptr<ID3DBlob> compile_shader(const std::string& shaderType, const std::vector<unsigned char>& file_bytes, const std::string& entryPoint);
	static winrt::com_ptr<ID3D12Resource> create_default_buffer(
		ID3D12Device* device, 
		ID3D12GraphicsCommandList* cmdList, 
		const void* initData, 
		UINT64 byteSize, 
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);
};
