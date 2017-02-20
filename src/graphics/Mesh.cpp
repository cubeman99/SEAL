#include "Mesh.h"
#include <vector>
#include <sstream>

Mesh::Mesh() :
	m_firstIndex(-1),
	m_numIndices(0),
	m_primitiveType(VertexPrimitiveType::TRIANGLES),
	m_transformMatrix(Matrix4f::IDENTITY)
{
}

Mesh::~Mesh()
{
}



//-----------------------------------------------------------------------------
// Static methods
//-----------------------------------------------------------------------------

struct ObjFace
{
	int position;
	int texCoord;
	int normal;

	ObjFace() :
		position(-1),
		texCoord(-1),
		normal(-1)
	{}
};

Mesh* Mesh::ImportFromOBJ(const std::string& objFileContents)
{
	Vector3f v3;
	Vector2f v2;
	std::string line;
	std::string lineType;

	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> texCoords;
	std::vector<ObjFace> faces;

	std::stringstream fileStream(objFileContents);
	
	// Read the file line by line.
	while (std::getline(fileStream, line))
	{
		// Ignore empty lines or commented lines.
		if (line.length() == 0 || line[0] == '#')
			continue;

		std::stringstream lineStream(line);
		lineStream >> lineType;
		
		// Handle each case
		if (lineType == "v")
		{
			// v = vertex position
			// Ex: v 0.697566 -0.456340 0.845467
			if ((lineStream >> v3.x) &&
				(lineStream >> v3.y) &&
				(lineStream >> v3.z))
				positions.push_back(v3);
		}
		else if (lineType == "vt")
		{
			// vt = vertex texture coordinate
			// Example: vt 0.123412 0.821312
			if ((lineStream >> v2.x) &&
				(lineStream >> v2.y))
				texCoords.push_back(v2);
		}
		else if (lineType == "vn")
		{
			// vn = vertex normal
			// Example: vn -0.989900 -0.140000 0.024600
			if ((lineStream >> v3.x) &&
				(lineStream >> v3.y) &&
				(lineStream >> v3.z))
				normals.push_back(v3);
		}
		else if (lineType == "f")
		{
			// f = face
			// Example: f 27//3 28//3 5//3
			// vertex-index/tex-coord-index/normal-index
			std::string token;
			while (lineStream >> token)
			{
				ObjFace face;

				unsigned int index = token.find_first_of('/', 0);
				if (index > 0)
					face.position = atoi(token.substr(0, index).c_str()) - 1;
				unsigned int start = index + 1;
				index = token.find_first_of('/', start);
				if (index > start)
					face.texCoord = atoi(token.substr(start, index - start).c_str()) - 1;
				start = index + 1;
				if (start < token.length())
					face.normal = atoi(token.substr(start, token.length() - start).c_str()) - 1;
				
				//cout << position << " / " << texCoord << " / " << normal << endl;

				faces.push_back(face);
			}
		}
		else if (lineType == "o")
		{
			// o = new object
			// (ignore this)
		}
		else if (lineType == "s")
		{
			// s = ???
			// (ignore this)
		}
	}

	// Compile the vertex and face data into a list of vertices and indices.
	bool hasNormals = (normals.size() > 0);
	bool hasTexCoords = (texCoords.size() > 0);
	bool hasColors = (texCoords.size() > 0);
	std::vector<VertexPosNorm> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		VertexPosNorm vertex;
		vertex.position = positions[faces[i].position];
		vertex.normal = normals[faces[i].normal];
		vertex.normal.Normalize();
		vertices.push_back(vertex);
		indices.push_back(i);
	}

	// Create the mesh.
	Mesh* mesh = new Mesh();
	mesh->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
	mesh->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
	return mesh;
}



