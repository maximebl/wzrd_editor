#pragma once
#include "MathHelper.h"
#include "utilities.h"

//struct Vertex
//{
//	DirectX::XMFLOAT3 Pos;
//	DirectX::XMFLOAT3 Normal;
//	DirectX::XMFLOAT2 TexC;
//};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct Vertex_tex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT2 TexC;
};

struct simple_object_constants
{
	DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

struct simple_texture_constants
{
	DirectX::XMFLOAT4X4 texture_transform = MathHelper::Identity4x4();
};

struct Light
{
	DirectX::XMFLOAT3 strength = { 0.5f, 0.5f, 0.5f };
	float falloff_start = 1.0f;
	DirectX::XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };
	float falloff_end = 10.0f;
	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	float spot_power = 64.0f;
};

struct object_constants
{
	DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 texture_transform = MathHelper::Identity4x4();
};

struct pass_constants
{
	DirectX::XMFLOAT4X4 view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 inv_view = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 inv_proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 view_proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 inv_view_proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 eye_pos_w = { 0.0f, 0.0f, 0.0f };
	float cb_per_object_pad1 = 0.0f;
	DirectX::XMFLOAT2 render_target_size = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 inv_render_target_size = { 0.0f, 0.0f };
	float near_z = 0.0f;
	float far_z = 0.0f;
	float total_time = 0.0f;
	float delta_time = 0.0f;
	DirectX::XMFLOAT4 ambient_light = { 0.0f,0.0f,0.0f,1.0f };
	Light lights[3];
};

struct material_constants
{
	DirectX::XMFLOAT4 diffuse_albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 fresnel_r0 = { 0.01f, 0.01f, 0.01f };
	float roughness = 0.25f;
	DirectX::XMFLOAT4X4 mat_transform = MathHelper::Identity4x4();
};

struct frame_resource
{
	frame_resource(ID3D12Device* device, UINT pass_count, UINT object_count, UINT material_count);
	frame_resource(const frame_resource& rhs) = delete;
	frame_resource& operator=(const frame_resource& rhs) = delete;

	winrt::com_ptr<ID3D12CommandAllocator> cmd_list_allocator;
	std::unique_ptr<upload_buffer<pass_constants>> pass_cb = nullptr;
	std::unique_ptr<upload_buffer<material_constants>> material_cb = nullptr;
	std::unique_ptr<upload_buffer<object_constants>> object_cb = nullptr;
	UINT64 fence = 0;
};