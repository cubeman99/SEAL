#include "World.h"
#include <math/MathLib.h>
#include <math/Matrix4f.h>
#include <math/Vector2f.h>


World::World() :
	m_radius(1.0f)
{
	// Create a icosahedron.
	float g = 1.61803398875f;
	float l = 1.0f;
	Vector3f xNyNz = Vector3f(0, -l, -g);
	Vector3f xPyNz = Vector3f(0, l, -g);
	Vector3f xPyPz = Vector3f(0, l, g);
	Vector3f xNyPz = Vector3f(0, -l, g);
	Vector3f yNxNz = Vector3f(-g, 0, -l);
	Vector3f yPxNz = Vector3f(g, 0, -l);
	Vector3f yPxPz = Vector3f(g, 0, l);
	Vector3f yNxPz = Vector3f(-g, 0, l);
	Vector3f zNxNy = Vector3f(-l, -g, 0);
	Vector3f zPxNy = Vector3f(l, -g, 0);
	Vector3f zPxPy = Vector3f(l, g, 0);
	Vector3f zNxPy = Vector3f(-l, g, 0);
	Vector3f startingPositions[] = {
		xPyPz, yPxPz, xNyPz,
		xPyPz, xNyPz, yNxPz,
		xPyNz, xNyNz, yPxNz,
		xPyNz, yNxNz, xNyNz,
		yPxPz, zPxPy, yPxNz,
		yPxPz, yPxNz, zPxNy,
		yNxPz, yNxNz, zNxPy,
		yNxPz, zNxNy, yNxNz,
		zPxPy, xPyPz, zNxPy,
		zPxPy, zNxPy, xPyNz,
		zPxNy, zNxNy, xNyPz,
		zPxNy, xNyNz, zNxNy,
		xPyPz, zPxPy, yPxPz,
		xPyPz, yNxPz, zNxPy,
		xNyPz, yPxPz, zPxNy,
		xNyPz, zNxNy, yNxPz,
		xPyNz, yPxNz, zPxPy,
		xPyNz, zNxPy, yNxNz,
		xNyNz, zPxNy, yPxNz,
		xNyNz, yNxNz, zNxNy,
	};

	std::vector<VertexPosTexNorm> vertices;
	for (int i = 0; i < 20 * 3; i++)
	{
		VertexPosTexNorm v;
		v.position = startingPositions[i];
		v.position.Normalize();
		v.normal = v.position;
		v.texCoord = Vector2f::ZERO;
		vertices.push_back(v);
	}

	// Normalize vertices.
	for (unsigned int i = 0; i < m_vertices.size(); i++)
		vertices[i].position.Normalize();

	// Subdivide.
	for (int k = 0; k < 5; k++)
	{
		std::vector<VertexPosTexNorm> newVertices;
		for (unsigned int i = 0; i < vertices.size(); i += 3)
		{
			VertexPosTexNorm a = vertices[i];
			VertexPosTexNorm b = vertices[i + 1];
			VertexPosTexNorm c = vertices[i + 2];
			VertexPosTexNorm ab;
			ab.position = Vector3f::Normalize((a.position + b.position) * 0.5f);
			ab.normal = ab.position;
			ab.texCoord = Vector2f::ZERO;
			VertexPosTexNorm bc;
			bc.position = Vector3f::Normalize((b.position + c.position) * 0.5f);
			bc.normal = bc.position;
			bc.texCoord = Vector2f::ZERO;
			VertexPosTexNorm ac;
			ac.position = Vector3f::Normalize((a.position + c.position) * 0.5f);
			ac.normal = ac.position;
			ac.texCoord = Vector2f::ZERO;
/*
			Vector3f a = vertices[i].position;
			Vector3f b = vertices[i + 1].position;
			Vector3f c = vertices[i + 2].position;
			Vector3f ab = (a + b) * 0.5f;
			Vector3f bc = (b + c) * 0.5f;
			Vector3f ac = (a + c) * 0.5f;
		
			a.Normalize();
			b.Normalize();
			c.Normalize();
			ab.Normalize();
			bc.Normalize();
			ac.Normalize();*/

			newVertices.push_back(a);
			newVertices.push_back(ab);
			newVertices.push_back(ac);
			newVertices.push_back(ab);
			newVertices.push_back(b);
			newVertices.push_back(bc);
			newVertices.push_back(ac);
			newVertices.push_back(bc);
			newVertices.push_back(c);
			newVertices.push_back(ab);
			newVertices.push_back(bc);
			newVertices.push_back(ac);
		}
		vertices = newVertices;
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < vertices.size(); i++)
		indices.push_back(i);

	m_mesh = new Mesh();
	m_mesh->GetVertexData()->BufferVertices((int) vertices.size(), vertices.data());
	m_mesh->GetIndexData()->BufferIndices((int) indices.size(), indices.data());
}

World::~World()
{
	delete m_mesh;
}

// Perform a sphere-ray intersection.
bool World::CastRay(const Ray& ray, float& distance) const
{
	Vector3f worldPos = Vector3f::ZERO;

	// Setup a quadratic equation: ax^2 + bx + c.
	float a = 1.0f;
	float b = 2.0f * ray.direction.Dot(ray.origin - worldPos);
	float c = worldPos.LengthSquared() + ray.origin.LengthSquared() -
		(2.0f * worldPos.Dot(ray.origin)) - (m_radius * m_radius);

	// Solve the quadratic equation.
	float discriminant = b*b - 4*a*c;
	if (discriminant < 0.0f)
	{
		return false;
	}
	else
	{
		distance = (-b - Math::Sqrt(discriminant)) / (2.0f * a);
		return true;
	}
}

