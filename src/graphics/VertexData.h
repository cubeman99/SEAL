#ifndef _VERTEX_DATA_H_
#define _VERTEX_DATA_H_

#include <graphics/Color.h>
#include "OpenGLIncludes.h"
#include <math/Vector2f.h>
#include <math/Vector3f.h>
#include <math/Matrix4f.h>
#include <assert.h>
	
//-----------------------------------------------------------------------------
// Standard vertex formats for models
//-----------------------------------------------------------------------------

// The format in which vertex lists or vertex element arrays are organized
// into primitives.
struct VertexPrimitiveType
{ 
	typedef int value_type;

	enum
	{
		POINTS = 0,
		LINES,
		LINE_STRIP,
		LINE_LOOP,		// A single line loop
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		QUADS,
		QUAD_STRIP,
		POLYGON,			// This is a *single* polygon
		NUM_VERTEX_PRIMITIVE_TYPES
	};
};

struct PrimitiveList
{
	int m_firstIndex;
	int m_numIndices;

	PrimitiveList() {}
	PrimitiveList(int firstIndex, int indexCount) :
		m_firstIndex(firstIndex),
		m_numIndices(indexCount)
	{}
};


//-----------------------------------------------------------------------------
// Standard vertex formats for models
//-----------------------------------------------------------------------------

struct VertexType
{
	enum
	{
		POSITION	= 0x1, // All vertices should have a position.
		NORMAL		= 0x2,
		TEX_COORD	= 0x4,
		COLOR		= 0x8,
	};
};

// These locations are important for vertex shaders!!
struct VertexAttribLocations
{
	enum
	{
		POSITION	= 0,	// layout (location = 0) in vec3 a_vertPos;
		NORMAL		= 1,	// layout (location = 1) in vec3 a_vertNorm;
		TEX_COORD	= 2,	// layout (location = 2) in vec2 a_vertTexCoord;
		COLOR		= 3,	// layout (location = 3) in vec4 a_vertColor;
	};
};


#define DECLARE_VERTEX_TYPE(_type) \
	enum { kVertexType = _type }


struct VertexPosTexNormCol
{
	Vector3f	position;
	Vector2f	texCoord;
	Vector3f	normal;
	Vector4f	color;

	VertexPosTexNormCol()
	{}
	
	DECLARE_VERTEX_TYPE(VertexType::POSITION |
						VertexType::TEX_COORD |
						VertexType::NORMAL |
						VertexType::COLOR);
};

// A vertex with a position, texture coordinate, and normal.
struct VertexPosTexNorm
{
	Vector3f	position;
	Vector2f	texCoord;
	Vector3f	normal;

	VertexPosTexNorm()
	{}
	VertexPosTexNorm(const Vector3f& position, const Vector2f& texCoord, const Vector3f& normal) :
		position(position), texCoord(texCoord), normal(normal)
	{}

	DECLARE_VERTEX_TYPE(VertexType::POSITION |
						VertexType::TEX_COORD |
						VertexType::NORMAL);
};

struct VertexPosTexCol
{
	Vector3f	position;
	Vector2f	texCoord;
	Vector4f	color;

	VertexPosTexCol()
	{}

	DECLARE_VERTEX_TYPE(VertexType::POSITION |
						VertexType::TEX_COORD |
						VertexType::COLOR);
};

struct VertexPosNormCol
{
	Vector3f	position;
	Vector3f	normal;
	Vector4f	color;

	VertexPosNormCol()
	{}
	VertexPosNormCol(const Vector3f& position) :
		position(position)
	{}
	VertexPosNormCol(const Vector3f& position, const Vector3f& normal, const Vector4f& color) :
		position(position), normal(normal), color(color)
	{}
	VertexPosNormCol(const Vector3f& position, const Vector3f& normal, const Color& color) :
		position(position), normal(normal), color(color.ToVector4f())
	{}

	DECLARE_VERTEX_TYPE(VertexType::POSITION |
						VertexType::NORMAL |
						VertexType::COLOR);
};

// A vertex with a position and texture coordinate.
struct VertexPosTex
{
	Vector3f	position;
	Vector2f	texCoord;
	
	VertexPosTex(float x, float y, float z, float u, float v) :
		position(x, y, z),
		texCoord(u, v)
	{}
	VertexPosTex(const Vector3f& position, const Vector2f& texCoord) :
		position(position),
		texCoord(texCoord)
	{}

	DECLARE_VERTEX_TYPE(VertexType::POSITION | VertexType::TEX_COORD);
};

struct VertexPosNorm
{
	Vector3f	position;
	Vector3f	normal;
	
	VertexPosNorm()
	{}
	VertexPosNorm(const Vector3f& position, const Vector3f& normal) :
		position(position),
		normal(normal)
	{}
	
	DECLARE_VERTEX_TYPE(VertexType::POSITION | VertexType::NORMAL);
};

struct VertexPosCol
{
	Vector3f	position;
	Vector4f	color;

	VertexPosCol() {}

	VertexPosCol(const Vector3f& position) :
		position(position)
	{}
	VertexPosCol(const Vector3f& position, const Vector4f& color) :
		position(position), color(color)
	{}
	VertexPosCol(const Vector3f& position, const Color& color) :
		position(position), color(color.ToVector4f())
	{}
	
	DECLARE_VERTEX_TYPE(VertexType::POSITION | VertexType::COLOR);
};

struct VertexPos
{
	Vector3f	position;
	
	VertexPos(float x, float y, float z) :
		position(x, y, z)
	{}
	VertexPos(const Vector3f& position) :
		position(position)
	{}


	DECLARE_VERTEX_TYPE(VertexType::POSITION);
};


//-----------------------------------------------------------------------------
// OpenGL Vertex Buffer
//-----------------------------------------------------------------------------

// An object for storing a buffer of vertices interlaced a particlar set of
// attributes.
class VertexBuffer
{
	friend class Renderer;

public:
	// Constructors/destructor.
	VertexBuffer();
	~VertexBuffer();
	
	// Accessors.
	int GetVertexCount() const;

	// Mutators.
	template <class T>
	void SetVertices(int numVertices, const T* vertices);

private:
public:
	int				m_numVertices;		// The number of vertices in the vertex buffer.
	int				m_bufferSize;		// The size in bytes of the vertex buffer.
	unsigned int	m_glVertexBuffer;	// The ID for the OpenGL vertex buffer.
	unsigned int	m_glVertexArray;	// The ID for the OpenGL vertex array object.
	unsigned int	m_vertexType;
};


//-----------------------------------------------------------------------------
// OpenGL Index Buffer
//-----------------------------------------------------------------------------

// An object for storing a buffer of indices that refers to the indices of
// elements inside another buffer (usually a vertex buffer).
class IndexBuffer
{
	friend class Renderer;

public:
	// Constructors/destructor.
	IndexBuffer();
	~IndexBuffer();
	
	// Accessors.
	unsigned int GetIndexCount() const;
	
	// Mutators.
	void SetIndices(unsigned int numIndices, const unsigned int* pIndices);

private:
public:
	unsigned 		m_numIndices;
	unsigned int	m_glIndexBuffer; // The ID for the OpenGL index buffer.
};


//-----------------------------------------------------------------------------
// Vertex Data
//-----------------------------------------------------------------------------
class VertexData
{
public:
	VertexData();
	VertexData(unsigned int start, unsigned int count);
	~VertexData();

	void BufferVertices(int numVertices, const VertexPosTex* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	void BufferVertices(int numVertices, const VertexPosTexNorm* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	template <class T>
	void BufferVertices(int numVertices, const T* vertices)
	{
		m_vertexStart = 0;
		m_vertexCount = numVertices;
		m_vertexBuffer.SetVertices(numVertices, vertices);
	}

	void SetVertexRange(unsigned int start, unsigned int count)
	{
		m_vertexStart = start;
		m_vertexCount = count;
	}

	inline void SetVertexStart(unsigned int start) { m_vertexStart = start; }
	inline void SetVertexCount(unsigned int count) { m_vertexCount = count; }

	inline unsigned int GetStart() const { return m_vertexStart; }
	inline unsigned int GetCount() const { return m_vertexCount; }
	inline VertexBuffer* GetVertexBuffer() { return &m_vertexBuffer; }


public:
	unsigned int	m_vertexStart;
	unsigned int	m_vertexCount;
	VertexBuffer	m_vertexBuffer;
};


//-----------------------------------------------------------------------------
// Index Data
//-----------------------------------------------------------------------------
class IndexData
{
public:
	IndexData();
	IndexData(unsigned int start, unsigned int count);
	~IndexData();
		
	void BufferIndices(unsigned int numIndices, const unsigned int* indices)
	{
		m_indexStart = 0;
		m_indexCount = numIndices;
		m_indexBuffer.SetIndices(numIndices, indices);
	}

	void SetIndexRange(unsigned int start, unsigned int count)
	{
		m_indexStart = start;
		m_indexCount = count;
	}

	inline void SetIndexStart(unsigned int start) { m_indexStart = start; }
	inline void SetIndexCount(unsigned int count) { m_indexCount = count; }

	inline unsigned int GetStart() const { return m_indexStart; }
	inline unsigned int GetCount() const { return m_indexCount; }
	inline IndexBuffer* GetIndexBuffer() { return &m_indexBuffer; }


public:
	unsigned int	m_indexStart;
	unsigned int	m_indexCount;
	IndexBuffer		m_indexBuffer;
};




template <class T>
void VertexBuffer::SetVertices(int numVertices, const T* vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertexBuffer);

	// Buffer the vertex data.
	int newBufferSize = numVertices * sizeof(T);
	if (m_bufferSize < 0)
	{
		// Buffer new vertices.
		glBufferData(GL_ARRAY_BUFFER, newBufferSize, vertices, GL_STATIC_DRAW);
		m_bufferSize = newBufferSize;
	}
	else
	{
		// Buffer over existing vertices.
		assert(newBufferSize <= m_bufferSize);//, "You cannot increase the buffer size"); // We mustn't increase the buffer size.
		glBufferSubData(GL_ARRAY_BUFFER, 0, newBufferSize, vertices);
	}
		
	// Set the attribute locations.
	glBindVertexArray(m_glVertexArray);

	unsigned int offset = 0;
	int sizeOfVertex = sizeof(T);

	m_vertexType = T::kVertexType;

	if (m_vertexType & VertexType::POSITION)
	{
		glEnableVertexAttribArray(VertexAttribLocations::POSITION);
		glVertexAttribPointer(VertexAttribLocations::POSITION, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector3f);
	}
	if (m_vertexType & VertexType::TEX_COORD)
	{
		glEnableVertexAttribArray(VertexAttribLocations::TEX_COORD);
		glVertexAttribPointer(VertexAttribLocations::TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector2f);
	}
	if (m_vertexType & VertexType::NORMAL)
	{
		glEnableVertexAttribArray(VertexAttribLocations::NORMAL);
		glVertexAttribPointer(VertexAttribLocations::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector3f);
	}
	if (m_vertexType & VertexType::COLOR)
	{
		glEnableVertexAttribArray(VertexAttribLocations::COLOR);
		glVertexAttribPointer(VertexAttribLocations::COLOR, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*) offset);
		offset += sizeof(Vector4f);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_numVertices = numVertices;
}



#endif // _VERTEX_DATA_H_