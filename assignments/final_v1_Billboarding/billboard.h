#pragma once

#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>

class Billboard
{
	//developed in stages, first getting the basic concept working, then get more complex concepts in.
public:
	//setup
	ew::Transform transform;
		//only inportant for canvas
	ew::Vec2 size;

		//canvas is a billboard the size of the camera view at cartian distance and is used for offsetting objects from the camera's center
	Billboard* canvas = nullptr;
	ew::Vec3 position;
	float scaleDistance;
	ew::Vec2 offSave;
	float sparkExpire;

	Billboard() { this->transform.position = ew::Vec3(0, 0, 0); };
	Billboard(ew::Vec3 pos);
	void update(ew::Vec3 playerPos);
	void update(ew::Vec3 playerPos, ew::Vec3 camTarget);
	void update(ew::Vec3 playerPos, ew::Vec3 camTarget, ew::Vec2 canvasOffset);

};