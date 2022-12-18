#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame(int difficulty)	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true; // By default the Gravity is applied
	inSelectionMode = false;
	plane = false;
	finalScore = 0;
	gameStatus = "GameRunning";
	difficultyLevel = difficulty;

	Debug::SetRenderer(renderer);

	InitialiseAssets(difficultyLevel);
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets(int difficulty) {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");


	if (difficulty == 3) {
		InitMenu();
	}
	else if (difficulty == 2) {
		InitWorldHard();
	}
	else if (difficulty == 1) {
		InitWorldNormal();
	}
	else if (difficulty == 0) {
		InitWorldEasy();
	}
}

void TutorialGame::ClearResetGame()
{
	world->ClearAndErase();
	physics->Clear();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

// Menu
void TutorialGame::InitMenu() {
	ClearResetGame();
	InitMenuCamera();
}

void TutorialGame::UpdateMenu(float dt)
{
	//MenuCameraUpdate(dt);
	InitMenuCamera();
	renderer->Update(dt);
	Debug::FlushRenderables(dt);
	renderer->Render();
}

// Initiate World -----------------------------------------------------------------------------------------
void TutorialGame::InitWorldEasy() {
	ClearResetGame();
	InitCamera();
	InitCommonObj();
	//HangingConstraints("HangingConstraints");
	//InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	//InitGameExamples();
	//InitGameBlock();
	
	//InitBonusCoin("Bonus", 5, Vector3(1, 2, 1));
	//InitOBBCube();

	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0), 1.0f, 1.0f, 1.0f);
}

void TutorialGame::InitWorldNormal() {
	ClearResetGame();
	InitCamera();
	InitCommonObj();
}

void TutorialGame::InitWorldHard() {
	ClearResetGame();
	InitCamera();
}

// Camera -------------------------------------------------------------------------------------------------
void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 10, 0)); //Vector3(-60, 40, 60)
	lockedObject = nullptr;
}

void TutorialGame::InitMenuCamera()
{
	InitCamera();
	world->GetMainCamera()->SetPosition(Vector3(40, 50, 2)); //Vector3(-60, 40, 60)
}

void TutorialGame::MenuCameraUpdate(float dt) {
	//world->GetMainCamera()->SetPosition(Vector3(cos(dt), 10, cos(dt)));
	world->GetMainCamera()->UpdateCamera(dt);
}

//Initiate Common Objects ----------------------------------------------------------------------------------
void TutorialGame::InitCommonObj()
{
	InitFloorCeiling("AABB-Floor&Ceiling");
	InitPlayerSphere("PlayerBall");
	InitHangingConstraints("HangingConstraints", 3, Vector3(-3, 10.0f, -3));
	InitHangingConstraints("HangingConstraints", 3, Vector3(-6, 10.0f, 6));
	InitHangingConstraints("HangingConstraints", 3, Vector3(6, 10.0f, -6));
	InitOBBCube();
	InitSpinningCapsules();
	InitStatefulObjects();
	InitBonusCoin("Bonus", 5);

	//For Normal Mode
	if (difficultyLevel == 1) {
		InitKillerObjsOfNormalMode();
	}
}

// Floor-Ceiling -------------------------------------------------------------------------------------------
void TutorialGame::InitFloorCeiling(string name) {
	Floor = AddFloorToWorld(name, Vector3(0, -2, 0));
	Floor->GetPhysicsObject()->SetElasticity(0.0f);
	Ceiling = AddFloorToWorld(name, Vector3(0, 12, 0));
	Ceiling->GetPhysicsObject()->SetElasticity(1.0f);
}

// Player Sphere ---------------------------------------------------------------------------------------------
void TutorialGame::InitPlayerSphere(string name) {
	playerKey = AddSphereToWorld(name, Vector3(-45.0f, 1.0f, 45.0f), 1.0f, 1.0f);
	playerKey->GetPhysicsObject()->SetElasticity(1.0f);
	//lockedObject = playerHandle;
}

// Hanging Constraints --------------------------------------------------------------------------------------
void TutorialGame::InitHangingConstraints(string name, int num, const Vector3& pos) {
	Vector3 position = pos;
	for (int i = 0; i < num; i++) {
		BuildHangingConstraints(name, position);
		position += Vector3(1.5f, 0, 1.5f);
	}
}

// Initiate OBB Cubes ----------------------------------------------------------------------------------------
void TutorialGame::InitOBBCube()
{
	SolidOBBWall1 = AddOBBCubeToWorld(Vector3(0, 5, 50), Vector3(35.0f, 5.0f, 10.0f), 0.0f);
	SolidOBBWall1->GetTransform().SetScale(Vector3(70, 10, 20));
	SolidOBBWall1->GetPhysicsObject()->SetElasticity(1.0f);
	SolidOBBWall2 = AddOBBCubeToWorld(Vector3(0, 5, -50), Vector3(35.0f, 5.0f, 10.0f), 0.0f);
	SolidOBBWall2->GetTransform().SetScale(Vector3(70, 10, 20));
	SolidOBBWall2->GetPhysicsObject()->SetElasticity(1.0f);
	SolidOBBWall3 = AddOBBCubeToWorld(Vector3(50, 5, 0), Vector3(35.0f, 5.0f, 25.0f), 0.0f);
	SolidOBBWall3->GetTransform().SetScale(Vector3(70, 10, 50));
	SolidOBBWall3->GetPhysicsObject()->SetElasticity(1.0f);
	SolidOBBWall4 = AddOBBCubeToWorld(Vector3(-50, 5, 0), Vector3(35.0f, 5.0f, 25.0f), 0.0f);
	SolidOBBWall4->GetTransform().SetScale(Vector3(70, 10, 50));
	SolidOBBWall4->GetPhysicsObject()->SetElasticity(1.0f);

	Victory = AddVictoryCubeToWorld("Victory", Vector3(45.0f, 8.0f, -45.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f);
	Victory->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(90, Vector3(0, 0, 1))));
}

// Initiate Spinning Capsules -------------------------------------------------------------------------------
void TutorialGame::InitSpinningCapsules() {
	if (difficultyLevel == 0) {
		SolidSpinningCapsule1 = AddCapsuleToWorld(Vector3(-12, 5, 32), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule1->GetPhysicsObject()->SetElasticity(1.0f);
		SolidSpinningCapsule2 = AddCapsuleToWorld(Vector3(-38, 5, -32), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule2->GetPhysicsObject()->SetElasticity(1.0f);
		SolidSpinningCapsule3 = AddCapsuleToWorld(Vector3(42, 5, -37), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule3->GetPhysicsObject()->SetElasticity(1.0f);
		SolidSpinningCapsule4 = AddCapsuleToWorld(Vector3(42, 5, 32), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule4->GetPhysicsObject()->SetElasticity(1.0f);
	}
	else if (difficultyLevel == 1) {
		SolidSpinningCapsule1 = AddCapsuleToWorld(Vector3(-12, 5, 32), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule1->GetPhysicsObject()->SetElasticity(1.0f);
		SolidSpinningCapsule2 = AddCapsuleToWorld(Vector3(-38, 5, -32), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule2->GetPhysicsObject()->SetElasticity(1.0f);
		SolidSpinningCapsule3 = AddCapsuleToWorld(Vector3(42, 5, -37), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule3->GetPhysicsObject()->SetElasticity(1.0f);
		SolidSpinningCapsule4 = AddCapsuleToWorld(Vector3(42, 5, 32), 3.5f, 1.0f, 0.0f);
		SolidSpinningCapsule4->GetPhysicsObject()->SetElasticity(1.0f);
	}
}

// initiate Stateful Objects ---------------------------------------------------------------------------------
void TutorialGame::InitStatefulObjects() {
	
		StatefulBlock1 = AddStateObjectSphereToWorld("StatefulBlock", Vector3(0, 3.2, 15), 1.5f, 0.5f);
		StatefulBlock1->GetPhysicsObject()->SetElasticity(0.0f);
		StatefulBlock2 = AddStateObjectSphereToWorld("StatefulBlock", Vector3(0, 3.2, -15), 1.5f, 0.5f);
		StatefulBlock2->GetPhysicsObject()->SetElasticity(0.0f);
		
	if (difficultyLevel == 1) {
		//StatefulBlock3;
		//StatefulBlock4;
	}
}

// Initiate Bonus Coins --------------------------------------------------------------------------------------
void TutorialGame::InitBonusCoin(string name, const int& numOf)
{
	AddBonusCoinToWorld(name, Vector3(-32, 2, 32));
	AddBonusCoinToWorld(name, Vector3(47, 2, 34));
	AddBonusCoinToWorld(name, Vector3(0, 2, -20));
	AddBonusCoinToWorld(name, Vector3(-47, 2, -31));
	AddBonusCoinToWorld(name, Vector3(39, 2, -48));
}

// Initiate Killer Objects -----------------------------------------------------------------------------------
void TutorialGame::InitKillerObjsOfNormalMode()
{
	KillerSphere1 = AddKillerSphereToWorld("Killer", Vector3(-10.5f, 5, 4), 1.0f, 0.0f);
	KillerSphere1->GetPhysicsObject()->SetElasticity(0.0f);
	KillerSphere1->GetRenderObject()->SetColour(Vector4(255, 0, 0, 0.5f));

	KillerSphere2 = AddKillerSphereToWorld("Killer", Vector3(9, 5, -21), 1.0f, 0.0f);
	KillerSphere2->GetPhysicsObject()->SetElasticity(0.0f);
	KillerSphere2->GetRenderObject()->SetColour(Vector4(255, 0, 0, 0.5f));

	KillerSphere3 = AddKillerSphereToWorld("Killer", Vector3(17, 3, -33), 1.0f, 0.0f);
	KillerSphere3->GetPhysicsObject()->SetElasticity(0.0f);
	KillerSphere3->GetRenderObject()->SetColour(Vector4(255, 0, 0, 0.5f));

	KillerCube1 = AddKillerCubeToWorld("Killer", Vector3(10.5f, 5, 4), Vector3(1.0f, 1.0f, 1.0f), 0.0f);
	KillerCube1->GetPhysicsObject()->SetElasticity(0.0f);
	KillerCube1->GetRenderObject()->SetColour(Vector4(255, 0, 0, 0.5f));

	KillerCube2 = AddKillerCubeToWorld("Killer", Vector3(-9, 5, -21), Vector3(1.0f, 1.0f, 1.0f), 0.0f);
	KillerCube2->GetPhysicsObject()->SetElasticity(0.0f);
	KillerCube2->GetRenderObject()->SetColour(Vector4(255, 0, 0, 0.5f));

	KillerCube3 = AddKillerCubeToWorld("Killer", Vector3(17, 7, -33), Vector3(1.0f, 1.0f, 1.0f), 0.0f);
	KillerCube3->GetPhysicsObject()->SetElasticity(0.0f);
	KillerCube3->GetRenderObject()->SetColour(Vector4(255, 0, 0, 0.5f));
}

/*
void TutorialGame::InitGameBlock(string name)
{
	for (int x = 50; x > -51; x--)
	{
		Vector3 position = Vector3(x, 0, 50);
		AddCubeToWorld(name, position, Vector3(0.5, 0.5, 0.5), 0.0f);
	}

	for (int x = 50; x > -51; x--)
	{
		Vector3 position = Vector3(x, 0, -50);
		AddCubeToWorld(name, position, Vector3(0.5, 0.5, 0.5), 0.0f);
	}

	for (int z = 49; z > -50; z--)
	{
		Vector3 position = Vector3(50, 0, z);
		AddCubeToWorld(name, position, Vector3(0.5, 0.5, 0.5), 0.0f);
	}

	for (int z = 49; z > -50; z--)
	{
		Vector3 position = Vector3(-50, 0, z);
		AddCubeToWorld(name, position, Vector3(0.5, 0.5, 0.5), 0.0f);
	}
}
*/

// Score Display ---------------------------------------------------------------------------------------------
void TutorialGame::InitScoreDisplay()
{
	renderer->DrawString("Score :: " + std::to_string(playerKey->score), Vector2(5, 5), Vector4(255, 0, 0, 0.6), 15.0f);
	//renderer->DrawString(("Pos :: " + std::to_string(playerKey->GetTransform().GetPosition().x) + ", " + std::to_string(playerKey->GetTransform().GetPosition().y) + ", " + std::to_string(playerKey->GetTransform().GetPosition().z)), Vector2(5, 10), Vector4(255, 0, 0, 0.6), 15.0f);
}

// Plane -----------------------------------------------------------------------------------------------------
void TutorialGame::InitPlane() {
	if (plane == true) {
		Ray r = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		Plane p(Vector3(0, 0, -1), 200.0f);
		bool rp = CollisionDetection::RayPlaneIntersection(r, p, RayCollision());
		if (rp == true) {
			renderer->DrawString("Debug Info :: Plane", Vector2(50, 5));
		}
	}
}

void TutorialGame::UpdateGame(float dt)
{
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}

	SelectObject();
	MoveSelectedObject();
	physics->Update(dt);

	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos - Vector3(17.0f, 8.5f, 10.0f));
		world->GetMainCamera()->SetPitch(angles.x + 17.0f);
		world->GetMainCamera()->SetYaw(angles.y - 45.0f);

		//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}

	InitPlane();
	InitScoreDisplay();
	finalScore = playerKey->score;
	
	if (playerKey->IsTouchDown() == true && playerKey->IsGameOver() == false) {
		gameStatus = "Won";
		return;
	}
	else if (playerKey->IsTouchDown() == false && playerKey->IsGameOver() == true) {
		gameStatus = "Lost";
		return;
	}

	if (playerKey->GetTransform().GetPosition().x > 51.0f
		|| playerKey->GetTransform().GetPosition().x < -51.0f
		|| playerKey->GetTransform().GetPosition().z < -51.0f
		|| playerKey->GetTransform().GetPosition().z > 51.0f
		|| playerKey->GetTransform().GetPosition().y < -3.0f
		|| playerKey->GetTransform().GetPosition().y > 15.0f) {
		gameStatus = "Lost";
		//playerHandle->GetTransform().SetPosition(Vector3(-45.0f, 1.0f, 45.0f));
		//playerHandle->DeactivateObj();
		return;
	}

	UpdateAllMode(dt);
	
	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorldEasy(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject	= nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}

	if (plane == false) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			plane = true;
		}
	}
	else {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
			plane = false;
		}
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 upAxis = Vector3::Cross(fwdAxis, rightAxis);
	upAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 10.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		lockedObject->GetPhysicsObject()->AddForce(-upAxis * (force * 50.0f));//Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::UpdateAllMode(float dt)
{
		if (StatefulBlock2) {
			StatefulBlock2->Update(dt);
			StatefulBlock2->GetRenderObject()->SetColour(Vector4(0.5, 0.5, 0, 1));
		}
		if (StatefulBlock1) {
			StatefulBlock1->Update(dt);
			StatefulBlock1->GetRenderObject()->SetColour(Vector4(0.5, 0.5, 0, 1));
		}

		SolidSpinningCapsule1->GetPhysicsObject()->SetAngularVelocity(Vector3(2, 0, 0));
		SolidSpinningCapsule2->GetPhysicsObject()->SetAngularVelocity(Vector3(2, 0, 0));
		SolidSpinningCapsule3->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 2));
		SolidSpinningCapsule4->GetPhysicsObject()->SetAngularVelocity(Vector3(2, 0, 0));

		SolidSpinningCapsule1->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
		SolidSpinningCapsule2->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
		SolidSpinningCapsule3->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
		SolidSpinningCapsule4->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));

		playerKey->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

		for (auto& i : bonusCoinsSet) {
			i->GetRenderObject()->SetColour(Vector3(1.0f, 0.5f, 0.0f));
		}

		for (auto& i : haningConstraintSet) {
			i->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
		}

		Victory->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));
		Victory->GetRenderObject()->SetColour(Vector4(0, 0, 0, 0.5));

	if (difficultyLevel == 1) {
		KillerCube1->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));
		KillerCube2->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));
		KillerCube3->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));

		KillerSphere1->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));
		KillerSphere2->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));
		KillerSphere3->GetPhysicsObject()->SetAngularVelocity(Vector3(3, 3, 3));

		KillerSphere1->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		KillerSphere2->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		KillerSphere3->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		KillerCube1->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		KillerCube2->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
		KillerCube3->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	}
}

// Objects Adding Functions -------------------------------------------------------------------------------------------------------------
// Position Constraints
void TutorialGame::BuildHangingConstraints(string name, const Vector3& pos) {

	Vector3 cubeSize = Vector3(0.5, 0.5, 0.5);
	float invCubeMass = 1; // how heavy the middle pieces are
	int numLinks = 5;
	float maxDistance = 2; // constraint distance
	float cubeDistance = 2; // distance between links
	Vector3 startPos = pos;
	GameObject* start = AddCubeToWorld(name, startPos + Vector3(0, 0, 0), cubeSize, 0);
	//GameObject* end = AddCubeToWorld("Constraint_Cube", startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
	GameObject* previous = start;
	haningConstraintSet.emplace_back(previous);
	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(name, startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		haningConstraintSet.emplace_back(block);
		previous = block;
	}
	//PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	//world->AddConstraint(constraint);
}

/*
A single function to add a large immoveable cube to the bottom of our world
*/
// Floor & Ceiling
GameObject* TutorialGame::AddFloorToWorld(string name, const Vector3& position) {
	GameObject* floor = new GameObject(name);

	Vector3 floorSize	= Vector3(50, 1, 50);
	AABBVolume* volume	= new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetScale(floorSize * 2).SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0.0f);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	std::cout << "Get World ID :: " << floor->GetWorldID() << " !";

	return floor;
}

/*
Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.
*/
// Sphere Object
GameObject* TutorialGame::AddSphereToWorld(string name, const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject(name);

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	std::cout << "Get World ID :: " << sphere->GetWorldID() << " !";

	return sphere;
}

// Capsule Object
GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform().SetScale(Vector3(radius* 2, halfHeight, radius * 2)).SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}

// Cube Object
GameObject* TutorialGame::AddCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject(name);

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector3(0, 0, 0.8));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	std::cout << "Get World ID :: " << cube->GetWorldID() << " !";

	return cube;
}

// StateObject - Sphere
StateGameObject* TutorialGame::AddStateObjectSphereToWorld(string name, const Vector3& position, float radius, float inverseMass)
{
	StateGameObject* sphere = new StateGameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	std::cout << "Get World ID :: " << sphere->GetWorldID() << " !";

	return sphere;
}

// StateObject - Cube
StateGameObject* TutorialGame::AddStateObjectCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass) {
	StateGameObject* cube = new StateGameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	std::cout << "Get World ID :: " << cube->GetWorldID() << " !";

	return cube;
}

// OBB - Cube
GameObject* TutorialGame::AddOBBCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* OBBcube = new GameObject();

	OBBVolume* volume = new OBBVolume(dimensions);

	OBBcube->SetBoundingVolume((CollisionVolume*)volume);

	OBBcube->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	OBBcube->SetRenderObject(new RenderObject(&OBBcube->GetTransform(), cubeMesh, basicTex, basicShader));
	OBBcube->SetPhysicsObject(new PhysicsObject(&OBBcube->GetTransform(), OBBcube->GetBoundingVolume()));

	OBBcube->GetPhysicsObject()->SetInverseMass(inverseMass);
	OBBcube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(OBBcube);

	std::cout << "Get World ID :: " << OBBcube->GetWorldID() << " !";

	return OBBcube;
}

// Bonus Coins
Bonus* TutorialGame::AddBonusCoinToWorld(string name, const Vector3& position)
{
	Bonus* bonus = new Bonus("Bonus");

	SphereVolume* volume = new SphereVolume(1.5f);
	bonus->SetBoundingVolume((CollisionVolume*)volume);

	bonus->GetTransform().SetScale(Vector3(0.25, 0.25, 0.25)).SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), bonusMesh, nullptr, basicShader));
	bonus->GetRenderObject()->SetColour(Vector3(1.0f, 0.5f, 0.0f));
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume()));

	bonus->GetPhysicsObject()->SetInverseMass(0.0f);
	bonus->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(bonus);
	bonusCoinsSet.emplace_back(bonus);

	std::cout << "Get World ID :: " << bonus->GetWorldID() << " !";

	return bonus;
}

// Victory Cube -- OBB
VictoryCube* TutorialGame::AddVictoryCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass)
{
	VictoryCube* victory = new VictoryCube(name);

	OBBVolume* volume = new OBBVolume(dimensions);

	victory->SetBoundingVolume((CollisionVolume*)volume);

	victory->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	victory->SetRenderObject(new RenderObject(&victory->GetTransform(), cubeMesh, basicTex, basicShader));
	victory->SetPhysicsObject(new PhysicsObject(&victory->GetTransform(), victory->GetBoundingVolume()));

	victory->GetPhysicsObject()->SetInverseMass(inverseMass);
	victory->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(victory);

	std::cout << "Get World ID :: " << victory->GetWorldID() << " !";

	return victory;
}

//Killer Objects of Normal Mode
// Cube
Killer* TutorialGame::AddKillerCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass) {
	Killer* cube = new Killer(name);

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	std::cout << "Get World ID :: " << cube->GetWorldID() << " !";

	return cube;
}

// Sphere
Killer* TutorialGame::AddKillerSphereToWorld(string name, const Vector3& position, float radius, float inverseMass) {
	Killer* sphere = new Killer(name);

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	std::cout << "Get World ID :: " << sphere->GetWorldID() << " !";

	return sphere;
}

/*
GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}
*/

/*
GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject("apple");

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}
*/

/*
void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld("Sphere", position, radius, 1.0f);
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld("Cube", position, cubeDims);
			}
			else {
				AddSphereToWorld("Sphere", position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld("Cube", position, cubeDims, 1.0f);
		}
	}
}
*/

//void TutorialGame::InitGameExamples() {
	//AddPlayerToWorld(Vector3(0, 5, 0));
	//AddEnemyToWorld(Vector3(5, 5, 0));
	//AddBonusToWorld(Vector3(10, 2, 20));
//}



/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject	= nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}

//------------------------------------------- RAY-vs-WORLD Debug Information ---------------------------------------------------------------------------------------
		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::MIDDLE))
		{
			if (selectionObject)
			{
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				if (selectionObject)
				{
					int volumeType = ((int)(VolumeType*)selectionObject->GetBoundingVolume()->type);

					switch (volumeType)
					{
						//AABB = 1,
						//OBB = 2,
						//Sphere = 4,
						//Mesh = 8,
						//Capsule = 16,
						//Compound = 32,
						//Invalid = 256
					case 1:
						std::cout << "\nDebug Information :: AABB";
						renderer->DrawString("Debug Info :: AABB", Vector2(50, 5)); break;

					case 2:
						std::cout << "\nDebug Information :: OBB";
						renderer->DrawString("Debug Info :: OBB", Vector2(50, 5)); break;

					case 4:
						std::cout << "\nDebug Information :: Sphere";
						renderer->DrawString("Debug Info :: Sphere", Vector2(50, 5)); break;

					case 8:
						std::cout << "\nDebug Information :: Mesh";
						renderer->DrawString("Debug Info :: Mesh", Vector2(50, 5)); break;

					case 16:
						std::cout << "\nDebug Information :: Capsule";
						renderer->DrawString("Debug Info :: Capsule", Vector2(50, 5)); break;

					case 32:
						std::cout << "\nDebug Information :: Compound";
						renderer->DrawString("Debug Info :: Compound", Vector2(50, 5)); break;

					case 256:
						std::cout << "\nDebug Information :: Invalid";
						renderer->DrawString("Debug Info :: Invalid", Vector2(50, 5)); break;

					default:
						std::cout << "\nDebug Information :: NA";
						renderer->DrawString("Debug Info :: NA", Vector2(50, 5)); break;
					}

				}
			}
		}
	}

	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if(selectionObject){
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() {

	renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20)); // Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;
	if (!selectionObject) {
		return;// we haven 't selected anything !
	}

	// Push the selected object !
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}