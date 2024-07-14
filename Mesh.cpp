#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <locale>
#include <filesystem>
#include "GraphicsEngine.h"
#include "VertexMesh.h"

Mesh::Mesh(const wchar_t* full_path) : Resource(full_path)
{
	//mesh attributes
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	//handling file path
	std::wstring ws(full_path);
	std::string inputfile(ws.begin(), ws.end());
	//loading the mesh
	bool res = tinyobj::LoadObj(&attribs, &shapes, &materials, &warn, &err, inputfile.c_str());
	//exceptions
	if (!err.empty()) throw std::exception("Mesh not created successfully!");
	if (!res) throw std::exception("Mesh not created successfully!");
	if (shapes.size() > 1) throw std::exception("Mesh not created successfully!");

	//vertex lists
	std::vector<VertexMesh> list_vertices;
	std::vector<unsigned int> list_indices;
	//looping over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		size_t index_offset = 0;
		//reserving in memory for faster inserts
		list_vertices.reserve(shapes[s].mesh.indices.size());
		list_indices.reserve(shapes[s].mesh.indices.size());
		//looping over faces
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			unsigned char num_face_verts = shapes[s].mesh.num_face_vertices[f];
			//looping over vertices
			for (unsigned char v = 0; v < num_face_verts; v++)
			{
				tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];
				//vertex
				tinyobj::real_t vx = attribs.vertices[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.vertex_index) * 3];
				tinyobj::real_t vy = attribs.vertices[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.vertex_index) * 3 + 1];
				tinyobj::real_t vz = attribs.vertices[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.vertex_index) * 3 + 2];
				//texture
				tinyobj::real_t tx = attribs.texcoords[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.texcoord_index) * 2];
				tinyobj::real_t ty = attribs.texcoords[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.texcoord_index) * 2 + 1];
				//normal
				tinyobj::real_t nx = attribs.normals[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.normal_index) * 3];
				tinyobj::real_t ny = attribs.normals[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.normal_index) * 3 + 1];
				tinyobj::real_t nz = attribs.normals[static_cast<std::vector<tinyobj::real_t, std::allocator<tinyobj::real_t>>::size_type>(index.normal_index) * 3 + 2];

				VertexMesh vertex(Vector3D(vx, vy, vz), Vector2D(tx, ty), Vector3D(nx, ny, nz));
				list_vertices.push_back(vertex);

				list_indices.push_back((unsigned int)index_offset + v);
			}

			index_offset += num_face_verts;
		}
	}

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getVertexMeshLayoutShaderByteCodeAndSize(&shader_byte_code, &size_shader);
	m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(&list_vertices[0], sizeof(VertexMesh),
		(UINT)list_vertices.size(), shader_byte_code, (UINT)size_shader);
	m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(&list_indices[0], (UINT)list_indices.size());

}


Mesh::~Mesh()
{
}

const VertexBufferPtr& Mesh::getVertexBuffer()
{
	return m_vertex_buffer;
}

const IndexBufferPtr& Mesh::getIndexBuffer()
{
	return m_index_buffer;
}