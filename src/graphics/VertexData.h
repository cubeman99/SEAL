#ifndef _VERTEX_DATA_H_
#define _VERTEX_DATA_H_

#include <graphics/Color.h>
//#include <cmgGraphics/opengl/cmgTextureParams.h>
#include <math/Vector2f.h>
#include <math/Vector3f.h>
//#include <math/Matrix3f.h>
#include <math/Matrix4f.h>

	
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

#define NUM_BONES_PER_VERTEX 4

// A standard model vertex, with position, normal, texCoord, color, and bone
// weights/indices.
struct ModelVertex
{
	Vector3f	position;
	Vector3f	normal;
	Vector2f	texCoord;
	Color		color;
	float		boneWeights[NUM_BONES_PER_VERTEX];
	int			boneIndices[NUM_BONES_PER_VERTEX];
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
	VertexPosTex(Vector3f position, Vector2f texCoord) :
		position(position),
		texCoord(texCoord)
	{}
};

// A vertex with a position, texture coordinate, and normal.
struct VertexPosTexNorm
{
	Vector3f	position;
	Vector2f	texCoord;
	Vector3f	normal;
};

// A vertex with a position, texture coordinate, and normal.
//struct VertexPosTexTBN
//{
//	Vector3f	position;
//	Vector2f	texCoord;
//	Matrix3f	tbn;
//};


//-----------------------------------------------------------------------------
// OpenGL Vertex Buffer
//-----------------------------------------------------------------------------

// An object for storing a buffer of vertices interlaced a particlar set of
// attributes.
class VertexBuffer
{
public:
	// Constructors/destructor.
	VertexBuffer();
	~VertexBuffer();
	
	// Accessors.
	int GetVertexCount() const;

	// Mutators.
	void SetVertices(int numVertices, const VertexPosTex* pVertices);
	void SetVertices(int numVertices, const VertexPosTexNorm* pVertices);
	//void SetVertices(const ModelVertex* pVertices, int numVertices);

private:
public:
	int				m_numVertices;		// The number of vertices in the vertex buffer.
	int				m_bufferSize;		// The size in bytes of the vertex buffer.
	unsigned int	m_glVertexBuffer;	// The ID for the OpenGL vertex buffer.
	unsigned int	m_glVertexArray;	// The ID for the OpenGL vertex array object.
};


//-----------------------------------------------------------------------------
// OpenGL Index Buffer
//-----------------------------------------------------------------------------

// An object for storing a buffer of indices that refers to the indices of
// elements inside another buffer (usually a vertex buffer).
class IndexBuffer
{
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


#endif // _VERTEX_DATA_H_