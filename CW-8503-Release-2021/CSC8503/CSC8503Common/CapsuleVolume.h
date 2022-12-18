#pragma once
#include "CollisionVolume.h"

namespace NCL {
    class CapsuleVolume : public CollisionVolume
    {
    public:
        CapsuleVolume(float halfHeight, float radius) {
            this->halfHeight    = halfHeight;
            this->radius        = radius;
            this->type          = VolumeType::Capsule;
        };
        ~CapsuleVolume() {

        }
        float GetRadius() const {
            return radius;
        }

        float GetHalfHeight() const {
            return halfHeight;
        }

        static inline Vector3 ProjectPointOntoVector(Vector3 A, Vector3 B, Vector3 pointC) {
            Vector3 AB = B - A;
            Vector3 AC = pointC - A;
            float t = Vector3::Dot(AC, AB) / Vector3::Dot(AB, AB);
            float saturated_t = std::fmin(std::fmax(t, 0.0f), 1.0f); //std::min(std::max(t, 0.0f), 1.0f);
            return (A + AB * saturated_t);
        }

    protected:
        float radius;
        float halfHeight;
    };
}

