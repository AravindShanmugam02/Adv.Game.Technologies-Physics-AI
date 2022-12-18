#pragma once
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/GameWorld.h"

using namespace NCL;
using namespace CSC8503;

class Bonus : public GameObject
{
public:
	Bonus(string name) : GameObject(name)
	{

	}
	~Bonus() {}

protected:
	void OnCollisionBegin(GameObject* CollideObject) override
	{
		if (this->GetName() == "Bonus" && this->IsActive() == true && CollideObject->GetName() == "PlayerBall")
		{
			std::cout << "\nBonus Collected !!!";
			CollideObject->score += (rand() % 15 + 1);
			this->DeactivateObj();
			this->GetTransform().SetPosition(Vector3(55, -10, 55));
			//otherObject->GameWorld::RemoveGameObject(mainObject, true);
		}
		/*else if (mainObject->GetName() == "PlayerBall" && otherObject->IsActive() == true)
		{
			std::cout << "\nBonus Collected !!!";
			otherObject->DeactivateObj();
			//handle->RemoveGameObject(mainObject, true);
		}*/
	}
};