#pragma once
#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/GameWorld.h"

using namespace NCL;
using namespace CSC8503;

class VictoryCube : public GameObject
{
public:
	VictoryCube(string name) : GameObject(name)
	{

	}
	~VictoryCube() {}

protected:
	void OnCollisionBegin(GameObject* CollideObject) override
	{
		if (this->GetName() == "Victory" && this->IsActive() == true && CollideObject->GetName() == "PlayerBall")
		{
			std::cout << "\nPlayer Won !!!";
			CollideObject->TouchDownTrue();
			CollideObject->GameOverFalse();
			std::cout << "\nTouchDown :: " << CollideObject->IsTouchDown() << "\nGameover :: " << CollideObject->IsGameOver();
		}
	}
};