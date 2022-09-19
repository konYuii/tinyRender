#include"camera.h"

namespace render {
	Camera::Camera(float fov, float ratio, float znear, float zfar)
	{
		this->fov = fov;
		this->aspect_ratio = ratio;
		this->znear = znear;
		this->zfar = zfar;

		this->viewInver = Eigen::Matrix4f::Identity();
		this->pos = Eigen::Vector3f();
		this->right = Eigen::Vector3f();
		this->up = Eigen::Vector3f();
	}
	void Camera::Move(Eigen::Vector3f move)
	{
		Eigen::Vector4f v(move.x(), move.y(), move.z(), 0.0f);
		v = this->viewInver * v;
		move = Eigen::Vector3f(v.x(), v.y(), v.z());
		this->pos += move * this->moveSpeed;
	}

	void Camera::Rotate(int axis, int counterSide)
	{
		float speed = rotateSpeed;
		if (counterSide)
			speed = -rotateSpeed;
		if (axis == 0)
		{
			Eigen::Vector4f newRight(cos(speed), 0.0f, sin(speed), 0.0f);
			newRight = this->viewInver * newRight;
			newRight.normalize();
			this->right = Eigen::Vector3f(newRight.x(), newRight.y(), newRight.z());
		}
		else if (axis == 1)
		{
			Eigen::Vector4f newUp(0.0f, cos(speed), sin(speed), 0.0f);
			newUp = this->viewInver * newUp;
			newUp.normalize();
			if(std::abs(newUp.dot(Eigen::Vector4f(0.0f,1.0f,0.0f,0.0f))) > 0.9f)
				this->up = Eigen::Vector3f(newUp.x(), newUp.y(), newUp.z());
		}

	}

	Eigen::Matrix4f Camera::SetView()
	{
		Eigen::Matrix4f move;
		move << 1.0f, 0.0f, 0.0f, -pos.x(),
			0.0f, 1.0f, 0.0f, -pos.y(),
			0.0f, 0.0f, 1.0f, -pos.z(),
			0.0f, 0.0f, 0.0f, 1.0f;


		Eigen::Matrix4f view;

		Eigen::Vector3f lookAt = this->up.cross(this->right);
		view << right.x(), right.y(), right.z(), 0.0f,
			up.x(), up.y(), up.z(), 0.0f,
			-lookAt.x(), -lookAt.y(), -lookAt.z(), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;
		
		this->view = view * move;
		this->viewInver = this->view.inverse();

		return this->view;
	}


}