#pragma once
#include"../model.h"
#include"shaderProgram.h"
#include "../tool.h"

namespace render {
	struct SkyBoxV2F {
		Eigen::Vector3f WS_Pos;
	};
	struct SkyBoxUniform {
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f project;

		Eigen::Vector3f cameraPos;
		Texture_loaded* texture;
	};


	class SkyBox :public IShader
	{
	public:
		std::vector<SkyBoxV2F> vertexOut;
		SkyBoxV2F pixelIn;

		SkyBoxUniform uniform;

		void VS(Indice* indice, Mesh* mesh);
		Eigen::Vector3f PS(int x,int y);
		void InterpolateValue(std::array<float, 3> weight, int mid);

	};

}