#pragma once

#include "vertexShader.h"
#include "rasterizer.h"

namespace render {


	class application {
	public:
		application(size_t bufferWidth, size_t bufferHeight, Shader shader);
		void Rendering(Model* model);
		void Rendering(Mesh* mesh);
		void Clear();

		void SetViewMatrix(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt);
		void SetViewMatrix(Eigen::Matrix4f view);
		void SetProjectionMatrix(float znear, float zfar, float aspect_ratio, float fov);
		std::vector<Eigen::Vector3f>& GetFramebuffer();
	private:
		vertexShader* vs;
		rasterizer* rs;

	};
}