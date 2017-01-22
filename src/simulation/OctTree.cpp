#include "OctTree.h"


OctTreeNode::OctTreeNode()
{
	for (unsigned int i = 0; i < 8; i++)
		m_children[i] = nullptr;
}


unsigned int OctTreeNode::GetChildIndex(const Vector3f& point)
{
	int index = 0;
	if (point.x > m_position.x)
		index |= 0x1;
	if (point.y > m_position.y)
		index |= 0x2;
	if (point.z > m_position.z)
		index |= 0x4;
	return index;
}

OctTreeNode::object_list::iterator OctTreeNode::objects_begin()
{
	return m_objects.begin();
}

OctTreeNode::object_list::iterator OctTreeNode::objects_end()
{
	return m_objects.end();
}

