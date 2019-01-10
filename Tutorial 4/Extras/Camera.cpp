#include "Camera.h"

namespace VisualDebugger
{
	using namespace physx;

	Camera::Camera(const PxVec3 &_eye, const PxVec3& _dir, PxReal _speed)
	{
		eye_init = _eye;
		dir_init = _dir.getNormalized();
		speed_init = _speed;
		Reset();
	}

	void Camera::Reset()
	{
		eye = eye_init;
		dir = dir_init;
		speed = speed_init;
	}

	void Camera::Motion(int dx, int dy, PxReal delta_time)
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();

		PxQuat qx(PxPi * dx * speed * delta_time/ 180.0f, PxVec3(0,1,0));
		dir = qx.rotate(dir);
		PxQuat qy(PxPi * dy * speed * delta_time/ 180.0f, viewY);
		dir = qy.rotate(dir);

		dir.normalize();
	}

	void Camera::AnalogMove(float x, float y)
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();
		eye += dir*y;
		eye += viewY*x;
	}

	PxVec3 Camera::getEye() const 
	{ 
		return eye; 
	}

	PxVec3 Camera::getDir() const 
	{ 
		return dir; 
	}

	PxTransform	Camera::getTransform() const
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0));

		if(viewY.normalize()<1e-6f) 
			return PxTransform(eye);

		PxMat33 m(dir.cross(viewY), viewY, -dir);
		return PxTransform(eye, PxQuat(m));
	}

	void Camera::MoveForward(PxReal delta_time) 
	{ 
		eye += dir*speed*delta_time; 
	}

	void Camera::MoveBackward(PxReal delta_time) 
	{ 
		eye -= dir*speed*delta_time; 
	}

	void Camera::MoveLeft(PxReal delta_time) 
	{
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();
		eye -= viewY*speed*delta_time; 
	}

	void Camera::MoveRight(PxReal delta_time) 
	{ 
		PxVec3 viewY = dir.cross(PxVec3(0,1,0)).getNormalized();
		eye += viewY*speed*delta_time; 
	}

	void Camera::MoveUp(PxReal delta_time)
	{
		eye += PxVec3(0,1,0)*speed*delta_time; 
	}

	void Camera::MoveDown(PxReal delta_time)
	{
		eye -= PxVec3(0,1,0)*speed*delta_time; 
	}
}