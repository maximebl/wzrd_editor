#include "pch.h"
#include "VertexBufferGenerator.h"

VertexBufferGenerator::VertexBufferGenerator()
{

}

void VertexBufferGenerator::push_vertex(float x, float y, float z, float u, float v)
{
	using namespace DirectX;
	m_vertices.push_back(Vertex_tex({ XMFLOAT3(x, y, z), XMFLOAT2(u, v) }));
}

std::vector<Vertex_tex>& VertexBufferGenerator::vertices()
{
	return m_vertices;
}