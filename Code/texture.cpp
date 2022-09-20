#include "texture.h"

namespace render {
	bool Texture::LoadTexture(const char* path, const std::string& directory)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;


		this->image = cv::imread(filename);


		if (this->image.empty())
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			return false;
		}
		else
		{
			cv::cvtColor(this->image, this->image, cv::COLOR_RGB2BGR);
			return true;
		}
	}
	Eigen::Vector3f Texture::GetPixel(Eigen::Vector2f texcoord)
	{

		if (this->image.empty())
		{
			std::cout << "image empty!\n";
			return errorColor;
		}

		float u = std::min(0.99999f, std::max(0.00001f, texcoord.x()));
		float v = std::min(0.99999f, std::max(0.00001f, texcoord.y()));
		int x = u * this->image.rows;
		int y = v * this->image.cols;

		
		//std::cout << texture->image.cols;
		//std::cout << x << ' ' << y << '\n';

		auto color = this->image.at<cv::Vec3b>(x, y);

		//std::cout << texture->image.size();
		return Eigen::Vector3f(color[0], color[1], color[2]);
	}
}
