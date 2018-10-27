#pragma once
#include "FrameResource.h"

class VertexBufferGenerator {
public:
	VertexBufferGenerator();
	void push_vertex(float x, float y, float z, float u, float v);
	std::vector<Vertex_tex>& vertices();

private:
	std::vector<Vertex_tex> m_vertices;
};