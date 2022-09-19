#include "application.h"
#include "camera.h"
#include<opencv2/opencv.hpp>

size_t width = 700, height = 700;

float fov = 90.0f;
float cameraMoveSpeed = .15f;
float cameraRotateSpeed = render::PI / 60.0f;
float znear = 0.5f, zfar = 50.0f;
Eigen::Vector3f cameraPos = Eigen::Vector3f(0.0f, 0.0f, 2.0f);
Eigen::Vector3f cameraUp = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
Eigen::Vector3f cameraRight = Eigen::Vector3f(1.0f, 0.0f, 0.0f);

Eigen::Vector3f light1Pos = Eigen::Vector3f(-20.0, 20.0f, 0.0f);
Eigen::Vector3f light1Intensity = Eigen::Vector3f(1.0f, 1.0f, 1.0f) * 2.0f;

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



int main(){

	
	render::Camera* camera = new render::Camera(fov, width * 1.0 / height, znear, zfar);
	camera->pos = cameraPos;
	camera->up = cameraUp;
	camera->right = cameraRight;
	camera->moveSpeed = cameraMoveSpeed;
	camera->rotateSpeed = cameraRotateSpeed;

	render::Shader shader = render::Shader::Color;
	render::application app(width, height, camera);

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

	app.AddPointLight(light1Pos, light1Intensity, 1.0f);
	while (true)
	{
		
		int control = cv::waitKeyEx(10);
		if (control == 27)
			break;
		else
		{
			app.CameraControl(control);
		}

		//render::VectorPrint(camera->pos);
		//if(control>0)
		//	std::cout << control << '\n';
		app.ClearBuffer();


		//std::cout << control << '\n';
		//app.Rendering(&mesh);
		app.Rendering(&model,render::Shader::BlinPhong);


		cv::Mat image(width, height, CV_32FC3, app.GetFramebuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);



	}



	return 0;
}