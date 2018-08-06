#pragma once
#include <string>
#include <d3d12.h>
#include "winrt/Windows.Foundation.h"

struct Texture
{
	std::string Name;
	std::wstring Filename;
	winrt::com_ptr<ID3D12Resource> Resource = nullptr;
	winrt::com_ptr<ID3D12Resource> UploadHeap = nullptr;
};

class Utilities
{
public:
	Utilities();
	static concurrency::task<std::vector<unsigned char>> read_shader_file(winrt::Windows::Storage::Streams::IBuffer fileBuffer);
	static winrt::com_ptr<ID3DBlob> compile_shader(const std::string& shaderType, const std::vector<unsigned char>& file_bytes, const std::string& entryPoint);
};
