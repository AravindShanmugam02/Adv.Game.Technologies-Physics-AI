#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/GameWorld.h"

using namespace NCL;
using namespace CSC8503;

class Killer : public GameObject
{
public:
	Killer(string name) : GameObject(name)
	{

	}
	~Killer() {}

protected:
	void OnCollisionBegin(GameObject* CollideObject) override
	{
		if (this->GetName() == "Killer" && this->IsActive() == true && CollideObject->GetName() == "PlayerBall")
		{
			std::cout << "\nPlayer Killed !!!";
			CollideObject->TouchDownFalse();
			CollideObject->GameOverTrue();
			CollideObject->DeactivateObj();
		}
	}
};