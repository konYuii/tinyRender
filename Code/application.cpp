#include "application.h"

namespace render {

	application::application(size_t bufferWidth, size_t bufferHeight, Camera* camera)
	{
		this->rs = new rasterizer();

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
	void application::AddPointLight(Eigen::Vector3f pos, Eigen::Vector3f intensity, float radius)
	{
		PointLight light;
		light.position = pos;
		light.intensity = intensity;
		light.radius = radius;


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

	}
	void application::AddClipPlane(ClipPlane plane)
	{
		rs->planes.push_back(plane);
	}
}