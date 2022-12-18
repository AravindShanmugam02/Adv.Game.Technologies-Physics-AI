#pragma once
#include <stack>

namespace NCL {
	namespace CSC8503 {
		class PushdownState;

		class PushdownMachine
		{
		public:
			PushdownMachine(PushdownState* initialState) {
				this->initialState = initialState;
			}
			~PushdownMachine() {}

			/*PushdownMachine();
			~PushdownMachine();*/

			bool Update(float dt);
			/*void Update();*/

		protected:
			PushdownState* activeState;
			PushdownState* initialState;

			std::stack<PushdownState*> stateStack;
		};
	}
}

