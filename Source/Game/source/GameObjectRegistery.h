#pragma once
#include "GameObject.h"
#include <memory>
#include <vector>
class GameObjectRegistery
{
public:
	~GameObjectRegistery() = default;
	static GameObjectRegistery& Get() { static GameObjectRegistery myInstance; return myInstance;}
	const void AddGameObject(std::shared_ptr<GameObject> aGameObject);
	std::shared_ptr<GameObject> GetGameObject(int aID);
	std::vector<std::shared_ptr<GameObject>> GetAllGameObject();
	bool HasGameObjectCollided();
	std::vector<int> GetCollidedGameObjects();
	std::vector<int> GetPreviousGameObjects() { return PreviousHitID; }
	void InitHitBoxCommand() { PreviousHitID.push_back(0); PreviousHitID.push_back(0); return; }
	
private:
	std::vector<std::shared_ptr<GameObject>> myGameObjects;
	std::vector<int> PreviousHitID;
};

