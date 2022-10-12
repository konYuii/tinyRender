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

	void application::BeginRender(size_t bufferWidth, size_t bufferHeight)
	{
		this->rs->BeginRasterize(bufferWidth, bufferHeight);
		SetViewMatrix(camera->SetView());
		SetProjectionMatrix(camera->znear, camera->zfar, camera->aspect_ratio, camera->fov);
		this->rs->cameraPos = camera->pos;
	}
	void application::Rendering(Model* model, VertexShaderType vs, PixelShaderType ps)
	{
		for (auto& mesh : model->meshes)
		{
			//mesh.modelMatrix = mesh.modelMatrix * 3;
			Rendering(&mesh, vs, ps);
		}
	}
	void application::Rendering(Mesh* mesh, VertexShaderType vs, PixelShaderType ps)
	{


		this->rs->ps->SetTextures(mesh->textures_load);
		this->vs->SetModelMat(mesh->modelMatrix);
		if (ps == PixelShaderType::SKYBOX)
		{
			this->vs->clipOn = false;
			this->rs->zTest = false;
		}
		else
		{
			this->vs->clipOn = true;
			this->rs->zTest = true;
		}
		//std::cout << mesh->textures_load->data.size();
		for (auto& indice : mesh->indices)
		{
			std::vector<ShadingData> vout;
			this->vs->doVertex(indice, mesh, vout, vs);
			if (vout.size() == 0)
			{
				continue;
			}
			this->rs->Rasterize(vout, ps);
		}
	}


	Eigen::Matrix4f application::SetViewMatrix(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt)
	{
		return this->vs->SetViewMat(cameraPos, cameraUp, lookAt);
	}
	void application::SetViewMatrix(Eigen::Matrix4f view)
	{
		this->vs->SetViewMat(view);
	}
	void application::SetModelMatrix(Eigen::Matrix4f model)
	{
		this->vs->SetModelMat(model);
	}
	Eigen::Matrix4f application::SetProjectionMatrix(float znear, float zfar, float aspect_ratio, float fov)
	{
		return this->vs->SetProjectionMat(znear, zfar, aspect_ratio, fov);
	}

	Eigen::Matrix4f application::SetOrthoMatrix(float znear, float zfar, float width, float height)
	{
		return this->vs->SetOrthoMat(znear, zfar, width, height);
	}


	std::vector<Eigen::Vector3f>& application::GetFramebuffer()
	{
		return this->rs->frameBuffer;
	}
	void application::AddPointLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, float radius)
	{
		PointLight light;
		light.position = pos;
		light.intensity = intensity;
		light.radius = radius;

		this->rs->ps->pointLights.push_back(light);
	}
	void application::AddDirectionLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, Eigen::Vector3f direction, float width, float height, float znear, float zfar)
	{
		DirectLight light;
		light.position = pos;
		light.intensity = intensity;
		light.direction = direction;
		light.width = width;
		light.height = height;
		light.znear = znear;
		light.zfar = zfar;
		this->rs->ps->directLights.push_back(light);
	}
}