#pragma once

#include"global.h"

namespace render {
	class Camera {
	public:
		Eigen::Vector3f pos;
		Eigen::Vector3f up;
		Eigen::Vector3f right;

		float fov;
		float aspect_ratio;		
		float znear;
		float zfar;

		float moveSpeed;
		float rotateSpeed;

		Camera(float fov, float ratio, float znear, float zfar);
		void Move(Eigen::Vector3f move);
		void Rotate(int axis, int counterSide);
		Eigen::Matrix4f SetView();
	private:

		Eigen::Matrix4f viewInver;
		Eigen::Matrix4f view;

	};
}