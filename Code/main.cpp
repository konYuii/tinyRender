#include "application.h"
#include "camera.h"
#include<opencv2/opencv.hpp>

size_t width = 700, height = 700;
#pragma region 相机参数
float fov = 90.0f;
float cameraMoveSpeed = .15f;
float cameraRotateSpeed = render::PI / 60.0f;
float znear = -0.5f, zfar = -50.0f;
Eigen::Vector3f cameraPos = Eigen::Vector3f(0.0f, 0.0f, 4.0f);
Eigen::Vector3f cameraUp = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
Eigen::Vector3f cameraRight = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
#pragma endregion


#pragma region 光源参数
Eigen::Vector3f light1Pos = Eigen::Vector3f(-5.0f, 5.0f, 2.0f);
Eigen::Vector3f light1Intensity = Eigen::Vector3f(1.0f, 1.0f, 1.0f) * 20.0f;
Eigen::Vector3f light2Pos = Eigen::Vector3f(5.0f, 5.0f, -2.0f);
Eigen::Vector3f light2Intensity = Eigen::Vector3f(1.0f, 1.0f, 1.0f) * 20.0f;
Eigen::Vector3f lightSize = Eigen::Vector3f(0.2f, 0.2f, 0.2f);
#pragma endregion


#pragma region 立方体顶点
std::vector<Eigen::Vector3f> cubeVer = { {-1.0f,-1.0,0.5f},{-1.0f,1.0f,0.5f},{1.0f,1.0f,0.5f},{1.0f,-1.0f,0.5f},
						{-1.0f,-1.0,-0.5f},{-1.0f,1.0f,-0.5f},{1.0f,1.0f,-0.5f},{1.0f,-1.0f,-0.5f} };
std::vector<int> cubeInd = { 0,1,2,
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

#pragma endregion

#pragma region 平面顶点
std::vector<Eigen::Vector3f> planeVer = { {-0.5f,-0.5f,-0.5f},{-0.5f,0.5f,-0.5f},{0.5f,0.5f,-0.5f},{0.5f,-0.5f,-0.5f}};
std::vector<int> planeInd = { 0,1,2,
							  0,2,3 };
#pragma endregion


void LoadTexture(std::string path,std::string directory,render::Texture_loaded& load);
void LoadCubeMap(std::vector<std::string>& path, std::string directory, std::vector<int>& textures, render::Texture_loaded& load);
void BindVertex(std::vector<Eigen::Vector3f>& ver, std::vector<int>& Ind, std::vector<render::Vertex>& vertex, std::vector<render::Indice>& indice)
{
	int len = Ind.size() / 3;
	int verCnt = ver.size();
	
	vertex.resize(verCnt);
	indice.resize(len);
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
}

int main(){
	
	render::Camera* camera = new render::Camera(fov, width * 1.0 / height, znear, zfar);
	camera->pos = cameraPos;
	camera->up = cameraUp;
	camera->right = cameraRight;
	camera->moveSpeed = cameraMoveSpeed;
	camera->rotateSpeed = cameraRotateSpeed;


	render::application app(width, height, camera);

#pragma region 立方体顶点绑定
	std::vector<render::Vertex> cubeVertex;
	std::vector<render::Indice> cubeIndice;
	BindVertex(cubeVer, cubeInd, cubeVertex, cubeIndice);
	std::vector<int> t;
	render::Mesh light(cubeVertex, cubeIndice, t, nullptr);
#pragma endregion



#pragma region 天空盒
	std::vector<render::Vertex> planeVertex;
	std::vector<render::Indice> planeIndice;
	BindVertex(planeVer, planeInd, planeVertex, planeIndice);

	Eigen::Vector3f skyboxScale(1.0f, 1.0f, 1.0f);

	std::vector<int> skyBoxIndex;
	render::Texture_loaded skyBoxImg;
	std::vector<std::string> face{
	"right.jpg",
	"left.jpg",
	"top.jpg",
	"bottom.jpg",
	"front.jpg",
	"back.jpg" };
	LoadCubeMap(face, "../models/skybox", skyBoxIndex, skyBoxImg);
	render::Mesh skybox(planeVertex, planeIndice, skyBoxIndex, &skyBoxImg);

#pragma endregion







	std::string path = "../models/spot/spot.obj";
	render::Model model(path);
	//std::cout<<model.meshes.size()<<'\n';
	//std::cout << model.textures_model.size();
	//std::cout << model.meshes[0].vertices.size();

	app.AddPointLight(light1Pos, light1Intensity, 1.0f);
	app.AddPointLight(light2Pos, light2Intensity, 1.0f);

	bool first = true;
	while (true)
	{
		
		int control = cv::waitKeyEx(10);
		if (control == 27)
			break;
		else if(control != -1 || first)
		{
			first = false;
			app.CameraControl(control);
			app.BeginRender();

			skybox.SetModelMatrix(camera->pos, skyboxScale);
			app.Rendering(&skybox, render::VertexShaderType::SKYBOX,render::PixelShaderType::SKYBOX);


			app.Rendering(&model, render::VertexShaderType::MESH, render::PixelShaderType::BLINPHONG);
			light.SetModelMatrix(light1Pos, lightSize);
			app.Rendering(&light, render::VertexShaderType::MESH, render::PixelShaderType::LIGHT);
			light.SetModelMatrix(light2Pos, lightSize);
			app.Rendering(&light, render::VertexShaderType::MESH, render::PixelShaderType::LIGHT);


		}

		cv::Mat image(width, height, CV_32FC3, app.GetFramebuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);



	}



	return 0;
}


void LoadCubeMap(std::vector<std::string>& path, std::string directory, std::vector<int>& textures, render::Texture_loaded& load)
{
	for (auto& s : path)
	{
		render::Texture texture;
		texture.type = render::TextureType::CUBEMAP;
		texture.path = s;

		texture.LoadTexture(s.c_str(), directory);

		load.data.push_back(texture);
		textures.push_back(load.data.size() - 1);
	}
}