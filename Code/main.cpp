#include "application.h"
#include "tool.h"
#include "shaders/shaderType.h"
#include "camera.h"
#include<opencv2/opencv.hpp>

size_t width = 700, height = 700;
#pragma region 相机参数
float fov = 90.0f;
float cameraMoveSpeed = .15f;
float cameraRotateSpeed = render::PI / 45.0f;
float znear = 0.5f, zfar = 500.0f;
Eigen::Vector3f cameraPos = Eigen::Vector3f(0.0f, 0.0f, 4.0f);
render::Camera* camera;

int screenX, screenY;
bool first = true;
#pragma endregion


#pragma region 光源参数
Eigen::Vector3f light1Pos = Eigen::Vector3f(-10.0f, 10.0f, 4.0f);
Eigen::Vector3f light1Intensity = Eigen::Vector3f(1.0f, 1.0f, 0.96f) / 2.0f;
Eigen::Vector3f light2Pos = Eigen::Vector3f(3.0f, 3.0f, 2.0f);
Eigen::Vector3f light2Intensity = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
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
			  4,6,7 };

#pragma endregion

#pragma region 平面顶点
std::vector<Eigen::Vector3f> planeVer = { {-0.5f,-0.5f,0.5f},{-0.5f,0.5f,0.5f},{0.5f,0.5f,0.5f},{0.5f,-0.5f,0.5f} };
std::vector<int> planeInd = { 0,1,2,
							  0,2,3 };
#pragma endregion


void LoadTexture(std::string path, std::string directory, render::Texture_loaded& load);
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

void KeyboardControl(int control);
void MouseControl(int even, int x, int y, int flags, void* param);


int main() {
	camera = new render::Camera(cameraPos);

#pragma region 渲染器的设置
	render::application app(width, height);

	app.AddClipPlane(render::ClipPlane::POSITIVE_X);
	app.AddClipPlane(render::ClipPlane::POSITIVE_Y);
	app.AddClipPlane(render::ClipPlane::NEGATIVE_X);
	app.AddClipPlane(render::ClipPlane::NEGATIVE_Y);
	app.AddClipPlane(render::ClipPlane::POSITIVE_Z);
	app.AddClipPlane(render::ClipPlane::NEGATIVE_Z);

#pragma endregion



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

	render::SkyBox skyboxShader;
	skyboxShader.uniform.texture = skybox.textures_load;
#pragma endregion


#pragma region Spot初始化
	std::string spotPath = "../models/spot/floor_spot.obj";
	render::Model spot(spotPath);

	render::BlinPhong spotShader;
	spotShader.uniform.ka = Eigen::Vector3f(240.0f, 255.0f, 240.0f);
	spotShader.uniform.kd = Eigen::Vector3f(238.0f, 238.0f, 209.0f);
	spotShader.uniform.ks = Eigen::Vector3f(151.0f, 255.0f, 255.0f);
	spotShader.uniform.sunLight.direction = -(light1Pos.normalized());
	spotShader.uniform.sunLight.intensity = light1Intensity;
#pragma endregion



#pragma region 阴影设置
	Eigen::Matrix4f lightMatrix = render::GetOrthoMat(0.5f, 100.0f, 5.0f, 5.0f)
		* render::GetViewMat(light1Pos, render::Normal2Tangent(-light1Pos), -light1Pos);

	render::DepthBuffer shadowMap(width, height);
	render::Shadow shadowShader;
	shadowShader.uniform.lightMatrix = lightMatrix;
	app.BeginRender(width, height);
	app.ClipOn(true);
	app.ZTestOn(true);
	shadowShader.uniform.shadowMap = &shadowMap;

	shadowShader.uniform.modelMatrix = spot.modelMatrix;
	app.Rendering(&spot, &shadowShader);
	//shadowShader.uniform.modelMatrix = slope.modelMatrix;
	//app.Rendering(&slope, &shadowShader);

	spotShader.uniform.shadowMap = &shadowMap;
	spotShader.uniform.lightMat = shadowShader.uniform.lightMatrix;
	spotShader.uniform.ztest = true;
#pragma endregion


#pragma region 渲染主循环
	cv::namedWindow("soft render");
	cv::setMouseCallback("soft render", MouseControl, 0);
	while (true)
	{
		int control = cv::waitKey(10);
		if (control == 27)
			break;
		KeyboardControl(control);


		app.BeginRender(width, height);

		Eigen::Matrix4f view = render::GetViewMat(camera->pos, camera->up, camera->lookAt);

#pragma region 渲染天空盒
		//app.ZTestOn(false);
		//app.ClipOn(false);
		//skybox.SetModelMatrix(camera->pos, skyboxScale);
		//skyboxShader.uniform.view = view;
		//skyboxShader.uniform.project = render::GetProjectionMat(znear, zfar, width * 1.0f / height, fov);
		//skyboxShader.uniform.cameraPos = camera->pos;
		//app.Rendering(&skybox, &skyboxShader);

#pragma endregion



#pragma region 渲染模型
		app.ZTestOn(true);
		app.ClipOn(true);
		spotShader.uniform.modelMat = spot.modelMatrix;
		spotShader.uniform.viewMat = view;
		//spotShader.uniform.projectMat = render::GetOrthoMat(znear, zfar, 2.0f, 2.0f);
		spotShader.uniform.projectMat = render::GetProjectionMat(znear, zfar, width * 1.0f / height, fov);
		spotShader.uniform.cameraPos = camera->pos;
		spotShader.uniform.lightMat = lightMatrix;
		spotShader.uniform.ztest = true;
		app.Rendering(&spot, &spotShader);
#pragma endregion


		cv::Mat image(width, height, CV_32FC3, app.GetFramebuffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("soft render", image);
	}


#pragma endregion



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

void KeyboardControl(int control)
{
	if (control == 119) //w
		camera->Move(camera->lookAt * cameraMoveSpeed);
	else if (control == 115) //s
		camera->Move(-camera->lookAt * cameraMoveSpeed);
	else if (control == 97) //a
		camera->Move(-camera->right * cameraMoveSpeed);
	else if (control == 100) //d
		camera->Move(camera->right * cameraMoveSpeed);
}

void MouseControl(int even, int x, int y, int flags, void* param)
{
	if (even == cv::EVENT_MOUSEMOVE)
	{
		if (first)
		{
			first = false;
			screenX = x;
			screenY = y;
			return;
		}

		float dx = (x - screenX) * cameraRotateSpeed;
		float dy = (screenY - y) * cameraRotateSpeed;
		screenX = x;
		screenY = y;

		camera->Rotate(dx, dy);
	}

}
