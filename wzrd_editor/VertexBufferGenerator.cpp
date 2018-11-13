#include "pch.h"
#include "VertexBufferGenerator.h"

VertexBufferGenerator::VertexBufferGenerator()
{

}

void VertexBufferGenerator::regenerate_vertices_from_model(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> model_vertices)
{
	m_vertices.clear();
	for (size_t i = 0; i < model_vertices.Size(); i++)
	{
		Vertex_tex new_vertex;

		new_vertex.Pos.x = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->x();
		new_vertex.Pos.y = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->y();
		new_vertex.Pos.z = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->z();

		new_vertex.Color.x = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->r();
		new_vertex.Color.y = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->g();
		new_vertex.Color.z = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->b();
		new_vertex.Color.w = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->a();

		new_vertex.TexC.x = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->u();
		new_vertex.TexC.y = model_vertices.GetAt(i).try_as<winrt::wzrd_editor::implementation::Vertex>()->v();

		m_vertices.push_back(std::move(new_vertex));
	}
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