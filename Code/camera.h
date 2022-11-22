#pragma once

#include"global.h"

namespace render {
	class Camera {
	public:
		Eigen::Vector3f pos;
		Eigen::Vector3f up;
		Eigen::Vector3f lookAt;
		Eigen::Vector3f right;

		

		float yaw;
		float pitch;
		Camera(Eigen::Vector3f);
		void Move(Eigen::Vector3f shift);
		void Rotate(float alpha, float theta);

	private:
		Eigen::Matrix3f GetMat(Eigen::Vector3f Up, Eigen::Vector3f LookAt);
	};
}