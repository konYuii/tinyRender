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
	static Eigen::Vector3f testColor = Eigen::Vector3f(123.0f, 191.0f, 234.0f);
	static Eigen::Vector3f errorColor = Eigen::Vector3f(255.0f, 182.0f, 193.0f);

	struct Vertex {
		Eigen::Vector3f position;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoords;

	};


	struct VertexOut {
		Eigen::Vector3f worldPos;
		Eigen::Vector3f csPos;
		Eigen::Vector4f hpos;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoord;

		int textureIndex;
	};

	struct ShadingData {

		Eigen::Vector3f worldPos;
		Eigen::Vector3f csPos;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoord;
		Eigen::Vector3f color;

		Eigen::Vector3f cameraPos;
		int textureIndex;
	};

	struct Indice {
		std::vector<int> v;
	};


	enum class AA
	{
		None, SSAA, MSAA
	};

	inline void VectorPrint(Eigen::Vector2f v)
	{
		std::cout << v << "\n*******\n";
	}
	inline void VectorPrint(Eigen::Vector3f v)
	{
		std::cout << v << "\n*******\n";
	}
	inline void VectorPrint(Eigen::Vector4f v)
	{
		std::cout << v << "\n*******\n";
	}
}