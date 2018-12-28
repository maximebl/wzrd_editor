#pragma once
#include <string>
#include <d3d12.h>
#include "winrt/Windows.Foundation.h"
#include "MathHelper.h"



namespace winrt::wzrd_editor::data
{
	struct Texture
	{
		std::string Name;
		std::wstring Filename;
		winrt::com_ptr<ID3D12Resource> Resource = nullptr;
		winrt::com_ptr<ID3D12Resource> UploadHeap = nullptr;
	};
}

struct MeshGeometry
{
	std::string Name;

	winrt::com_ptr<ID3D12Resource> VertexBufferGPU = nullptr;
	winrt::com_ptr<ID3D12Resource> SwapVertexBufferGPU = nullptr;
	winrt::com_ptr<ID3D12Resource> IndexBufferGPU = nullptr;
	winrt::com_ptr<ID3D12Resource> SwapIndexBufferGPU = nullptr;

	winrt::com_ptr<ID3D12Resource> VertexBufferUploader = nullptr;
	winrt::com_ptr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT VertexByteStride = 0;
	UINT SwapVertexByteStride = 0;

	UINT VertexBufferByteSize = 0;
	UINT SwapVertexBufferByteSize = 0;

	UINT IndexBufferByteSize = 0;
	UINT SwapIndexBufferByteSize = 0;

	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;

	UINT index_count = 0;
	UINT start_index_location = 0;
	INT base_vertex_location = 0;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;
		return vbv;
	}

	D3D12_VERTEX_BUFFER_VIEW SwapBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = SwapVertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = SwapVertexByteStride;
		vbv.SizeInBytes = SwapVertexBufferByteSize;
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

	D3D12_INDEX_BUFFER_VIEW SwapIndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = SwapIndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = SwapIndexBufferByteSize;
		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

enum class pick_modes {
	single_file_async = 0,
	multiple_files_async = 1
};

template <typename T>
class upload_buffer
{
public:
	upload_buffer(ID3D12Device* device, UINT element_count, bool is_constant_buffer, bool is_auto_resize) :
		m_is_constant_buffer(is_constant_buffer),
		m_element_count(element_count),
		m_is_auto_resize(is_auto_resize)
	{
		m_element_byte_size = sizeof(T);

		if (is_constant_buffer)
		{
			// Constant data reads must be a multiple of 256 bytes from the beginning of the heap. (aka 256 bytes aligned)
			// https://docs.microsoft.com/en-ca/windows/desktop/direct3d12/upload-and-readback-of-texture-data
			m_element_byte_size = Utilities::constant_buffer_byte_size(sizeof(T));
		}

		winrt::check_hresult(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_element_byte_size*element_count),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				winrt::guid_of<ID3D12Resource>(),
				m_upload_buffer.put_void()
			));

		winrt::check_hresult(
			// Map: Gets a CPU pointer to the specified subresource (0) in the m_upload_buffer Resource (m_upload_buffer).
			// Also invalidate the CPU cache (if needed) so that CPU reads to this address reflect modifications made by the GPU.
			// m_mapped_data is the pointer to the pointer to the resource data.
			m_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_data))
		);
	}

	~upload_buffer()
	{
		m_mapped_data = nullptr;
	}

	upload_buffer(const upload_buffer& rhs) = delete;
	upload_buffer& operator=(const upload_buffer* rhs) = delete;

	uint32_t m_element_count = 0;
	bool m_is_auto_resize = false;

	ID3D12Resource* get_resource() const
	{
		return m_upload_buffer.get();
	}

	void clear_data()
	{
		if (m_upload_buffer != nullptr)
		{
			m_upload_buffer->Unmap(0, nullptr);
		}
	}

	void copy_data(int element_index, const T& data)
	{
		std::memcpy(&m_mapped_data[element_index * m_element_byte_size], &data, sizeof(T));
	}

	unsigned char* m_mapped_data = nullptr;
private:
	winrt::com_ptr<ID3D12Resource> m_upload_buffer;
	UINT m_element_byte_size = 0;
	bool m_is_constant_buffer = false;
};

class Utilities
{
private:
	struct compilation_result
	{
		bool is_success;
		winrt::com_ptr<ID3D10Blob> result_blob;
	};
public:
	Utilities();
	static concurrency::task<std::vector<unsigned char>> read_file_bytes(winrt::Windows::Storage::Streams::IBuffer fileBuffer);
	static compilation_result compile_shader(
		const std::string shaderType,
		const std::vector<unsigned char>& file_bytes,
		const std::string entryPoint);
	static winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> pick_file_buffer(winrt::hstring file_extension, pick_modes pick_mode);
	static concurrency::task<std::vector<unsigned char>> pick_file(winrt::hstring file_extension);
	static winrt::com_ptr<ID3D12Resource> create_default_buffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		winrt::com_ptr<ID3D12Resource>& uploadBuffer);
	static UINT constant_buffer_byte_size(UINT byte_size);
	static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> get_static_samplers();
	static void print_coordinates(float x, float y);
	static void wait(DWORD duration);
	static ID3D12Device* device;
};

