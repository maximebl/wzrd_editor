#include "FrameResource.h"

frame_resource::frame_resource(ID3D12Device* device, UINT pass_count, UINT object_count, UINT material_count)
{
	winrt::check_hresult(
		device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			__uuidof(cmd_list_allocator),
			cmd_list_allocator.put_void()
		));

	pass_cb = std::make_unique<upload_buffer<pass_constants>>(device, pass_count, true);
	material_cb = std::make_unique<upload_buffer<material_constants>>(device, material_count, true);
	object_cb = std::make_unique<upload_buffer<object_constants>>(device, object_count, true);
}