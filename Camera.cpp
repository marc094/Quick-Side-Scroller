#include "Camera.h"
#include "PhysBody.h"

extern float realDeltaTime;

Camera::Camera()
{
	rect = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };
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
	rect.x = (int)(pos.x - rect.w / 2);
	rect.y = (int)(pos.y - rect.h / 2);
}

void Camera::SetTarget(PhysBody* object)
{
	target = object;
}

scalar & Camera::GetScale()
{
	return scale;
}

svec2 Camera::GetPosition()
{
	return position;
}

iRect Camera::GetRect()
{
	return rect;
}

PhysBody* Camera::GetTarget()
{
	return target;
}

void Camera::Move(svec2 vector)
{
	position += vector;
	SetPosition(position);
}

void Camera::Update()
{
	if (target != nullptr)
	{
		speed = Utils::Interpolate(speed, MAX_CAMERA_MOVEMENT_SPEED, CAMERA_ACCELERATION);
		SetPosition(Utils::Interpolate(position, target->pos, speed * (1 / scale) * realDeltaTime));

		if (!target->active)
			target = nullptr;
	}
	else speed = 0.0;
}
