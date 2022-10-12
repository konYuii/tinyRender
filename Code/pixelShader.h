#pragma once

#include "global.h"
#include "light.h"
#include "texture.h"

namespace render {

	enum class PixelShaderType
	{
		COLOR,BLINPHONG,LIGHT,SKYBOX,SHADOW,SHADOWTEST
	};

	struct ShadowMap {
		std::vector<float> zbuffer;
		int width;
		int height;
	};

	class pixelShader {
	public:
		std::vector<PointLight> pointLights;
		std::vector<DirectLight> directLights;
		ShadowMap shadowMap;
		Texture_loaded* textures;

		pixelShader();
		void SetTextures(Texture_loaded* ts);
		Eigen::Vector3f Shading(ShadingData, PixelShaderType shader);
	private:
		Eigen::Vector3f SampleTexture(int textureIndex, Eigen::Vector2f texcoord);
		Eigen::Vector3f SampleCubeMap(Eigen::Vector3f dir);
		


		Eigen::Vector3f ColorShading(ShadingData);
		Eigen::Vector3f BlinPhong(ShadingData);
		Eigen::Vector3f LightSource(ShadingData);

		Eigen::Vector3f SkyBox(ShadingData);

	};
}