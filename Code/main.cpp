#include "application.h"
#include "camera.h"
#include<opencv2/opencv.hpp>

size_t width = 700, height = 700;

float fov = 90.0f;
float cameraMoveSpeed = .15f;
float cameraRotateSpeed = render::PI / 60.0f;
float znear = 0.5f, zfar = 50.0f;


Eigen::Vector3f ver[] = { {-1.0f,-1.0,0.0f},{-1.0f,1.0f,0.0f},{1.0f,1.0f,0.0f},{1.0f,-1.0f,0.0f},
						{-1.0f,-1.0,-1.0f},{-1.0f,1.0f,-1.0f},{1.0f,1.0f,-1.0f},{1.0f,-1.0f,-1.0f} };
int Ind[] = { 0,1,2,
			  0,2,3,
			  0,4,5,
			  0,5,1,
			  1,5,6,
			  1,6,2,
			  2,6,7,
			  2,7,3,
			  3,7,4,
			  3,4,0,
			  4,5,6,
			  4,6,7};


void CameraHandle(int control, render::Camera* camera)
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

int main()
{
	render::Shader shader = render::Shader::Color;
	render::application app(width, height, shader);
	
	render::Camera* camera = new render::Camera(fov, width * 1.0 / height);
	camera->pos = Eigen::Vector3f(0.0f, 0.0f, 4.0f);
	camera->up = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
	camera->right = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
	camera->moveSpeed = cameraMoveSpeed;
	camera->rotateSpeed = cameraRotateSpeed;

	std::vector<render::Texture*> t;
	int len = (sizeof(Ind) / sizeof(int)) / 3;
	int verCnt = sizeof(ver) / sizeof(Eigen::Vector3f);
	std::vector<render::Vertex> vertex(verCnt);
	std::vector<render::Indice> indice(len);
	for (int i = 0; i < verCnt; i++)
	{
		vertex[i].position = ver[i];
		vertex[i].normal = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
		vertex[i].texcoords = Eigen::Vector2f::Zero();
	}
	for (int i = 0; i < len; i++)
	{
		indice[i].v.resize(3);
		for (int j = 0; j < 3; j++)
			indice[i].v[j] = Ind[i * 3 + j];
	}




	render::Mesh mesh(vertex, indice,  t);
	mesh.modelMatrix = Eigen::Matrix4f::Identity();

	std::string path = "../models/spot/spot.obj";
	render::Model model(path);



	while (true)
	{
		
		int control = cv::waitKeyEx(10);
		if (control == 27)
			break;
		else
		{
			//std::cout << control << "\n";
			CameraHandle(control, camera);
		}

		//render::VectorPrint(camera->pos);
		//if(control>0)
		//	std::cout << control << '\n';
		app.Clear();

		app.SetViewMatrix(camera->SetView());
		app.SetProjectionMatrix(znear, zfar, camera->aspect_ratio, camera->fov);


		//std::cout << control << '\n';
		//app.Rendering(&mesh);
		app.Rendering(&model);


		cv::Mat image(width, height, CV_32FC3, app.GetFramebuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);



	}



	return 0;
}