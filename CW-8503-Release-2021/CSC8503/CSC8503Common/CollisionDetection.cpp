#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"
#include "Debug.h"

#include <list>

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;
		case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i) { // get best 3 intersections
		if (rayDir[i] > 0) {
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		}
		else if (rayDir[i] < 0) {
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
		}
	}

	float bestT = tVals.GetMaxElement();
	if (bestT < 0.0f) {
		return false; // no backwards rays !
	}

	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f; // an amount of leeway in our calcs
	for (int i = 0; i < 3; ++i) {
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) {
			return false; // best intersection doesn 't touch the box !
		}
	}
	collision.collidedAt = intersection;
	collision.rayDistance = bestT;

	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {
	
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();

	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {
	
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();
	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());
	Vector3 localRayPos = r.GetPosition() - position;
	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);

	if (collided) {
		collision.collidedAt = transform * collision.collidedAt + position;
	}

	return collided;
}

float capsuleIntersect(Vector3 rayOrigin, Vector3 rayDirection, Vector3 pointA, Vector3 pointB, float radius)
{
	//BA - lineSegment
	//OA - relativeOrigin
	Vector3 BA = pointB - pointA;
	Vector3 OA = rayOrigin - pointA;
	float BABA = Vector3::Dot(BA, BA);
	float BARD = Vector3::Dot(BA, rayDirection);
	float BAOA = Vector3::Dot(BA, OA);
	float RDOA = Vector3::Dot(rayDirection, OA);
	float OAOA = Vector3::Dot(OA, OA);
	float a = BABA - BARD * BARD;
	float b = BABA * RDOA - BAOA * BARD;
	float c = BABA * OAOA - BAOA * BAOA - radius * radius * BABA;
	float h = b * b - a * c;
	if (h >= 0.0)
	{
		float t = (-b - sqrt(h)) / a;
		float y = BAOA + t * BARD;
		// body
		if (y > 0.0 && y < BABA) return t;
		// caps
		Vector3 oc = (y <= 0.0) ? OA : rayOrigin - pointB;
		b = Vector3::Dot(rayDirection, oc);
		c = Vector3::Dot(oc, oc) - radius * radius;
		h = b * b - c;
		if (h > 0.0) return -b - sqrt(h);
	}
	return -1.0;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {
	
	float capRadius = volume.GetRadius();
	float capHalfHeight = volume.GetHalfHeight();
	Quaternion capOrientation = worldTransform.GetOrientation();
	Vector3 capPosition = worldTransform.GetPosition();
	Matrix3 capTransform = Matrix3(capOrientation);
	Matrix3 capInvTransform = Matrix3(capOrientation.Conjugate());
	Vector3 capOrigin = worldTransform.GetPosition();
	Vector3 up = capInvTransform * Vector4(0, 1, 0, 1.0f);

	Vector3 Top(capOrigin + up * capHalfHeight), Bottom(capOrigin + up * capHalfHeight);
	float rayDistance = capsuleIntersect(r.GetPosition(), r.GetDirection(), Top, Bottom, capRadius);

	if (rayDistance == -1.0f)
	{
		return false;
	}

	collision.rayDistance = rayDistance;
	collision.collidedAt = r.GetPosition() + r.GetDirection() * rayDistance;

	return true;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {
	
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();

	// Get the direction between the ray origin and the sphere origin
	Vector3 dir = (spherePos - r.GetPosition());

	// Then project the sphere 's origin onto our ray direction vector
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f) {
		return false; // point is behind the ray !
	}

	// Get closest point on ray line to sphere
	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius) {
		return false;
	}

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));

	collision.rayDistance = sphereProj - (offset);
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
	
	return true;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x, screenSize.y - screenMouse.y, -0.99999f);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	//std::cout << "Ray Direction:" << c << std::endl;

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0]  = aspect / h;
	m.array[5]  = tan(fov*PI_OVER_360);

	m.array[10] = 0.0f;
	m.array[11] = 1.0f / d;

	m.array[14] = 1.0f / e;

	m.array[15] = -c / (d*e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
Matrix4::Translation(position) *
Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
Matrix4::Rotation(pitch, Vector3(1, 0, 0));

return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	//AABB
	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	//Sphere
	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	//OBB
	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}

	//AABB-Sphere / Sphere-AABB
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//Capsule-Sphere / Sphere-Capsule
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//OBB-Sphere / Sphere-OBB :: For CourseWork
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) {
		return OBBSphereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBSphereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//AABB-OBB / OBB-AABB :: For CourseWork
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::OBB) {
		return AABBOBBIntersection((AABBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBOBBIntersection((AABBVolume&)*volB, transformB, (OBBVolume&)*volA, transformA, collisionInfo);
	}

	/*
	//Capsule-AABB / AABB-Capsule :: For CourseWork
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		return AABBCapsuleIntersection((CapsuleVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((CapsuleVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);
	}*/

	return false;
}

//AABB TEST
bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;
	if (abs(delta.x) < totalSize.x && abs(delta.y) < totalSize.y && abs(delta.z) < totalSize.z) {
		return true;
	}
	return false;
}

//AABB / AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();
	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();
	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

	if (overlap) {
		static const Vector3 faces[6] =
		{
		Vector3(-1, 0, 0), Vector3(1, 0, 0),
		Vector3(0, -1, 0), Vector3(0, 1, 0),
		Vector3(0, 0, -1), Vector3(0, 0, 1),
		};
		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;
		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;
		float distances[6] =
		{
		(maxB.x - minA.x),// distance of box 'b' to 'left ' of 'a '.
		(maxA.x - minB.x),// distance of box 'b' to 'right ' of 'a '.
		(maxB.y - minA.y),// distance of box 'b' to 'bottom ' of 'a '.
		(maxA.y - minB.y),// distance of box 'b' to 'top ' of 'a '.
		(maxB.z - minA.z),// distance of box 'b' to 'far ' of 'a '.
		(maxA.z - minB.z) // distance of box 'b' to 'near ' of 'a '.
		};
		float penetration = FLT_MAX;
		Vector3 bestAxis;
		for (int i = 0; i < 6; i++)
		{
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}

	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();
		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;// we 're colliding !
	}
	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();
	if (distance < volumeB.GetRadius()) {// yes , we 're colliding !
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);
		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();
		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}

bool CollisionDetection::OBBIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	return false;
}

// Capsule - Sphere Intersection
bool CollisionDetection::SphereCapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Bringing Up Usual Calc For Capsule
	Quaternion orientationA = worldTransformA.GetOrientation();
	Vector3 positionA = worldTransformA.GetPosition();
	Matrix3 transformA = Matrix3(orientationA);
	Matrix3 invTransformA = Matrix3(orientationA.Conjugate());
	Vector3 originA = positionA;
	Vector3 UP = transformA * Vector3(0, 1, 0);
	Vector3 RIGHT = transformA * Vector3(1, 0, 0);
	float capsuleRadius = volumeA.GetRadius();
	float capsuleHalfHeight = volumeA.GetHalfHeight();
	float capsuleHemisphereOriginDistance = (capsuleHalfHeight - capsuleRadius);
	Vector3 capsuleHemisphereOriginUP(originA + UP * capsuleHemisphereOriginDistance), capsuleHemisphereOriginDOWN(originA - UP * capsuleHemisphereOriginDistance);

	//Sphere Origin
	Vector3 originB = worldTransformB.GetPosition();

	Vector3 capsuleBestSphereOrigin = CapsuleVolume::ProjectPointOntoVector(capsuleHemisphereOriginDOWN, capsuleHemisphereOriginUP, originB);
	bool result = SphereIntersection(SphereVolume(capsuleRadius), Transform().SetPosition(capsuleBestSphereOrigin), volumeB, worldTransformB, collisionInfo);
	if (result)
	{
		ContactPoint CP = collisionInfo.GetCP();
		CP.localA += capsuleBestSphereOrigin;
		collisionInfo.AddCP(CP);
	}
	return result;
}

// OBB - Sphere Intersection
bool CollisionDetection::OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientationA = worldTransformA.GetOrientation();
	Matrix3 transformA = Matrix3(orientationA);
	Matrix3 invTransformA = Matrix3(orientationA.Conjugate());
	Vector3 positionA = worldTransformA.GetPosition();
	Vector3 positionB = invTransformA * (worldTransformB.GetPosition() - positionA);
	Vector3 delta = positionB; //positionB - positionA;

	Vector3 closestPointOnBox = Maths::Clamp(delta, -volumeA.GetHalfDimensions(), volumeA.GetHalfDimensions());
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();
	if (distance < volumeB.GetRadius()) {// yes , we 're colliding !
		Vector3 collisionNormal = localPoint.Normalised(); //transformA * 
		float penetration = (volumeB.GetRadius() - distance);
		Vector3 localA = Vector3();
		Vector3 localB = (-collisionNormal * volumeB.GetRadius());
		collisionInfo.AddContactPoint(localA, transformA * localB, transformA * collisionNormal, penetration);
		return true;
	}
	return false;
}

// AABB - OBB Intersection
bool CollisionDetection::AABBOBBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientationB = worldTransformB.GetOrientation();
	Matrix3 transformB = Matrix3(orientationB);
	Matrix3 invTransformB = Matrix3(orientationB.Conjugate());
	Vector3 positionB = worldTransformB.GetPosition(); //invTransformB *
	Vector3 positionA = invTransformB * worldTransformA.GetPosition() - positionB; //invTransformB * 

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();
	bool overlap = AABBTest(positionA, positionB, boxASize, boxBSize);
	if (overlap) {

		//Vector3 positionB = transformB * Vector3() + worldTransformB.GetPosition(); //invTransformB *
		//Vector3 positionA = Vector3() + worldTransformA.GetPosition(); //invTransformB * 

		static const Vector3 faces[6] =
		{
		Vector3(-1, 0, 0), Vector3(1, 0, 0),
		Vector3(0, -1, 0), Vector3(0, 1, 0),
		Vector3(0, 0, -1), Vector3(0, 0, 1),
		};
		Vector3 maxA = positionA + boxASize;
		Vector3 minA = positionA - boxASize;
		Vector3 maxB = positionB + boxBSize;
		Vector3 minB = positionB - boxBSize;
		float distances[6] =
		{
		(maxB.x - minA.x),// distance of box 'b' to 'left ' of 'a '.
		(maxA.x - minB.x),// distance of box 'b' to 'right ' of 'a '.
		(maxB.y - minA.y),// distance of box 'b' to 'bottom ' of 'a '.
		(maxA.y - minB.y),// distance of box 'b' to 'top ' of 'a '.
		(maxB.z - minA.z),// distance of box 'b' to 'far ' of 'a '.
		(maxA.z - minB.z) // distance of box 'b' to 'near ' of 'a '.
		};
		float penetration = FLT_MAX;
		Vector3 bestAxis;
		for (int i = 0; i < 6; i++)
		{
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}
	return false;
}

// AABB - Capsule Intersection
bool CollisionDetection::AABBCapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Bringing Up Usual Calc For Capsule
	Quaternion orientationA = worldTransformA.GetOrientation();
	Vector3 positionA = worldTransformA.GetPosition();
	Matrix3 transformA = Matrix3(orientationA);
	Matrix3 invTransformA = Matrix3(orientationA.Conjugate());
	Vector3 originA = positionA;
	Vector3 UP = transformA * Vector3(0, 1, 0);
	Vector3 RIGHT = transformA * Vector3(1, 0, 0);
	float capsuleRadius = volumeA.GetRadius();
	float capsuleHalfHeight = volumeA.GetHalfHeight();
	float capsuleHemisphereOriginDistance = (capsuleHalfHeight - capsuleRadius);
	Vector3 HemisphereOriginUP(originA + UP * capsuleHemisphereOriginDistance), HemisphereOriginDOWN(originA - UP * capsuleHemisphereOriginDistance);
	Vector3 capsuleHemisphereOriginUP(originA + UP * capsuleHemisphereOriginDistance), capsuleHemisphereOriginDOWN(originA - UP * capsuleHemisphereOriginDistance);

	//AABB Origin
	Vector3 originB = worldTransformB.GetPosition();

	Vector3 capsuleBestSphereOrigin = CapsuleVolume::ProjectPointOntoVector(capsuleHemisphereOriginDOWN, capsuleHemisphereOriginUP, originB);
	bool result = AABBSphereIntersection(volumeB, worldTransformB, SphereVolume(capsuleRadius), Transform().SetPosition(capsuleBestSphereOrigin), collisionInfo);
	if (result) {
		ContactPoint CP = collisionInfo.GetCP();
		CP.localB += capsuleBestSphereOrigin; Vector3 lB = CP.localB;
		CP.localB = CP.localA; CP.localA = lB;
		CP.normal *= -1.0f;
		collisionInfo.AddCP(CP);
	}
	return result;
}