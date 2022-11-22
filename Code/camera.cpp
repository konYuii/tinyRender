#include"camera.h"
#include"tool.h"

namespace render {
	Camera::Camera(Eigen::Vector3f pos)
	{
		this->pos = pos;
		this->yaw = -90.0f;
		this->pitch = 0.0f;
	}
	void Camera::Move(Eigen::Vector3f shift)
	{
		this->pos += shift;
	}
	void Camera::Rotate(float dx, float dy)
	{
		yaw += dx;
		pitch += dy;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		float alpha = yaw * PI / 180.0f;
		float theta = pitch * PI / 180.0f;
		Eigen::Vector3f front;
		front.x() = cos(alpha) * cos(theta);
		front.y() = sin(theta);
		front.z() = sin(alpha) * cos(theta);
		this->lookAt = front.normalized();

		this->right = this->lookAt.cross(Eigen::Vector3f(0.0f, 1.0f, 0.0f));
		this->up = this->right.cross(this->lookAt);
	}

	Eigen::Matrix3f Camera::GetMat(Eigen::Vector3f Up,Eigen::Vector3f LookAt)
	{

		Eigen::Vector3f Right = LookAt.cross(Up);
		Eigen::Matrix3f trans;
		trans << Right.x(), Right.y(), Right.z(),
			Up.x(), Up.y(), Up.z(),
			-LookAt.x(), -LookAt.y(), -LookAt.z();
		

		return trans.transpose();

	}



}