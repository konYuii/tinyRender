#pragma once

#include"global.h"
#include"texture.h"
#include"buffer.h"
namespace render {
	static Eigen::Matrix4f GetProjectionMat(float znear, float zfar, float aspect_ratio, float fov)
	{

		Eigen::Matrix4f project;

		Eigen::Matrix4f P2O;
		P2O << znear, 0.0f, 0.0f, 0.0f,
			0.0f, znear, 0.0f, 0.0f,
			0.0f, 0.0f, znear + zfar, -znear * zfar,
			0.0f, 0.0f, 1.0f, 0.0f;

		float yTop = tan(fov * PI / 360) * znear;
		float xRight = yTop * aspect_ratio;

		Eigen::Matrix4f move;
		move << 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, -(znear + zfar) * 0.5f,
			0.0f, 0.0f, 0.0f, 1.0f;

		Eigen::Matrix4f scale;
		scale << 1.0f / xRight, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / yTop, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (zfar - znear), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

		project = scale * move * P2O;
		return project;
	}
	static Eigen::Matrix4f GetOrthoMat(float znear, float zfar, float width, float height)
	{


		Eigen::Matrix4f project;
		project << 1.0f / width, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / height, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (zfar - znear), -(znear + zfar) / (zfar - znear),
			0.0f, 0.0f, 0.0f, 1.0f;

		return project;
	}
	static Eigen::Matrix4f GetViewMat(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt)
	{
		cameraUp.normalize();
		lookAt.normalize();

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
			lookAt.x(), lookAt.y(), lookAt.z(), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

		view = project * move;
		return view;
	}

	static Eigen::Vector3f Normal2Tangent(Eigen::Vector3f normal)
	{
		normal.normalize();
		float mi = sqrt(normal.x() * normal.x() + normal.z() * normal.z());
		Eigen::Vector3f tangent = Eigen::Vector3f(normal.x() * normal.y() / mi, -mi, normal.z() * normal.y() / mi);
		//VectorPrint(tangent);
		//std::cout << tangent.dot(normal);

		return tangent;
	}

	static Eigen::Vector3f SampleTexture(Texture texture, Eigen::Vector2f texcoord)
	{
		return texture.GetPixel(texcoord);
	}

	static Eigen::Vector3f SampleCubeMap(Texture_loaded* textures, Eigen::Vector3f dir)
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
				res = SampleTexture(textures->data[0], texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(dir.z(), dir.y()) * 0.5f / dir.x();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f + texcoord.y());
				res = SampleTexture(textures->data[1], texcoord);
			}

		}
		else if (len.y() > len.z())
		{

			if (dir.y() >= 0)
			{
				texcoord = Eigen::Vector2f(dir.x(), dir.z()) * 0.5f / dir.y();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(textures->data[2], texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(dir.x(), dir.z()) * 0.5f / dir.y();
				texcoord = Eigen::Vector2f(0.5f - texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(textures->data[3], texcoord);
			}

		}
		else
		{
			if (dir.z() >= 0)
			{
				texcoord = Eigen::Vector2f(-dir.x(), dir.y()) * 0.5f / dir.z();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f - texcoord.y());
				res = SampleTexture(textures->data[5], texcoord);
			}

			else
			{
				texcoord = Eigen::Vector2f(-dir.x(), dir.y()) * 0.5f / dir.z();
				texcoord = Eigen::Vector2f(0.5f + texcoord.x(), 0.5f + texcoord.y());
				res = SampleTexture(textures->data[4], texcoord);
			}

		}

		return res;
	}

	static float SampleDepthBuffer(DepthBuffer* buffer, Eigen::Vector3f coord)
	{
		int x = (coord.x() * 0.5f + 0.5f) * static_cast<float>(buffer->width);
		int y = (-0.5f * coord.y() + 0.5f) * static_cast<float>(buffer->height);

		int index = y * buffer->width + x;

		//std::cout << "×ø±ê£º" << x << ' ' << y << "\tÉî¶È£º" << buffer->data[index] << '\n';
		if (index >= 0 && index < buffer->data.size() && x >= 0 && x < buffer->width && y > 0 && y < buffer->height)
		{
			if (coord.z() > buffer->data[index] + 1e-2)
			{
				return 1.0f;
			}
			return 0.0f;
		}
		return 0.0f;
	}

	static float SampleDepthBufferPCF(DepthBuffer* buffer, Eigen::Vector3f coord)
	{
		int x = (coord.x() * 0.5f + 0.5f) * static_cast<float>(buffer->width);
		int y = (-0.5f * coord.y() + 0.5f) * static_cast<float>(buffer->height);

		float shadow = 0.0f;
		for (int i = x - 3; i <= x + 3; i++)
		{
			for (int j = y - 3; j <= y + 3; j++)
			{
				int index = j * buffer->width + i;
				if (index >= 0 && index < buffer->data.size() && x >= 0 && x < buffer->width && y > 0 && y < buffer->height)
				{
					if (coord.z() > buffer->data[index] + 0.002f)
					{
						shadow += 1.0f;
					}
				}
			}
		}

		shadow /= 9.0f;

		return shadow;
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

	static float Sine(float x)
	{

		const float B = 4 / PI;

		const float C = -4 / (PI * PI);

		float y = B * x + C * x * abs(x);

		const float P = 0.225;

		y = P * (y * abs(y) - y) + y;

		return y;
	}
	static float Cose(float x)
	{
		x += PI * 0.5f;
		if (x > PI)
			x -= 2.0f * PI;
		return Sine(x);
	}
}