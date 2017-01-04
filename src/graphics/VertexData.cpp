#include "VertexData.h"
#include "OpenGLIncludes.h"
#include <assert.h>

	
//-----------------------------------------------------------------------------
// Vertex Buffer
//-----------------------------------------------------------------------------

VertexBuffer::VertexBuffer() :
	m_numVertices(0),
	m_bufferSize(-1)
{
	m_glVertexBuffer = 0;
	m_glVertexArray  = 0;
	glGenVertexArrays(1, &m_glVertexArray);
	glGenBuffers(1, &m_glVertexBuffer);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteVertexArrays(1, &m_glVertexBuffer);
	glDeleteBuffers(1, &m_glVertexBuffer);
}

int VertexBuffer::GetVertexCount() const
{
	return m_numVertices;
}

void VertexBuffer::SetVertices(int numVertices, const VertexPosTex* pVertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);

	// Buffer the vertex data.
	int newBufferSize = numVertices * sizeof(VertexPosTex);
	if (m_bufferSize < 0)
	{
		// Buffer new vertices.
		glBufferData(GL_ARRAY_BUFFER, newBufferSize, pVertices, GL_STATIC_DRAW);
		m_bufferSize = newBufferSize;
	}
	else
	{
		// Buffer over existing vertices.
		assert(newBufferSize <= m_bufferSize);//, "You cannot increase the buffer size");
		glBufferSubData(GL_ARRAY_BUFFER, 0, newBufferSize, pVertices);
	}

	// Set the attribute locations.
	glBindVertexArray(m_glVertexArray);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex), (void*) (offsetof(VertexPosTex, position)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex), (void*) (offsetof(VertexPosTex, texCoord)));
	glBindVertexArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}

void VertexBuffer::SetVertices(int numVertices, const VertexPosTexNorm* pVertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);

	// Buffer the vertex data.
	int newBufferSize = numVertices * sizeof(VertexPosTexNorm);
	if (m_bufferSize < 0)
	{
		// Buffer new vertices.
		glBufferData(GL_ARRAY_BUFFER, newBufferSize, pVertices, GL_STATIC_DRAW);
		m_bufferSize = newBufferSize;
	}
	else
	{
		// Buffer over existing vertices.
		assert(newBufferSize <= m_bufferSize);//, "You cannot increase the buffer size"); // We mustn't increase the buffer size.
		glBufferSubData(GL_ARRAY_BUFFER, 0, newBufferSize, pVertices);
	}

	// Set the attribute locations.
	glBindVertexArray(m_glVertexArray);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNorm), (void*) (offsetof(VertexPosTexNorm, position)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNorm), (void*) (offsetof(VertexPosTexNorm, texCoord)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTexNorm), (void*) (offsetof(VertexPosTexNorm, normal)));
	glBindVertexArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}


//-----------------------------------------------------------------------------
// Index Buffer
//-----------------------------------------------------------------------------

IndexBuffer::IndexBuffer()
{
	m_numIndices = 0;
	glGenBuffers(1, &m_glIndexBuffer);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_glIndexBuffer);
}


void IndexBuffer::SetIndices(unsigned int numIndices, const unsigned int* pIndices)
{
	m_numIndices = numIndices;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), pIndices, GL_STATIC_DRAW);
}

unsigned int IndexBuffer::GetIndexCount() const
{
	return m_numIndices;
}


//-----------------------------------------------------------------------------
// Vertex Data
//-----------------------------------------------------------------------------

VertexData::VertexData()
	: m_vertexStart(0)
	, m_vertexCount(0)
{
}

VertexData::VertexData(unsigned int start, unsigned int count)
	: m_vertexStart(start)
	, m_vertexCount(count)
{
}

VertexData::~VertexData()
{
}


//-----------------------------------------------------------------------------
// Index Data
//-----------------------------------------------------------------------------

IndexData::IndexData()
	: m_indexStart(0)
	, m_indexCount(0)
{
}

IndexData::IndexData(unsigned int start, unsigned int count)
	: m_indexStart(start)
	, m_indexCount(count)
{
}

IndexData::~IndexData()
{
}

