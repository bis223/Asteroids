#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include "GameEntity.h"
#include <list>

class QuadTree
{
public:
	struct Rectangle
	{
		float x, y;
		float width;
		float height;
	};

	QuadTree(const int nodeIndex, const Rectangle &rect, Collision * collisionSystem);

	typedef std::list<Collider *> ColliderList;

	void Clear();
	void Split();
	int  GetIndex(Collider* collider);
	void Insert(Collider* collider);
	void GetCollidableList(std::list<Collider* > &resolvedColliders, Collider* collider);
	ColliderList GetObjects() const {return objects_;} 

private:
	int          nodeIndex_; 
	ColliderList objects_;	 //The number of objects present inside this quad.
	Rectangle    rectangle_; 
	QuadTree*	 nodes_[4];  // 4 child nodes. Will be null if its a leaf node.
	Collision*   collisionSystem_;
	enum { MAX_OBJECTS = 10, MAX_LEVELS = 5 }; // Maximum num of objects a quad will hold before splitting to child nodes. Max level of child nodes.

};

#endif // QUADTREE_H_INCLUDED