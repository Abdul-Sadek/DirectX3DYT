#pragma once

#include "Vector2D.h"
#include "Vector3D.h"

class VertexMesh {
public:
	VertexMesh() : m_pos(), m_texcoord()
	{
	}
	VertexMesh(Vector3D position, Vector2D texcoord) :m_pos(position), m_texcoord(texcoord)
	{
	}
	VertexMesh(const VertexMesh& vertex) :m_pos(vertex.m_pos), m_texcoord(vertex.m_texcoord)
	{
	}
	~VertexMesh()
	{
	}
public:
	Vector2D m_texcoord;
	Vector3D m_pos;
};