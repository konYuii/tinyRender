#pragma once

#include<Eigen/Eigen>
#include<opencv2/opencv.hpp>
#include<vector>
#include<queue>
#include<string>
#include<limits>



namespace render
{
	constexpr auto PI = 3.1415926;


	struct VertexOut {
		Eigen::Vector3f worldPos;
		Eigen::Vector3f csPos;
		Eigen::Vector4f hpos;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoord;
	};

	struct ShadingData {

		Eigen::Vector3f worldPos;
		Eigen::Vector3f csPos;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoord;

		Eigen::Vector3f color;
		int bufferPos;
	};

	struct Indice {
		std::vector<int> v;
	};


	enum class AA
	{
		None, SSAA, MSAA
	};

	enum class Shader
	{
		Color
	};

	inline void VectorPrint(Eigen::Vector2f v)
	{
		std::cout << v << "\n*******\n";
	}
	inline void VectorPrint(Eigen::Vector3f v)
	{
		std::cout << v << "\n*******\n";
	}
}