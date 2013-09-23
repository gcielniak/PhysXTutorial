#pragma once

#include "foundation/PxTransform.h"
#include "foundation/PxMat33.h"

namespace VisualDebugger
{
	using namespace physx;

	///Camera class
	class Camera
	{
	private:
		PxVec3	mEye;
		PxVec3	mDir;
		float speed;

	public:
		Camera(const PxVec3 &eye, const PxVec3& dir);

		void Motion(int dx, int dy);

		void AnalogMove(float x, float y);

		PxVec3 getEye() const;

		PxVec3 getDir() const;

		PxTransform	getTransform() const;

		void MoveForward();

		void MoveBackward();

		void MoveLeft();

		void MoveRight();

		void Speed(float value);

		float Speed() const;
	};
}