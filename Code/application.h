#pragma once

#include "vertexShader.h"
#include "rasterizer.h"
#include "camera.h"

namespace render {


	class application {
	public:
		application(size_t bufferWidth, size_t bufferHeight, Camera* camera);

		void CameraControl(int);

		void Rendering(Model* model, Shader shader);
		void Rendering(Mesh* mesh, Shader shader);
		void ClearBuffer();

		void SetViewMatrix(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt);
		void SetViewMatrix(Eigen::Matrix4f view);
		void SetProjectionMatrix(float znear, float zfar, float aspect_ratio, float fov);
		std::vector<Eigen::Vector3f>& GetFramebuffer();

		void AddPointLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, float radius);
	private:
		vertexShader* vs;
		rasterizer* rs;
		Camera* camera;
	};
}