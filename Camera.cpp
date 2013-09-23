#include "Camera.h"

#pragma once

#include "foundation/PxTransform.h"
#include "foundation/PxMat33.h"

namespace VisualDebugger
{
	using namespace physx;

	Camera::Camera(const PxVec3 &eye, const PxVec3& dir)
	{
		mEye = eye;
		mDir = dir.getNormalized();
		speed = 0.1f;
	}

	void Camera::Motion(int dx, int dy)
	{
		PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();

		PxQuat qx(PxPi * dx * speed/ 180.0f, PxVec3(0,1,0));
		mDir = qx.rotate(mDir);
		PxQuat qy(PxPi * dy * speed/ 180.0f, viewY);
		mDir = qy.rotate(mDir);

		mDir.normalize();
	}

	void Camera::AnalogMove(float x, float y)
	{
		PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();
		mEye += mDir*y;
		mEye += viewY*x;
	}

	PxVec3 Camera::getEye() const 
	{ 
		return mEye; 
	}

	PxVec3 Camera::getDir() const 
	{ 
		return mDir; 
	}

	PxTransform	Camera::getTransform() const
	{
		PxVec3 viewY = mDir.cross(PxVec3(0,1,0));

		if(viewY.normalize()<1e-6f) 
			return PxTransform(mEye);

		PxMat33 m(mDir.cross(viewY), viewY, -mDir);
		return PxTransform(mEye, PxQuat(m));
	}

	void Camera::MoveForward() 
	{ 
		mEye += mDir*speed*4; 
	}

	void Camera::MoveBackward() 
	{ 
		mEye -= mDir*speed*4; 
	}

	void Camera::MoveLeft() 
	{
		PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();
		mEye -= viewY*speed*4; 
	}

	void Camera::MoveRight() 
	{ 
		PxVec3 viewY = mDir.cross(PxVec3(0,1,0)).getNormalized();
		mEye += viewY*speed*4; 
	}

	void Camera::Speed(float value)
	{ 
		speed = value; 
	}

	float Camera::Speed() const
	{
		return speed; 
	}
}