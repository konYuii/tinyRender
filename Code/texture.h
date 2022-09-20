#pragma once
#include "global.h"

namespace render {
	enum class TextureType
	{
		DIFFUSE,SPECULAR,NORMAL,HEIGHT
	};


	class Texture {
	public:
		std::string path;
		TextureType type;
		bool LoadTexture(const char* path, const std::string& directory);

		Eigen::Vector3f GetPixel(Eigen::Vector2f texcoord);

		void GernerateMipMap();
	private:
		cv::Mat image;
	};

	struct Texture_loaded {
		std::vector<Texture> data;
	};
}