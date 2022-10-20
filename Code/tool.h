#pragma once

#include"global.h"
namespace render {
	static Eigen::Matrix4f GetProjectionMat(float znear, float zfar, float aspect_ratio, float fov)
	{
		znear = znear > 0 ? -znear : znear;
		zfar = zfar > 0 ? -zfar : zfar;

		Eigen::Matrix4f project;

		Eigen::Matrix4f P2O;
		P2O << znear, 0.0f, 0.0f, 0.0f,
			0.0f, znear, 0.0f, 0.0f,
			0.0f, 0.0f, znear + zfar, -znear * zfar,
			0.0f, 0.0f, 1.0f, 0.0f;

		float yTop = tan(fov * PI / 360) * -znear;
		float xRight = yTop * aspect_ratio;

		Eigen::Matrix4f move;
		move << 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, -(znear + zfar) * 0.5f,
			0.0f, 0.0f, 0.0f, 1.0f;

		Eigen::Matrix4f scale;
		scale << 1.0f / xRight, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / yTop, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (-zfar + znear), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

		project = scale * move * P2O;
		return project;
	}
	static Eigen::Matrix4f GetOrthoMat(float znear, float zfar, float width, float height)
	{
		znear = znear > 0 ? -znear : znear;
		zfar = zfar > 0 ? -zfar : zfar;


		Eigen::Matrix4f project;
		project << 1.0f / width, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / height, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (-zfar + znear), -(znear + zfar) / (znear - zfar),
			0.0f, 0.0f, 0.0f, 1.0f;

		return project;
	}
	static Eigen::Matrix4f GetViewMat(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt)
	{
		Eigen::Matrix4f view;

		Eigen::Matrix4f move;
		move << 1.0f, 0.0f, 0.0f, -cameraPos.x(),
			0.0f, 1.0f, 0.0f, -cameraPos.y(),
			0.0f, 0.0f, 1.0f, -cameraPos.z(),
			0.0f, 0.0f, 0.0f, 1.0f;

		Eigen::Vector3f cameraRight = lookAt.cross(cameraUp);
		Eigen::Matrix4f project;
		project << cameraRight.x(), cameraRight.y(), cameraRight.z(), 0.0f,
			cameraUp.x(), cameraUp.y(), cameraUp.z(), 0.0f,
			-lookAt.x(), -lookAt.y(), -lookAt.z(), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

		view = project * move;
		return view;
	}

	static Eigen::Vector3f SampleTexture(int index, Eigen::Vector2f texcoord)
	{

		return Eigen::Vector3f::Zero();
	}

	static Eigen::Vector3f SampleCubeMap(Eigen::Vector3f dir)
	{
		Eigen::Vector3f res;

		Eigen::Vector3f len = Eigen::Vector3f(std::abs(dir.x()), std::abs(dir.y()), std::abs(dir.z()));

		Eigen::Vector2f texcoord;
		if (len.x() > len.y() && len.x() > len.z())
		{

			if (dir.x() >= 0)
			{
				texcoord = Eigen::Vector2f(dir.z(), dir.y()) * 0.5f / dir.x();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(0, texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(dir.z(), dir.y()) * 0.5f / dir.x();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f + texcoord.y());
				res = SampleTexture(1, texcoord);
			}

		}
		else if (len.y() > len.z())
		{

			if (dir.y() >= 0)
			{
				texcoord = Eigen::Vector2f(dir.x(), dir.z()) * 0.5f / dir.y();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(2, texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(dir.x(), dir.z()) * 0.5f / dir.y();
				texcoord = Eigen::Vector2f(0.5f - texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(3, texcoord);
			}

		}
		else
		{
			if (dir.z() >= 0)
			{
				texcoord = Eigen::Vector2f(-dir.x(), dir.y()) * 0.5f / dir.z();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(5, texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(-dir.x(), dir.y()) * 0.5f / dir.z();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f + texcoord.y());
				res = SampleTexture(4, texcoord);
			}

		}

		return res;
	}

	static float Clamp(float x, float min, float max)
	{
		if (x >= min && x <= max)
			return x;
		else if (x < min)
			return min;
		else
			return max;
	}
}