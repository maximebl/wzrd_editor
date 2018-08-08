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

template <typename T>
class upload_buffer
{
public:
	upload_buffer(ID3D12Device* device, UINT element_count, bool is_constant_buffer) : m_is_constant_buffer(is_constant_buffer)
	{
		m_element_byte_size = sizeof(T);

		if (is_constant_buffer)
		{
			m_element_byte_size = Utilities::constant_buffer_byte_size(sizeof(T));
		}
		
		winrt::check_hresult(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_element_byte_size*element_count),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				__uuidof(m_upload_buffer),
				m_upload_buffer.put_void()
			));

		winrt::check_hresult(
			m_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_data))
		);
	}

	~upload_buffer()
	{
		if (m_upload_buffer != nullptr)
		{
			m_upload_buffer->Unmap(0, nullptr);
		}
		m_mapped_data = nullptr;
	}

	upload_buffer(const upload_buffer& rhs) = delete;
	upload_buffer& operator=(const upload_buffer* rhs) = delete;

	ID3D12Resource* get_resource() const
	{
		return m_upload_buffer.get();
	}

	void copy_data(int element_index, const T& data)
	{
		std::memcpy(&m_mapped_data[element_index * m_element_byte_size], &data, sizeof(T));
	}

private:
	winrt::com_ptr<ID3D12Resource> m_upload_buffer;
	unsigned char* m_mapped_data = nullptr;
	UINT m_element_byte_size = 0;
	bool m_is_constant_buffer = false;
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
	static UINT constant_buffer_byte_size(UINT byte_size);
};

struct render_item
{
	render_item() = default;
	DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 tex_transform = MathHelper::Identity4x4();
	int num_frames_dirty = global_num_frame_resources;
	UINT obj_cb_index = -1;
	Material* material = nullptr;
	MeshGeometry* mesh_geometry = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT index_count = 0;
	UINT start_index_location = 0;
	int base_vertex_location = 0;
};

