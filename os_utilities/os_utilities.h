#pragma once
#include <ppltasks.h>
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Storage.Streams.h"

enum class pick_modes {
	single_file_async = 0,
	multiple_files_async = 1
};

concurrency::task<std::vector<unsigned char>> pick_file(winrt::hstring file_extension);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> pick_file_buffer(winrt::hstring file_extension, pick_modes pick_mode);
concurrency::task<std::vector<unsigned char>> read_file_bytes(winrt::Windows::Storage::Streams::IBuffer fileBuffer);
void wait_duration(DWORD duration);