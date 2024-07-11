#include "IndexBuffer.h"
#include "RenderSystem.h"
#include <exception>

IndexBuffer::IndexBuffer(void* list_indices, UINT size_list, RenderSystem* system) : m_system(system), m_buffer(0)
{
	//creating the buffer description
	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.ByteWidth = 4 * size_list;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	//creating subresource data
	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_indices;
	m_size_list = size_list;
	//creating the actual buffer
	if (FAILED(m_system->m_d3d_device->CreateBuffer(&buffer_desc, &init_data, &m_buffer))) {
		throw std::exception("Index buffer not created!");
	}
}

UINT IndexBuffer::getSizeIndexList()
{
	return this->m_size_list;
}

IndexBuffer::~IndexBuffer()
{
	m_buffer->Release();
}
