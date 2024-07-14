#pragma once

#include "Vector3D.h"
#include "Vector2D.h"

class VertexMesh {
public:
	VertexMesh() : m_pos(), m_texcoord(), m_normal()
	{
	}
	VertexMesh(Vector3D position, Vector2D texcoord, Vector3D normal) :
		m_pos(position),
		m_texcoord(texcoord),
		m_normal(normal)
	{
	}
	VertexMesh(const VertexMesh& vertex) :
		m_pos(vertex.m_pos),
		m_texcoord(vertex.m_texcoord),
		m_normal(vertex.m_normal)
	{
	}
	~VertexMesh()
	{
	}
public:
	Vector3D m_pos;
	Vector2D m_texcoord;
	Vector3D m_normal;
};