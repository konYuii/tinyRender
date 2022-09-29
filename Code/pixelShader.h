#pragma once

#include "global.h"
#include "light.h"
#include "texture.h"

namespace render {

	enum class PixelShaderType
	{
		COLOR,BLINPHONG,LIGHT,SKYBOX
	};

	class pixelShader {
	public:
		std::vector<Light> lights;
		Texture_loaded* textures;

		pixelShader();
		void SetTextures(Texture_loaded* ts);
		Eigen::Vector3f Shading(ShadingData, PixelShaderType shader);
	private:
		Eigen::Vector3f SampleTexture(int textureIndex, Eigen::Vector2f texcoord);


		Eigen::Vector3f ColorShading(ShadingData);
		Eigen::Vector3f BlinPhong(ShadingData);
		Eigen::Vector3f LightSource(ShadingData);

		Eigen::Vector3f SkyBox(ShadingData);
	};
}