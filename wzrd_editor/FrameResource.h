#pragma once
#include "MathHelper.h"
#include "utilities.h"
//
//struct Vertex
//{
//	DirectX::XMFLOAT3 Pos;
//	DirectX::XMFLOAT4 Color;
//};

struct Vertex_tex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT2 TexC;
};

struct object_constants
{
	DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 texture_transform = MathHelper::Identity4x4();
};
