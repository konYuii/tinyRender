#pragma once

#include "rasterizer.h"
#include "camera.h"

namespace render {


	class application {
	public:

		rasterizer* rs;
		Camera* camera;

		application(size_t bufferWidth, size_t bufferHeight, Camera* camera);

		void CameraControl(int);

		void BeginRender(size_t bufferWidth, size_t bufferHeight);


		void Rendering(Model* model, IShader* shader);
		void Rendering(Mesh* mesh, IShader* shader);



		std::vector<Eigen::Vector3f>& GetFramebuffer();

		void AddPointLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, float radius);
		void AddDirectionLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, Eigen::Vector3f direction, float width, float height, float znear, float zfar);

		void AddClipPlane(ClipPlane plane);
	};
}