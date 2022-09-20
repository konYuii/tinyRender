#include "application.h"

namespace render {
	application::application(size_t bufferWidth, size_t bufferHeight, Camera* camera)
	{
		this->rs = new rasterizer(bufferWidth, bufferHeight);
		this->vs = new vertexShader();

		this->camera = camera;
	}
	void application::CameraControl(int control)
	{
		if (control == 119) //w
			camera->Move(Eigen::Vector3f(0.0f, 0.0f, -1.0f));
		else if (control == 115) //s
			camera->Move(Eigen::Vector3f(0.0f, 0.0f, 1.0f));
		else if (control == 97) //a
			camera->Move(Eigen::Vector3f(-1.0f, 0.0f, 0.0f));
		else if (control == 100) //d
			camera->Move(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
		else if (control == 2424832) //<-
			camera->Rotate(0, 1);
		else if (control == 2555904) //->
			camera->Rotate(0, 0);
		else if (control == 2490368)
			camera->Rotate(1, 0);
		else if (control == 2621440)
			camera->Rotate(1, 1);
	}
	void application::Rendering(Model* model, Shader shader)
	{
		SetViewMatrix(camera->SetView());
		SetProjectionMatrix(camera->znear, camera->zfar, camera->aspect_ratio, camera->fov);
		this->rs->cameraPos = camera->pos;

		for (auto& mesh : model->meshes)
		{
			//mesh.modelMatrix = mesh.modelMatrix * 3;
			Rendering(&mesh, shader);
		}
	}
	void application::Rendering(Mesh* mesh, Shader shader)
	{
		this->rs->ps->SetTextures(mesh->textures_load);
		//std::cout << mesh->textures_load->data.size();
		for (auto& indice : mesh->indices)
		{
			std::vector<VertexOut> vout;
			this->vs->doVertex(indice, mesh, vout);
			if (vout.size() == 0)
			{
				continue;
			}
			this->rs->Rasterize(vout, shader);
		}
	}


	void application::ClearBuffer()
	{
		this->rs->ClearBuffer();
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
	void application::AddPointLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, float radius)
	{
		Light light;
		light.type = LightType::PointLight;
		light.position = pos;
		light.intensity = intensity;
		light.radius = radius;

		this->rs->ps->lights.push_back(light);
	}
}