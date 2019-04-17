#include "pch.h"
#include "dds_creation_dialog.h"
#include "utilities.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::wzrd_editor::implementation
{
	dds_creation_dialog::dds_creation_dialog()
	{
		InitializeComponent();

		auto alpha_modes_cpy = alpha_modes();
		Utilities::generate_alpha_modes_attributes(alpha_modes_cpy);
	}

	winrt::Windows::Foundation::IAsyncAction dds_creation_dialog::onclick_create(Windows::Foundation::IInspectable const & sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const & args)
	{
		//using namespace DirectX;

		//Image new_image;
		//new_image.format = DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM;
		//new_image.height = m_height;
		//new_image.width = m_width;

		//size_t row_pitch;
		//size_t slice_pitch;
		//check_hresult(
		//	ComputePitch(new_image.format, new_image.width, new_image.height, row_pitch, slice_pitch)
		//);

		//new_image.slicePitch = slice_pitch;
		//new_image.rowPitch = row_pitch;
		//new_image.pixels = 
		//Blob new_blob;
		//SaveToDDSMemory(new_image, DDS_FLAGS::DDS_FLAGS_FORCE_DX10_EXT, new_blob);

		co_return;
	}

	uint32_t dds_creation_dialog::width()
	{
		return m_width;
	}

	void dds_creation_dialog::width(uint32_t value)
	{
		update_value(L"width", m_width, value);
	}

	uint32_t dds_creation_dialog::height()
	{
		return m_height;
	}

	void dds_creation_dialog::height(uint32_t value)
	{
		update_value(L"height", m_height, value);
	}

	graphics::alpha_mode dds_creation_dialog::alpha_mode()
	{
		return m_alpha_mode;
	}

	void dds_creation_dialog::alpha_mode(graphics::alpha_mode const& value)
	{
		update_value(L"alpha_mode", m_alpha_mode, value);
	}

	Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> dds_creation_dialog::alpha_modes()
	{
		return m_alpha_modes;
	}

	bool dds_creation_dialog::is_texture_array()
	{
		return m_is_texture_array;
	}

	void dds_creation_dialog::is_texture_array(bool value)
	{
		update_value(L"is_texture_array", m_is_texture_array, value);
	}

	hstring dds_creation_dialog::texture_name()
	{
		return m_texture_name;
	}

	void dds_creation_dialog::texture_name(hstring const& value)
	{
		update_value(L"texture_name", m_texture_name, value);
	}

	winrt::event_token dds_creation_dialog::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
	{
		return m_property_changed.add(handler);
	}

	void dds_creation_dialog::PropertyChanged(winrt::event_token const& token) noexcept
	{
		m_property_changed.remove(token);
	}
}
