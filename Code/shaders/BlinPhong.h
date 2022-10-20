#pragma once
#include"../model.h"
#include"shaderProgram.h"

namespace render {
	struct BlinPhongV2F {
		Eigen::Vector3f WS_Pos;
		Eigen::Vector3f Normal;
		Eigen::Vector2f UV;
	};

	struct BlinPhongUniform {
		Eigen::Matrix4f modelMat;
		Eigen::Matrix4f viewMat;
		Eigen::Matrix4f projectMat;

	};

	class BlinPhong : public IShader
	{
	public:
		std::vector<BlinPhongV2F> vertexOut;
		std::vector<BlinPhongV2F> clipOut;
		BlinPhongV2F pixelIn;
		BlinPhongUniform uniform;
		
		void VS(Indice*, Mesh* mesh);
		Eigen::Vector3f PS();
		void InterpolateValue(std::array<float, 3> weight, int mid);
		void ClipAddPoint(float weight, int last, int cur);
		void ClipAddPoint(int);
		void ClipBegin();
		void ClipOver();
	};
}
