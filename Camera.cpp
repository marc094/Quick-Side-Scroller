#include "Camera.h"
#include "PhysBody.h"

extern float deltaTime;
extern float realDeltaTime;

Camera::Camera()
{
	SetPosition({ 0,0 });
}


Camera::~Camera()
{
}

void Camera::SetScale(scalar s)
{
	scale = s;
}

void Camera::SetPosition(svec2 pos)
{
	position = pos;
}

void Camera::SetTarget(PhysBody* object)
{
	target = object;
}

void Camera::SetSpeed(scalar s)
{
	speed = s;
}

scalar Camera::GetScale()
{
	return scale;
}

svec2 Camera::GetPosition()
{
	return position;
}

PhysBody* Camera::GetTarget()
{
	return target;
}

scalar Camera::GetSpeed()
{
	return speed;
}

svec2 Camera::ScreenToWorld(svec2 v)
{
	v.x = (v.x - HALF_SCREEN_WIDTH) / scale + position.x;
	v.y = (v.y - HALF_SCREEN_HEIGHT) / scale + position.y;
	return v;
}

iRect Camera::ScreenToWorld(iRect r)
{
	r.x = (r.x - HALF_SCREEN_WIDTH) / scale + position.x;
	r.y = (r.y - HALF_SCREEN_HEIGHT) / scale + position.y;
	r.w /= scale;
	r.h /= scale;
	return r;
}

const svec2 Camera::WorldToScreen(svec2 v)
{
	v.x = (v.x - position.x) * scale + HALF_SCREEN_WIDTH;
	v.y = (v.y - position.y) * scale + HALF_SCREEN_HEIGHT;
	return v;
}

iRect Camera::WorldToScreen(iRect r)
{
	r.x = (r.x - position.x) * scale + HALF_SCREEN_WIDTH;
	r.y = (r.y - position.y) * scale + HALF_SCREEN_HEIGHT;
	r.w *= scale;
	r.h *= scale;
	return r;
}

void Camera::Move(svec2 vector)
{
	position += vector * realDeltaTime / scale;
}

void Camera::Update()
{
	if (target != nullptr)
	{
		speed = Utils::Interpolate(speed, MAX_CAMERA_MOVEMENT_SPEED, CAMERA_ACCELERATION);
		SetPosition(Utils::Interpolate(position, target->pos, speed * max(deltaTime, realDeltaTime)));

		if (!target->active)
			target = nullptr;
	}
	else speed = 0.0;
}
