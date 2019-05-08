#include "QuadTree.h"
#include "Collider.h"
#include "Collision.h"
#include "Game.h"
#include <list>

QuadTree::QuadTree(const int nodeIndex, const Rectangle &rect, Collision * col) : nodeIndex_(nodeIndex), rectangle_(rect), collisionSystem_(col)
{
	nodes_[0] = nullptr;
	nodes_[1] = nullptr;
	nodes_[2] = nullptr;
	nodes_[3] = nullptr;
}

void QuadTree::Clear()
{
	objects_.clear();

	for (int i = 0; i < 4; i++)
	{
		if(nodes_[i])
		{
			nodes_[i]->Clear();
			collisionSystem_->RemoveFromList(nodes_[i]);
			delete nodes_[i];
			nodes_[i] = nullptr;
		}
		
	}
}

void QuadTree::Split()
{

	float halfWidth = rectangle_.width/2;
	float halfHeight = rectangle_.height/2;
	float x = rectangle_.x;
	float y = rectangle_.y;

	//Spliting node into 4 child nodes
	for (int i = 0; i < 4; i++)
	{
		Rectangle rect;
		if     (i == 0) {rect.x = x + halfWidth; rect.y = y + halfHeight;}
		else if(i == 1)	{rect.x = x - halfWidth; rect.y = y + halfHeight;}
		else if(i == 2)	{rect.x = x - halfWidth; rect.y = y - halfHeight;}
		else if(i == 4)	{rect.x = x + halfWidth; rect.y = y - halfHeight;}

		rect.width = halfWidth;
		rect.height = halfHeight;
		nodes_[i] = new QuadTree(nodeIndex_ + 1, rect, collisionSystem_);		
		collisionSystem_->AddToList(nodes_[i]);
	}
}

int QuadTree::GetIndex(Collider *collider)
{
	int index            = -1;
	float rectHalfWidth  = rectangle_.width/2;
	float rectHalfHeight = rectangle_.height/2;
	float radius         = collider->GetRadius();

	bool isInTop    = collider->GetPosition().y - radius > rectangle_.y 
					    && collider->GetPosition().y + radius < rectangle_.y + rectHalfHeight; //Completely fits top quadrant
	bool isInBottom = collider->GetPosition().y + radius < rectangle_.y 
						&& collider->GetPosition().y - radius > rectangle_.y - rectHalfHeight; //Completely fits bottom quadrant
	bool isInLeft   = collider->GetPosition().x + radius < rectangle_.x 
						&& collider->GetPosition().x - radius > rectangle_.x - rectHalfWidth;  //Completely fits left quadrant
	bool isInRight  = collider->GetPosition().x - radius > rectangle_.x 
						&& collider->GetPosition().x + radius < rectangle_.x + rectHalfWidth;  //Completely fits right quadrant


	// Finding the node index where the object belong based on its position. -1 means the object is occupying more than one quardrant.
	if(isInTop    && isInRight) {index = 0;}
	if(isInTop    && isInLeft)  {index = 1;}
	if(isInBottom && isInLeft)  {index = 2;}
	if(isInBottom && isInRight) {index = 3;}

	return index;
}

void QuadTree::Insert(Collider* collider)
{
	if(nodes_[0] != nullptr) //A null node indicates its a leaf node. Base of recursion.
	{
		int index = GetIndex(collider);
		
		if(index != -1)
		{
			nodes_[index]->Insert(collider);
			return;
		}
	}

	objects_.push_back(collider);


	if(objects_.size() > MAX_OBJECTS && nodeIndex_ < MAX_LEVELS)
	{
		if(nodes_[0] == nullptr)
		{
			Split(); // If a quardrant more objects than maximum, it will split into 4 child nodes.
		}


		//After the change in child node, the objects has to be reinserted to respective nodes.
		std::list<Collider *>::const_iterator colliderIt  = objects_.begin();
		
		while(colliderIt != objects_.end())
		{		
			int index = GetIndex(*colliderIt);
			if(index != -1)
			{
				nodes_[index]->Insert(*colliderIt);
				colliderIt = objects_.erase(colliderIt);
			}
			else
			{
				++colliderIt;
			}
		}
	}

}

void QuadTree::GetCollidableList(std::list<Collider* > &resolvedColliders, Collider* collider)
{
	int index = GetIndex(collider);
	
	if(index != -1 && nodes_[0] != nullptr)
	{
		nodes_[index]->GetCollidableList(resolvedColliders, collider);
	}	
	resolvedColliders.remove(collider); // removing the collider as it can't collide with itself
	resolvedColliders.insert(std::end(resolvedColliders), std::begin(objects_), std::end(objects_));
}
