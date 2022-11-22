#pragma once
#include"../model.h"
#include"../light.h"
#include"../buffer.h"
#include"shaderProgram.h"

namespace render {
	struct BlinPhongV2F {
		Eigen::Vector3f WS_Pos;
		Eigen::Vector3f Normal;
		Eigen::Vector2f UV;
		Eigen::Vector4f LS_Pos;
	};

	struct BlinPhongUniform {
		Eigen::Matrix4f modelMat;
		Eigen::Matrix4f viewMat;
		Eigen::Matrix4f projectMat;
		Eigen::Matrix4f lightMat;

		Eigen::Vector3f ka;
		Eigen::Vector3f kd;
		Eigen::Vector3f ks;

		Eigen::Vector3f cameraPos;

		DirectLight sunLight;

		DepthBuffer* shadowMap;
		bool ztest;
	};

	class BlinPhong : public IShader
	{
	public:
		std::vector<BlinPhongV2F> vertexOut;
		std::vector<BlinPhongV2F> clipOut;
		BlinPhongV2F pixelIn;
		BlinPhongUniform uniform;
		
		void VS(Indice*, Mesh* mesh);
		Eigen::Vector3f PS(int x,int y);
		void InterpolateValue(std::array<float, 3> weight, int mid);
		void ClipAddPoint(float weight, int last, int cur);
		void ClipAddPoint(int);
		void ClipBegin();
		void ClipOver();
	};
}
