#include "pch.h"
#include "VertexBufferGenerator.h"

VertexBufferGenerator::VertexBufferGenerator()
{

}

void VertexBufferGenerator::push_vertex(
	float x, float y, float z,
	float r, float g, float b, float a,
	float u, float v
)
{
	using namespace DirectX;
	m_vertices.push_back(Vertex_tex({ XMFLOAT3(x, y, z), XMFLOAT4(r,g,b,a), XMFLOAT2(u, v) }));
}

std::vector<Vertex_tex>& VertexBufferGenerator::vertices()
{
	return m_vertices;
}