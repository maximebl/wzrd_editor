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
