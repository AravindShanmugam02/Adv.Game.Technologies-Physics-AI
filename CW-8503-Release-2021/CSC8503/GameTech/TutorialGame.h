#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "Bonus.h"
#include "VictoryCube.h"
#include "KillerObjNormalMode.h"
#include "StateGameObject.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame(int difficulty);
			~TutorialGame();
			int finalScore;
			string gameStatus;
			bool plane;

			virtual void UpdateGame(float dt);
			virtual void UpdateMenu(float dt);
			void UpdateAllMode(float dt);

		protected:
			void InitialiseAssets(int difficulty);
			void ClearResetGame();

			void InitCamera();
			void InitMenuCamera();
			void MenuCameraUpdate(float dt);
			void UpdateKeys();

			void InitWorld();
			void InitWorldEasy();
			void InitWorldNormal();
			void InitWorldHard();
			void InitMenu();
			void InitCommonObj();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			//void InitDefaultFloor(string name);
			//void BridgeConstraintTest();
	
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(string name, const Vector3& position);
			GameObject* AddSphereToWorld(string name, const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);
			GameObject* AddPlayerToWorld(string name, const Vector3& position);
			GameObject* AddEnemyToWorld(string name, const Vector3& position);

			StateGameObject* AddStateObjectSphereToWorld(string name, const Vector3& position, float radius, float inverseMass = 10.0f);
			StateGameObject* AddStateObjectCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			StateGameObject* testStateObject;
			StateGameObject* StatefulBlock1;
			StateGameObject* StatefulBlock2;
			StateGameObject* StatefulBlock3;
			StateGameObject* StatefulBlock4;

			GameObject* SolidSpinningCapsule1;
			GameObject* SolidSpinningCapsule2;
			GameObject* SolidSpinningCapsule3;
			GameObject* SolidSpinningCapsule4;
			GameObject* SolidOBBWall1;
			GameObject* SolidOBBWall2;
			GameObject* SolidOBBWall3;
			GameObject* SolidOBBWall4;
			GameObject* Floor;
			GameObject* Ceiling;

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;
			int			difficultyLevel;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 10, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			//CW
			void InitPlayerSphere(string name);
			void InitBonusCoin(string name, const int& numOf);
			void InitGameBlock(string name);
			void InitOBBCube();
			void InitScoreDisplay();
			void InitPlane();
			void InitFloorCeiling(string name);
			void InitSpinningCapsules();
			void InitStatefulObjects();
			void InitHangingConstraints(string name, int num, const Vector3& pos);
			void BuildHangingConstraints(string name, const Vector3& pos);
			void InitKillerObjsOfNormalMode();
			
			GameObject* playerKey;
			VictoryCube* Victory;
			Killer* KillerSphere1;
			Killer* KillerSphere2;
			Killer* KillerSphere3;
			Killer* KillerCube1;
			Killer* KillerCube2;
			Killer* KillerCube3;
			vector<Bonus*> bonusCoinsSet;
			vector<GameObject*> haningConstraintSet;

			GameObject* AddOBBCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			Bonus* AddBonusCoinToWorld(string name, const Vector3& position);
			VictoryCube* AddVictoryCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			Killer* AddKillerCubeToWorld(string name, const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			Killer* AddKillerSphereToWorld(string name, const Vector3& position, float radius, float inverseMass = 10.0f);
		};
	}
}

