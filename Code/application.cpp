#include "application.h"

namespace render {

	application::application(size_t bufferWidth, size_t bufferHeight)
	{
		this->rs = new rasterizer();

	}

	void application::BeginRender(size_t bufferWidth, size_t bufferHeight)
	{
		this->rs->BeginRasterize(bufferWidth, bufferHeight);
		this->rs->clipOn = true;
		this->rs->zTest = true;
	}
	void application::Rendering(Model* model, IShader* shader)
	{
		for (auto& mesh : model->meshes)
		{
			//mesh.modelMatrix = mesh.modelMatrix * 3;
			Rendering(&mesh, shader);
		}
	}
	void application::Rendering(Mesh* mesh, IShader* shader)
	{


		//std::cout << mesh->textures_load->data.size();
		for (auto& indice : mesh->indices)
		{
			//std::cout << "round: cnt" << '\n';
			shader->VS(&indice, mesh);
			if (shader->ST_Pos.size() == 0)
			{
				continue;
			}
			this->rs->Rasterize(shader);
		}
	}



	std::vector<Eigen::Vector3f>& application::GetFramebuffer()
	{
		return this->rs->frameBuffer;
	}

	void application::AddClipPlane(ClipPlane plane)
	{
		rs->planes.push_back(plane);
	}
	void application::ClipOn(bool clip)
	{
		this->rs->clipOn = clip;
	}
	void application::ZTestOn(bool ztest)
	{
		this->rs->zTest = ztest;
	}
}