#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include <cmath>


namespace hb {

	//Identity matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Scale on x,y,z axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(s.x, 0, 0, 0,
						0, s.y, 0, 0,
						0, 0, s.z, 0,
						0, 0,   0, 1
			);
	};
	//Rotation around X axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		return ew::Mat4(1, 0, 0, 0,
						0, std::cos(rad), -std::sin(rad), 0,
						0, std::sin(rad), std::cos(rad) , 0,
						0, 0, 0, 1
		);
	};
	//Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		return ew::Mat4(
			std::cos(rad), 0, std::sin(rad), 0,
			0, 1, 0, 0,
			-std::sin(rad), 0, std::cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		return ew::Mat4(
			std::cos(rad), -std::sin(rad), 0, 0,
			std::sin(rad), std::cos(rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	//Creates a right handed view space
		//eye = eye (camera) position
		//target = position to look at
		//up = up axis, usually(0,1,0)
	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up)
	{
		//use ew::Cross for cross product!
		auto f = ew::Normalize(target - eye);
		auto r = ew::Cross(up, f);
		r = r / ew::Magnitude(r);
		auto u = ew::Cross(f, r);
		u = u / ew::Magnitude(u);
		return ew::Mat4(
			r.x, u.x, f.x, 0,
			r.y, u.y, f.y, 0,
			r.z, u.z, f.z, 0,
			0, 0, 0, 1
		);
	};
	//Orthographic projection
	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far) 
	{
		auto r = height*aspect / 2;
		auto l = -r;
		auto t = height / 2;
		auto b = -t;

		return ew::Mat4(
			(2/(r-l)), 0, 0, -((r+l)/(r-l)),
			0, (2/t-b), 0, -((t+b)/(t-b)),
			0, 0, -(2/(far-near)), -((far+near)/(far-near)),
			0, 0, 0, 1
		);
	};
	//Perspective projection
	//fov = vertical aspect ratio (radians)
	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far) {
		return ew::Mat4(
			(1 / (tan(fov / 2) * aspect)), 0, 0, 0,
			0, (1 / (tan(fov / 2))), 0, 0,
			0, 0, ((near+far)/(near-far)), ((2*far*near)/(near-far)),
			0, 0, -1, 1
		);
	};

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Euler angles (degrees)
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			ew::Mat4 rotate = hb::RotateY(ew::Radians(rotation.y))*  hb::RotateX(ew::Radians(rotation.x)) * hb::RotateZ(ew::Radians(rotation.z));
			return hb::Translate(position) * rotate * hb::Scale(scale);
		}
	};

}