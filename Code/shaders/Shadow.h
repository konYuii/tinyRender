#pragma once

#include "../buffer.h"
#include "shaderProgram.h"

namespace render {

	struct ShadowUnifrom {
		Eigen::Matrix4f lightMatrix;
		Eigen::Matrix4f modelMatrix;

		DepthBuffer* shadowMap;
	};

	class Shadow : public IShader
	{
	public:
		ShadowUnifrom uniform;
		float pixelIn;

		void VS(Indice*, Mesh* mesh);
		Eigen::Vector3f PS(int x,int y);
		void InterpolateValue(std::array<float, 3> weight, int mid);

		void ClipAddPoint(float weight, int last, int cur);
		void ClipAddPoint(int);
		void ClipBegin();
		void ClipOver();
	};
}