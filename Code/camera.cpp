#include"camera.h"

namespace render {
	Camera::Camera(Eigen::Vector3f pos, Eigen::Vector3f up, Eigen::Vector3f lookAt)
	{
		this->pos = pos;
		this->up = up;
		this->lookAt = lookAt;

		this->moveSpeed = 0.0f;
		this->rotateSpeed = 0.0f;
	}
	void Camera::Move(Eigen::Vector3f move)
	{
		move = GetMat(up,lookAt) * move;
		this->pos += move * this->moveSpeed;
	}

	void Camera::Rotate(int axis, int counterSide)
	{
		float speed = rotateSpeed;
		if (counterSide)
			speed = -rotateSpeed;
		if (axis == 0)
		{
			Eigen::Vector3f newRight(cos(speed), 0.0f, sin(speed));
			newRight = GetMat(Eigen::Vector3f(0.0f, 1.0f, 0.0f), Eigen::Vector3f(lookAt.x(), 0.0f, lookAt.z()).normalized()) * newRight;
			newRight.normalize();
			lookAt = up.cross(newRight);
		}
		else if (axis == 1)
		{
			Eigen::Vector3f newUp(0.0f, cos(speed), sin(speed));
			newUp = GetMat(up, lookAt) * newUp;
			newUp.normalize();
			if (newUp.y() > 0.9f)
			{
				up = newUp;
			}

		}

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