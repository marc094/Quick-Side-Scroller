#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Defs.h"
#include "Utils.h"
#include "Rect.h"

class PhysBody;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	void SetScale(scalar);
	void SetPosition(svec2);
	void SetTarget(PhysBody*);
	void SetSpeed(scalar);

	scalar GetScale();
	svec2 GetPosition();
	PhysBody* GetTarget();
	scalar GetSpeed();

	svec2 ScreenToWorld(svec2);
	iRect ScreenToWorld(iRect);
	const svec2 WorldToScreen(svec2);
	iRect WorldToScreen(iRect);

	iRect GetRect();

	void Move(svec2);

	void Update();

private:
	svec2 position;
	scalar scale = 1.0;
	scalar speed = 0.0;
	sint16 cullingMargin = 0;

	PhysBody* target = nullptr;
};
#endif