#pragma once
#include "pch.h"

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
			m_element_byte_size = constant_buffer_byte_size(sizeof(T));
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

	UINT constant_buffer_byte_size(UINT byte_size)
	{
		return (byte_size + 255) & ~255;
	}

	unsigned char* m_mapped_data = nullptr;

private:
	winrt::com_ptr<ID3D12Resource> m_upload_buffer;
	UINT m_element_byte_size = 0;
	bool m_is_constant_buffer = false;
};