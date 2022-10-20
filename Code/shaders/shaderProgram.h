#pragma once
#include"../model.h"

namespace render {

	class IShader {
	public:
		std::vector<Eigen::Vector4f> ST_Pos;
		std::vector<Eigen::Vector4f> clipST_Pos;

		virtual void VS(Indice* ,Mesh*) = 0;
		virtual Eigen::Vector3f PS() = 0;
		virtual void InterpolateValue(std::array<float, 3>, int) = 0;
		virtual void ClipBegin(){}
		virtual void ClipAddPoint(float, int, int){}
		virtual void ClipAddPoint(int){}
		virtual void ClipOver(){}
	};
}
