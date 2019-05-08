#include "Collision.h"
#include "Collider.h"
#include "Game.h"
#include "QuadTree.h"
#include <functional>

Collision::Collision(Game *game)
{
	QuadTree::Rectangle r;
	r.x = 0;
	r.y = 0;
	r.width = game->GetScreenSize().width;
	r.height = game->GetScreenSize().height;
	screenQuad_ = new QuadTree(0, r, this);
	AddToList(screenQuad_);
}

Collision::~Collision()
{
	for (ColliderList::iterator colliderIt = colliders_.begin(), end = colliders_.end();
		colliderIt != end;
		++colliderIt)
	{
		delete *colliderIt;
	}
	colliders_.clear();

	for (QuadTreeList::iterator quadTreeIt = quadTreeList_.begin(), end = quadTreeList_.end();
		quadTreeIt != end;
		++quadTreeIt)
	{
		delete (*quadTreeIt);
	}
	quadTreeList_.clear();
}

Collider *Collision::CreateCollider(GameEntity *entity)
{
	Collider *collider = new Collider();

	collider->position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	collider->radius = 0.0f;
	collider->entity = entity;
	collider->enabled = true;
	colliders_.push_back(collider);

	return collider;
}

void Collision::DestroyCollider(Collider *collider)
{
	colliders_.remove_if(std::bind1st((std::equal_to<Collider *>()), collider));
	delete collider;
}

void Collision::UpdateColliderPosition(Collider *collider, const D3DXVECTOR3 &position)
{
	collider->position = position;
}

void Collision::UpdateColliderRadius(Collider *collider, float radius)
{
	collider->radius = radius;
}

void Collision::EnableCollider(Collider *collider)
{
	collider->enabled = true;
}

void Collision::DisableCollider(Collider *collider)
{
	collider->enabled = false;
}

void Collision::AddToList(QuadTree *quadtree)
{
	quadTreeList_.push_back(quadtree);
}

void Collision::RemoveFromList(QuadTree *quadtree)
{	
	quadTreeList_.remove(quadtree);
}

void Collision::DoCollisions(Game *game) const
{
	screenQuad_->Clear();

	for (ColliderList::const_iterator colliderIt = colliders_.begin(), end = colliders_.end();
	colliderIt != end;
	++colliderIt)
	{
		screenQuad_->Insert(*colliderIt); // Updating quadtree.
	}
	
	std::list<std::list<GameEntity *>> collidedEntities;
	std::list<GameEntity *> collidedPairs;
	ColliderList resolvedColliders;

	for (QuadTreeList::const_iterator quadTreeIt = quadTreeList_.begin(), end = quadTreeList_.end();
		quadTreeIt != end;
		++quadTreeIt)
	{
		resolvedColliders.clear();
		resolvedColliders = (*quadTreeIt)->GetObjects(); // Collision Optimization: Collision is checked between objects lying inside same quadrant.
														 //	Objects lying far to each other will not be tested for collision.
		for (ColliderList::const_iterator colliderAIt = resolvedColliders.begin(), end = resolvedColliders.end();
			colliderAIt != end;
			++colliderAIt)
		{
		
			ColliderList::const_iterator colliderBIt = colliderAIt;
			for (++colliderBIt; colliderBIt != end; ++colliderBIt)
			{
				Collider *colliderA = *colliderAIt;
				Collider *colliderB = *colliderBIt;
			// BUG FIXED: Adding collided objects to a temporay list to perform collision logic and deletion. 
			//			  After a successfull collision upto two or no deletion can occur.

				if (CollisionTest(colliderA, colliderB))  
				{

					collidedPairs.push_back(colliderA->entity);	
					collidedPairs.push_back(colliderB->entity);
					
				}
			}
			if(collidedPairs.size() > 0)
			{
				collidedEntities.push_back(collidedPairs);
				collidedPairs.clear();
			}
		}
		
	}

	while (!collidedEntities.empty())
	{
		collidedPairs = collidedEntities.front();
		collidedEntities.pop_front();
		GameEntity *collidedEntityA = collidedPairs.front();
		collidedPairs.pop_front();
		GameEntity *collidedEntityB = collidedPairs.front();
		collidedPairs.pop_front();
		game->DoCollision(collidedEntityA, collidedEntityB);
	}
}

bool Collision::CollisionTest(Collider *a, Collider *b)
{
	if (a->enabled == false)
		return false;
	if (b->enabled == false)
		return false;

	D3DXVECTOR3 diff;
	D3DXVec3Subtract(&diff, &a->position, &b->position);
	float distance = D3DXVec3Length(&diff);
	if (distance < (a->radius + b->radius))
	{
		return true;
	}

	return false;
}
