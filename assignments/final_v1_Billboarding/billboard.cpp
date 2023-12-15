#include "Billboard.h"

Billboard::Billboard(ew::Vec3 pos)
{
	this->transform.position = pos;
}

//basic update, get the distance on each axis then set the rotation of the object based on this
void Billboard::update(ew::Vec3 playerPos)
{
	ew::Vec3 offset = playerPos - transform.position;
	float distX = sqrtf(powf(playerPos.z - transform.position.z, 2) + powf(playerPos.y - transform.position.y, 2));
	float distY = sqrtf(powf(playerPos.z- transform.position.z, 2) + powf(playerPos.x - transform.position.x, 2));
	float xRad = acos(offset.y / distX);
	float yRad = -acos(offset.x / distY);
	bool flip = offset.z <= -1;

	float x = fmod(ew::Degrees(flip ? xRad + 135 : xRad), 360);
	float y = ew::Degrees(flip ? -yRad : yRad) + 90;
	float z = flip ? 180 : 0;
	
	if (abs(offset.x) > 3 && !(offset.y < -2) && offset.z < 1 && offset.z > -1)
	{
		x = ew::Clamp(x, 0, 115);
	}

	transform.rotation = ew::Vec3(x, y, z);
}

//exactly the same as above update but also moves with camera
void Billboard::update(ew::Vec3 playerPos, ew::Vec3 camTarget)
{
	ew::Vec3 offset = playerPos - transform.position;
	bool flip = offset.z <= 0;

	float distX;
	if (abs(offset.z) > abs(offset.x))
	{
		distX = sqrtf(powf(playerPos.z - transform.position.z, 2) + powf(playerPos.y - transform.position.y, 2));
	}
	else
	{
		distX = sqrtf(powf(playerPos.x - transform.position.x, 2) + powf(playerPos.y - transform.position.y, 2));
	}
	float distY = sqrtf(powf(playerPos.z - transform.position.z, 2) + powf(playerPos.x - transform.position.x, 2));

	float xRad = acos(offset.y / distX);
	float yRad = -acos(offset.x / distY);

	float x = fmod(ew::Degrees(flip ? xRad + 135 : xRad), 360);
	float y = ew::Degrees(flip ? -yRad : yRad) + 90;
	float z = flip ? 180 : 0;

	if (abs(offset.x) > 3 && !(offset.y < -2) && offset.z < 1 && offset.z > -1)
	{
		x = ew::Clamp(x, 0, 115);
	}

	transform.rotation = ew::Vec3(x, y, z);

	//offset the board from camera based on scale
	ew::Vec3 Unitoffset = ew::Normalize(playerPos - camTarget);

	transform.position = playerPos - Unitoffset * scaleDistance;
}

void Billboard::update(ew::Vec3 playerPos, ew::Vec3 camTarget, ew::Vec2 canvasOffset)
{
	//start by figuring out the offset off the canvas center
	ew::Vec3 worldOffset;
	if (canvas)
	{
		//find the plane normal and use it to calculate the canvas's local forward down and left vectors
		ew::Vec3 planeOffset = canvas->transform.position - playerPos;
		ew::Vec3 planeNormal = ew::Normalize(planeOffset);
		ew::Vec3 forward = ew::Vec3(0, 0, 1);
		//the weird vector math I dont  totally understand, I worked for a long time on this and I am still not happy with this.
		//I have learned from this but would need to rework this sysem at a very core level to do it in a way that I like better, which I sady do not have the time to do
		float forwardAngle = acos(ew::Dot(forward, planeNormal) / (ew::Magnitude(forward) * ew::Magnitude(planeNormal)));
		float cosAngle = -cos(forwardAngle) * abs(sin(ew::Radians(canvas->transform.rotation.x)));
		float z;
		if (planeOffset.x < 0)
			z = ((-(0.5 + (cosAngle) / 2) / 2 + 0.5) * 360) * abs(sin(ew::Radians(canvas->transform.rotation.x)));
		else
			z = (((0.5 + (cosAngle) / 2) / 2 + 0.5) * 360) * abs(sin(ew::Radians(canvas->transform.rotation.x)));

		ew::Vec3 xVec = (ew::Vec3(cos(ew::Radians(canvas->transform.rotation.y)), 0, -sin(ew::Radians(canvas->transform.rotation.y))));
		ew::Vec3 yVec = (ew::Vec3(0, abs(sin(ew::Radians(canvas->transform.rotation.x))), -cos(ew::Radians(canvas->transform.rotation.x))));
		ew::Vec3 zVec = ew::Vec3(ew::Radians(sin(z)), 0, ew::Radians(sin(z)));

		ew::Vec3 left = xVec * (canvasOffset.x * canvas->size.x / 2);
		ew::Vec3 down = yVec * (canvasOffset.y * canvas->size.y / 2);
		forward = zVec * (canvas->size.x / 2 + canvas->size.y / 2)/3;
		
		//add these weird vectors scaled by canvas size
		worldOffset = ew::Vec3((left.x + down.x + forward.x), (left.y + down.y + forward.y), (left.z + down.z + forward.z));
	}
	else
	{
		worldOffset = ew::Vec3(canvasOffset.x, canvasOffset.y, 0);
	}
	//set to canvas offset by the value calculated above
	transform.position = canvas->transform.position + worldOffset;

	transform.rotation = canvas->transform.rotation;
}