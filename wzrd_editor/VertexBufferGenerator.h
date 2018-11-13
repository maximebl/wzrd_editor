#pragma once
#include "FrameResource.h"
#include "Vertex.h"

class VertexBufferGenerator {
public:
	VertexBufferGenerator();
	void push_vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v);
	void regenerate_vertices_from_model(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> model_vertices);
	std::vector<Vertex_tex>& vertices();

private:
	std::vector<Vertex_tex> m_vertices;
};