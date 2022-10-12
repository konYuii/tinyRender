#pragma once

#include "vertexShader.h"
#include "rasterizer.h"
#include "camera.h"

namespace render {


	class application {
	public:

		application(size_t bufferWidth, size_t bufferHeight, Camera* camera);

		void CameraControl(int);

		void BeginRender(size_t bufferWidth, size_t bufferHeight);


		void Rendering(Model* model, VertexShaderType vs ,PixelShaderType ps);
		void Rendering(Mesh* mesh, VertexShaderType vs, PixelShaderType ps);



		std::vector<Eigen::Vector3f>& GetFramebuffer();

		void AddPointLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, float radius);
		void AddDirectionLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, Eigen::Vector3f direction, float width, float height, float znear, float zfar);

	private:
		vertexShader* vs;
		rasterizer* rs;
		Camera* camera;

		Eigen::Matrix4f SetViewMatrix(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt);
		Eigen::Matrix4f SetProjectionMatrix(float znear, float zfar, float aspect_ratio, float fov);
		Eigen::Matrix4f SetOrthoMatrix(float znear, float zfar, float width, float height);
		void SetViewMatrix(Eigen::Matrix4f view);
		void SetModelMatrix(Eigen::Matrix4f model);

		
	};
}