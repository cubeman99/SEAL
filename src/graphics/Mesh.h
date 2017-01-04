#ifndef _MESH_H_
#define _MESH_H_

#include "VertexData.h"


class Mesh
{
private:
	public:
		// Constructors/destructor.
		Mesh();
		~Mesh();
	
		// Accessors.
		inline int					GetFirstIndex()		const { return m_firstIndex; }
		inline int					GetNumIndices()		const { return m_numIndices; }
		//inline const Bounds&		GetBounds()			const { return m_bounds; }
		inline VertexPrimitiveType::value_type	GetPrimitiveType()	const { return m_primitiveType; }
		
		inline VertexData*	GetVertexData()	{ return &m_vertexData; }
		inline IndexData*	GetIndexData()	{ return &m_indexData; }

		// Mutators.
		inline void SetIndices(int firstIndex, int numIndices)	{ m_firstIndex = firstIndex; m_numIndices = numIndices; }
		//inline void SetBounds(const Bounds& bounds)				{ m_bounds =  bounds; }
		inline void SetPrimitiveType(VertexPrimitiveType::value_type type)	{ m_primitiveType = type; }

	private:
		// Prevent copying.
		Mesh(Mesh& other);
		void operator=(Mesh& other) {}

		//Bounds	m_bounds;		// The axis-aligned bounding box of the submesh's vertices,
		int			m_firstIndex;	// Index to the first index of this sub-mesh in the mesh's index buffer.
		int			m_numIndices;	// Number of indices for this sub-mesh.

		VertexData	m_vertexData;
		IndexData	m_indexData;

		VertexPrimitiveType::value_type m_primitiveType; // How the index buffer is organized into primitives.
};


#endif // _MESH_H_