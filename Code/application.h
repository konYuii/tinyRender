#pragma once

#include "rasterizer.h"

namespace render {


	class application {
	public:

		rasterizer* rs;

		application(size_t bufferWidth, size_t bufferHeight);

		void BeginRender(size_t bufferWidth, size_t bufferHeight);


		void Rendering(Model* model, IShader* shader);
		void Rendering(Mesh* mesh, IShader* shader);



		std::vector<Eigen::Vector3f>& GetFramebuffer();


		void AddClipPlane(ClipPlane plane);

		void ClipOn(bool clip);
		void ZTestOn(bool ztest);
		void PerspectiveCorrectOn(bool correct);
	};
}