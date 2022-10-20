#pragma once

#include"global.h"

namespace render {
	class Camera {
	public:
		Eigen::Vector3f pos;
		Eigen::Vector3f up;
		Eigen::Vector3f lookAt;

		

		float moveSpeed;
		float rotateSpeed;
		Camera(Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f);
		void Move(Eigen::Vector3f move);
		void Rotate(int axis, int counterSide);

	private:
		Eigen::Matrix3f GetMat(Eigen::Vector3f Up, Eigen::Vector3f LookAt);
	};
}