#include "VertexBuffer.h"
#include "d3d11.h"
#include <stddef.h>

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::~VertexBuffer()
{
	ClearVertex();
}

std::size_t VertexBuffer::GetSize() const
{
	return m_VertexList.size();
}

const Vertex* VertexBuffer::GetVertexList(void)
{
	return m_VertexList.data();
}

int VertexBuffer::GetVertexNum(void)
{
	return m_VertexNum;
}

void VertexBuffer::AddVertex(const Vertex& vertex)
{
	m_VertexList.push_back(vertex);
	m_VertexNum = static_cast<unsigned int>(m_VertexList.size());
}

void VertexBuffer::ClearVertex(void)
{
	m_VertexList.clear();
	m_VertexNum = 0;
}