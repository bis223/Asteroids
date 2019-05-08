#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

#include <d3dx9math.h>
#include <list>

class GameEntity;
class Game;
class Collider;
class QuadTree;

class Collision
{
public:
	Collision(Game *game);
	~Collision();

	Collider *CreateCollider(GameEntity *entity);
	void DestroyCollider(Collider *collider);

	void UpdateColliderPosition(Collider *collider, const D3DXVECTOR3 &position);
	void UpdateColliderRadius(Collider *collider, float radius);
	void EnableCollider(Collider *collider);
	void DisableCollider(Collider *collider);
	
	void AddToList(QuadTree *quadtree);
	void RemoveFromList(QuadTree *quadtree);
	void DoCollisions(Game *game) const;	

private:

	typedef std::list<Collider *> ColliderList;
	typedef std::list<QuadTree *> QuadTreeList;

	static bool CollisionTest(Collider *a, Collider *b);

	QuadTree     *screenQuad_;
	ColliderList colliders_;
	QuadTreeList quadTreeList_;

};

#endif // COLLISION_H_INCLUDED
