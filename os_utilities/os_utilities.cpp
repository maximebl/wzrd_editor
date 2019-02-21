#include "pch.h"
#include "os_utilities.h"

namespace os_utilities
{
	concurrency::task<std::vector<unsigned char>> read_file_bytes(winrt::Windows::Storage::Streams::IBuffer file_buffer)
	{

		return concurrency::create_task([file_buffer]() -> std::vector<unsigned char>
		{
			std::vector<unsigned char> file_bytes = {};

			if (file_buffer)
			{
				int file_size = file_buffer.Length();
				file_bytes.assign(file_size, 0);
				auto data_reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(file_buffer);
				data_reader.ReadBytes(file_bytes);
			}
			return file_bytes;
		});
	}

	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> pick_file_buffer(winrt::hstring file_extension, pick_modes pick_mode)
	{
		winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
		filePicker.FileTypeFilter().Append(file_extension);

		switch (pick_mode)
		{
		case pick_modes::single_file_async:
		{
			auto file = co_await filePicker.PickSingleFileAsync();
			if (file != nullptr)
			{
				return co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
			}
		}
		break;

		case pick_modes::multiple_files_async:
		{
			//TODO: add support for selecting multiple files
			throw winrt::hresult_not_implemented();
		}
		break;
		}
		co_return nullptr;
	}

	concurrency::task<std::vector<unsigned char>> pick_file(winrt::hstring file_extension)
	{
		auto file_buffer = co_await pick_file_buffer(file_extension, pick_modes::single_file_async);
		return co_await read_file_bytes(file_buffer);
	}

	void wait_duration(DWORD duration)
	{
		HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		WaitForSingleObject(event_handle, duration);
		CloseHandle(event_handle);
	}

	winrt::Windows::Foundation::IAsyncAction show_error_dialog(winrt::hstring error_message, winrt::hstring title)
	{
		auto dialog = winrt::Windows::UI::Xaml::Controls::ContentDialog();
		dialog.Title(box_value(title));
		dialog.Content(box_value(error_message));
		dialog.CloseButtonText(L"Ok");
		co_await dialog.ShowAsync();
	}




}