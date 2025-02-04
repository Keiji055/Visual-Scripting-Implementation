#include "GameObjectRegistery.h"



const void GameObjectRegistery::AddGameObject(std::shared_ptr<GameObject> aGameObject)
{
	myGameObjects.push_back(aGameObject);
}

std::shared_ptr<GameObject> GameObjectRegistery::GetGameObject(int aID)
{
	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		if (myGameObjects[i]->GetID() == aID)
		{
			return myGameObjects[i];
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<GameObject>> GameObjectRegistery::GetAllGameObject()
{
	return myGameObjects;
}

bool GameObjectRegistery::HasGameObjectCollided()
{
	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		for (size_t j = i + 1; j < myGameObjects.size(); j++)
		{
			if (myGameObjects[i]->GetHitBox()->Intersect(myGameObjects[j]->GetHitBox()))
			{
				return true;
			}
		}

	}
	return false;
}

std::vector<int> GameObjectRegistery::GetCollidedGameObjects()
{
	std::vector<int> myCollidedGameObjectId;
	for (size_t i = 0; i < myGameObjects.size(); i++)
	{
		for (size_t j = i + 1; j < myGameObjects.size(); j++)
		{
			if (myGameObjects[i]->GetHitBox()->Intersect(myGameObjects[j]->GetHitBox()))
			{
				myCollidedGameObjectId.push_back(myGameObjects[i]->GetID());
				myCollidedGameObjectId.push_back(myGameObjects[j]->GetID());
				PreviousHitID[0] = myCollidedGameObjectId[0];
				PreviousHitID[1] = myCollidedGameObjectId[1];
			}
		}

	}
	return myCollidedGameObjectId;
}

