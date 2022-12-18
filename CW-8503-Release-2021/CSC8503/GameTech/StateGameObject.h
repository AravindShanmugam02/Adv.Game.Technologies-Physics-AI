#pragma once
#include "..\CSC8503Common\GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class StateGameObject :public GameObject {
		public:
			StateGameObject();
			~StateGameObject();

			virtual void Update(float dt);

		protected:
			void MoveLeft(float dt);
			void MoveRight(float dt);

			StateMachine* stateMachine;
			float counter;
			void OnCollisionBegin(GameObject* CollideObject) override {
				if (CollideObject->GetName() == "PlayerBall")
				{
					std::cout << "\nBonus Collected !!!";
					CollideObject->score += (rand() % 15 + 1);
					//->DeactivateObj();
					//otherObject->GameWorld::RemoveGameObject(mainObject, true);
				}
			}
		};
	}
}