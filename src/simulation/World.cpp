#include "World.h"
#include <math/MathLib.h>

World::World()
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
	Vector3f v[] = {
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
	for (int i = 0; i < 20 * 3; i++)
		m_vertices.push_back(v[i]);

	// Normalize vertices.
	for (unsigned int i = 0; i < m_vertices.size(); i++)
		m_vertices[i].Normalize();

	// Subdivide.
	for (int k = 0; k < 4; k++)
	{
		std::vector<Vector3f> newVertices;
		for (unsigned int i = 0; i < m_vertices.size(); i += 3)
		{
			Vector3f a = m_vertices[i];
			Vector3f b = m_vertices[i + 1];
			Vector3f c = m_vertices[i + 2];
			Vector3f ab = (a + b) * 0.5f;
			Vector3f bc = (b + c) * 0.5f;
			Vector3f ac = (a + c) * 0.5f;
		
			a.Normalize();
			b.Normalize();
			c.Normalize();
			ab.Normalize();
			bc.Normalize();
			ac.Normalize();

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
		m_vertices = newVertices;
	}
}

World::~World()
{
}

