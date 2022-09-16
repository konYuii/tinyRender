#include "application.h"

namespace render {
	application::application(size_t bufferWidth, size_t bufferHeight, Shader shader)
	{
		this->rs = new rasterizer(bufferWidth, bufferHeight);
		this->vs = new vertexShader();
		this->rs->ps = new pixelShader(shader);
	}
	void application::Rendering(Model* model)
	{
		for (auto& mesh : model->meshes)
		{
			//mesh.modelMatrix = mesh.modelMatrix * 3;
			Rendering(&mesh);
		}
	}
	void application::Rendering(Mesh* mesh)
	{
		for (auto& indice : mesh->indices)
		{
			std::vector<VertexOut> vout;
			this->vs->doVertex(indice, mesh, vout);
			if (vout.size() == 0)
			{
				continue;
			}
			this->rs->Rasterize(vout);
		}
	}


	void application::Clear()
	{
		this->rs->Clear();
	}
	void application::SetViewMatrix(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt)
	{
		this->vs->SetViewMat(cameraPos, cameraUp, lookAt);
	}
	void application::SetViewMatrix(Eigen::Matrix4f view)
	{
		this->vs->SetViewMat(view);
	}
	void application::SetProjectionMatrix(float znear, float zfar, float aspect_ratio, float fov)
	{
		this->vs->SetProjectionMat(znear, zfar, aspect_ratio, fov);
	}
	std::vector<Eigen::Vector3f>& application::GetFramebuffer()
	{
		return this->rs->frameBuffer;
	}
}