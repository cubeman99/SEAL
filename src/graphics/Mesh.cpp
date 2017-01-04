#include "Mesh.h"


//-----------------------------------------------------------------------------
// Sub-Mesh
//-----------------------------------------------------------------------------

Mesh::Mesh() :
	m_firstIndex(-1),
	m_numIndices(0),
	m_primitiveType(VertexPrimitiveType::TRIANGLES)
{
}

Mesh::~Mesh()
{
}

