#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"

using namespace NCL;
using namespace CSC8503;

Window* w;
TutorialGame* g;
int menuOption;
int menuLayer;
int difficultyLevel;
//int restartLevel;
bool returnToMainMenu;
bool exitGame = false;


/*void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;
	State* A = new State([&](float dt)-> void
		{
			std::cout << "I'm in state A!\n";
			data++;
		}
	);
	State* B = new State([&](float dt)-> void
		{
			std::cout << "I'm in state B!\n";
			data--;
		}
	);
	StateTransition* stateAB = new StateTransition(A, B, [&](void)-> bool
		{
			return data > 10;
		}
	);
	StateTransition* stateBA = new StateTransition(B, A, [&](void)-> bool
		{
			return data < 0;
		}
	);

	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);
	for (int i = 0; i < 100; ++i) {
		testMachine->Update(1.0f);
	}
}*/

// FinalScreen
class FinalScreen :public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		
		if (g->gameStatus == "Won") {
			Debug::Print("You Won The Game !!!", Vector2(10, 05), Vector4(1.0f, 0, 0, 0.6f));
			Debug::Print("FinalScore is :: " + std::to_string(g->finalScore), Vector2(10, 10), Vector4(1.0f, 0, 0, 0.6f));
		}
		else if (g->gameStatus == "Lost") {
			Debug::Print("You Lost The Game !!!", Vector2(10, 05), Vector4(1.0f, 0, 0, 0.6f));
			Debug::Print("FinalScore is :: " + std::to_string(g->finalScore), Vector2(10, 10), Vector4(1.0f, 0, 0, 0.6f));
		}
		

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
		{
			menuOption += 1;
			if (menuOption > 2) { menuOption = 0; }
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
		{
			menuOption -= 1;
			if (menuOption < 0) { menuOption = 2; }
		}

		//Menu Display
		switch (menuOption) {
				//Restart
			case 0: {
				Debug::Print("Restart  <:::::>", Vector2(50, 30));
				Debug::Print("Go To MainMenu", Vector2(50, 35));
				Debug::Print("Exit", Vector2(50, 40));
			}break;
				//MainMenu
			case 1: {
				Debug::Print("Restart", Vector2(50, 30));
				Debug::Print("Go To MainMenu  <:::::>", Vector2(50, 35));
				Debug::Print("Exit", Vector2(50, 40));
			}break;
				//Exit
			case 2: {
				Debug::Print("Restart", Vector2(50, 30));
				Debug::Print("Go To MainMenu", Vector2(50, 35));
				Debug::Print("Exit  <:::::>", Vector2(50, 40));
			}break;
				//Default
			default: {
				//std::cout << "Default  <:::::>\n";
			}break;
		}
		//Menu Selection
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
		{
			switch (menuOption)
			{
					//Restart
				case 0: {
					menuOption = 0;
					g = new TutorialGame(difficultyLevel);
					//std::cout << "Restart  <:::::>\n";
					return PushdownResult::Pop;
				}break;
					//MainMenu
				case 1: {
					menuOption = 0;
					returnToMainMenu = true;
					//std::cout << "Go To MainMenu  <:::::>\n";
					return PushdownResult::Pop;
				}break;
					//Exit
				case 2: {
					menuOption = 0;
					exitGame = true;
					//std::cout << "Exit  <:::::>\n";
					return PushdownResult::Pop;
				}break;
					//Default
				default: {
					//std::cout << "Default  <:::::>\n";
				}break;
			}
		}
		g->UpdateMenu(dt);
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		//std::cout << "FinalScreen\n";
		menuOption = 0;
	}
};

// PauseScreen
class PauseScreen :public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {

		/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)) {
			std::cout << "Game Resumed !!!\n";
			return PushdownResult::Pop;
		}*/

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			//std::cout << "Already in PauseScreen !!!\n";
			//std::cout << "Game Resumed !!!\n";
			return PushdownResult::Pop;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
		{
			menuOption += 1;
			if (menuOption > 1) { menuOption = 0; }
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
		{
			menuOption -= 1;
			if (menuOption < 0) { menuOption = 1; }
		}

		//Menu Display
		switch (menuOption) {
				//Start
			case 0: {
				Debug::Print("Restart  <:::::>", Vector2(20, 20));
				Debug::Print("Go To MainMenu", Vector2(20, 25));
			}break;
				//Exit
			case 1: {
				Debug::Print("Restart", Vector2(20, 20));
				Debug::Print("Go To MainMenu  <:::::>", Vector2(20, 25));
			}break;
				//Default
			default: {
				//std::cout << "Default  <:::::>\n";
			}break;
		}
		//Menu Selection
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
		{
			switch (menuOption)
			{
					//Restart
				case 0: {
					menuOption = 0;
					g = new TutorialGame(difficultyLevel);
					//std::cout << "Restart  <:::::>\n";
					return PushdownResult::Pop;
				}break;
					//Exit
				case 1: {
					menuOption = 0;
					returnToMainMenu = true;
					//std::cout << "Go To MainMenu  <:::::>\n";
					return PushdownResult::Pop;
				}break;
					//Default
				default: {
					//std::cout << "Default  <:::::>\n";
				}break;
			}
		}
		g->UpdateMenu(dt);
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		//std::cout << "Press Esc to unpause game!\n";
		menuOption = 0;
	}
};

// GameScreen
class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		
		/*pauseReminder -= dt;
		if (pauseReminder < 0) {
			std::cout << "Coins mined :" << coinsMined << "\n";
			std::cout << "Press P to pause game, or F1 to return to main menu !\n";
				pauseReminder += 1.0f;
		}*/

		if (g->gameStatus == "Won" || g->gameStatus == "Lost") {
			*newState = new FinalScreen();
			return PushdownResult::Push;
		}

		if (returnToMainMenu == true)
		{
			returnToMainMenu = false;
			//std::cout << "Returning to main menu !\n";
			return PushdownResult::Pop;
		}

		if (exitGame == true)
		{
			//std::cout << "Exiting Game !\n";
			return PushdownResult::Pop;
		}

		/*if (restartLevel != 3)
		{
			std::cout << "Restarting Game !\n";
			return PushdownResult::Pop;
		}*/

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;
		}

		/*if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
			std::cout << "Returning to main menu !\n";
			return PushdownResult::Pop;
		}*/

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		/*if (rand() % 7 == 0) {
			coinsMined++;
		}*/
		g->UpdateGame(dt);
		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		//std::cout << "Entered To GameScreen!!\n";
		g->gameStatus = "InGame";
	}
protected:
	//int coinsMined = 0;
	//float pauseReminder = 1;
};

// Intro Screen
class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			*newState = new GameScreen();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}*/

		/*if (restartLevel != 3) {
			if (restartLevel == 0) {
				restartLevel = 3;
				g = new TutorialGame(0);
				difficultyLevel = 0;
				*newState = new GameScreen();
				return PushdownResult::Push;
			}
			else if (restartLevel == 1) {
				restartLevel = 3;
				g = new TutorialGame(1);
				difficultyLevel = 1;
				*newState = new GameScreen();
				return PushdownResult::Push;
			}
			else if (restartLevel == 2) {
				restartLevel = 3;
				g = new TutorialGame(2);
				difficultyLevel = 2;
				*newState = new GameScreen();
				return PushdownResult::Push;
			}
		}*/

		if (exitGame == true)
		{
			exitGame = false;
			//std::cout << "Game Exit !!!\n";
			return PushdownResult::Pop;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}
		//w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		Debug::Print("WELCOME TO THE GAME!!!", Vector2(10, 05), Vector4(1.0f, 0, 0, 0.6f));
		Debug::Print("Select an option ::", Vector2(10, 10), Vector4(1.0f, 0, 0, 0.6f));

		
		if (menuLayer == 0)
		{
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
			{
				menuOption += 1;
				if (menuOption > 1) { menuOption = 0; }
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
			{
				menuOption -= 1;
				if (menuOption < 0) { menuOption = 1; }
			}

			//Menu Display
			switch (menuOption) {
					//Start
				case 0: {
					Debug::Print("Start  <:::::>", Vector2(20, 20));
					Debug::Print("Exit", Vector2(20, 25));
				}break;
					//Exit
				case 1: {
					Debug::Print("Start", Vector2(20, 20));
					Debug::Print("Exit  <:::::>", Vector2(20, 25));
				}break;
					//Default
				default: {
					//std::cout << "Default  <:::::>\n";
				}break;
			}
			//Menu Selection
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
			{
				switch (menuOption)
				{
						//Start
					case 0: {
						menuLayer = 1;
						menuOption = 0;
						//std::cout << "Welcome to MenuSelection case0\n";
					}break;
						//Exit
					case 1: {
						//std::cout << "Welcome to MenuSelection case1\n";
						return PushdownResult::Pop;
					}break;
						//Default
					default: {
						//std::cout << "Default  <:::::>\n";
					}break;
				}
			}
		}
		else if (menuLayer == 1)
		{
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
			{
				menuOption += 1;
				if (menuOption > 3) { menuOption = 0; }
			}

			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
			{
				menuOption -= 1;
				if (menuOption < 0) { menuOption = 3; }
			}

			//Menu Display
			switch (menuOption)
			{
					//Easy
				case 0: {
					Debug::Print("Easy  <:::::>", Vector2(20, 20));
					Debug::Print("Normal", Vector2(20, 25));
					Debug::Print("Hard", Vector2(20, 30));
					Debug::Print("Go Back", Vector2(20, 35));
				}break;
					//Normal
				case 1: {
					Debug::Print("Easy", Vector2(20, 20));
					Debug::Print("Normal  <:::::>", Vector2(20, 25));
					Debug::Print("Hard", Vector2(20, 30));
					Debug::Print("Go Back", Vector2(20, 35));
				}break;
					//Hard
				case 2: {
					Debug::Print("Easy", Vector2(20, 20));
					Debug::Print("Normal", Vector2(20, 25));
					Debug::Print("Hard  <:::::>", Vector2(20, 30));
					Debug::Print("Go Back", Vector2(20, 35));
				}break;
					//Go Back
				case 3: {
					Debug::Print("Easy", Vector2(20, 20));
					Debug::Print("Normal", Vector2(20, 25));
					Debug::Print("Hard", Vector2(20, 30));
					Debug::Print("Go Back  <:::::>", Vector2(20, 35));
				}break;
					//Default
				default: {
					//std::cout << "Default  <:::::>\n";
				}break;
			}

			//Menu Selection
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
			{
				switch (menuOption)
				{
						//Easy
					case 0: {
						//std::cout << "Welcome to MenuSelection case0\n";
						g = new TutorialGame(0);
						difficultyLevel = 0;
						*newState = new GameScreen();
						return PushdownResult::Push;
					}break;
						//Normal
					case 1: {
						//std::cout << "Welcome to MenuSelection case1\n";
						g = new TutorialGame(1);
						difficultyLevel = 1;
						*newState = new GameScreen();
						return PushdownResult::Push;
					}break;
						//Hard
					case 2: {
						std::cout << "YET TO BE UNLOCKED -- UNDER DEVELOPMENT!!! \n";
						//g = new TutorialGame(2);
						//difficultyLevel = 2;
						//*newState = new GameScreen();
						//return PushdownResult::Push;
					}break;
						//Go Back
					case 3: {
						menuLayer = 0;
						menuOption = 0;
						//std::cout << "Welcome to MenuSelection case3\n";
					}break;
						//Default
					default: {
						//std::cout << "Default  <:::::>\n";
					}break;
				}
			}
		}
		g->UpdateMenu(dt);
		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		//std::cout << "Welcome to a really awesome game !\n";
		//std::cout << "Press Space To Begin or escape to quit !\n";
		menuLayer = 0;
		menuOption = 0;
		difficultyLevel = 0;
		//restartLevel = 3;
		returnToMainMenu = false;
		g = new TutorialGame(3);
	}
};

/*void TestPushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (!machine.Update(dt)) {
			return;
		}
	}
}*/

/*void PushdownAutomataHandleFunction(Window* w)
{
	g = new TutorialGame(0);
	PushdownMachine machine(new IntroScreen());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (!machine.Update(dt)) {
			return;
		}
	}
}*/


/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main() {
	w = Window::CreateGameWindow("CW - Aravind Shanmugam - C1036775", 1900, 1030, false, 10, 10);

	if (!w->HasInitialised()) {
		return -1;
	}	

	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	w->SetTitle("Status: Loading... | CW - Aravind Shanmugam - C1036775");

	//TestPushdownAutomata(w);
	//PushdownAutomataHandleFunction(w);

	//TutorialGame* g = new TutorialGame();
	//w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	/*while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		//g->UpdateGame(dt);
		//TestStateMachine();
	}*/

	//g = new TutorialGame(3);

	float deltaTime = 0.0f, fps = 0.0f;
	int frames = 0;
	auto prevTime = std::chrono::high_resolution_clock::now(); //can also use steady_time instead of high_resolution_clock.

	PushdownMachine machine(new IntroScreen());
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			//std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (!machine.Update(dt)) {
			return 0;
		}

		++frames;
		deltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - prevTime)).count(); //count() converts this milliseconds values to float.

		if (deltaTime >= 500.0f) { //deltaTime is in milliseconds, convert it to seconds.
			fps = double(frames / ((deltaTime / 1000.0f) * 2));
			frames = 0;
			prevTime = std::chrono::high_resolution_clock::now();
		}
		w->SetTitle("Status: Running! | FPS: "+ std::to_string(fps) + " | CW - Aravind Shanmugam - C1036775");
	}
	w->SetTitle("Status: Exiting! | CW - Aravind Shanmugam - C1036775");
	Window::DestroyGameWindow();
}