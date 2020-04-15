#pragma once
#include <vector>

struct Vertex {
	float pos[2];
	float centerpos[2];
	float tex[2];
	float col[4];
};

class VertexBuffer final
{
private:
	int m_VertexNum;
	std::vector<Vertex> m_VertexList;

public:
	VertexBuffer();
	VertexBuffer(const VertexBuffer& vertexManager) = delete;
	VertexBuffer& operator=(const VertexBuffer& vertexManager) = delete;
	/*virtual*/ ~VertexBuffer();

	const Vertex* GetVertexList(void);
	int GetVertexNum(void);
	void AddVertex(const Vertex& vertex);
	std::size_t GetSize(void) const;
	void ClearVertex(void);
};