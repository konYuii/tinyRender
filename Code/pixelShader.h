#pragma once

#include "global.h"
#include "light.h"

namespace render {

	enum class Shader
	{
		Color,BlinPhong,Light
	};

	class pixelShader {
	public:
		std::vector<Light> lights;

		pixelShader();
		Eigen::Vector3f Shading(ShadingData, Shader shader);
	private:

		Eigen::Vector3f ColorShading(ShadingData);
		Eigen::Vector3f BlinPhong(ShadingData);
		Eigen::Vector3f LightSource(ShadingData);
	};
}