#pragma once

#include<Eigen/Eigen>
#include<opencv2/opencv.hpp>
#include<vector>
#include<queue>
#include<string>
#include<limits>
#include<unordered_map>



namespace render
{
	constexpr float PI = 3.1415926;
	constexpr float EPS = 1e-4;
	static Eigen::Vector3f testColor = Eigen::Vector3f(123.0f, 191.0f, 234.0f);
	static Eigen::Vector3f errorColor = Eigen::Vector3f(255.0f, 182.0f, 193.0f);


	struct Vertex {
		Eigen::Vector3f position;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoords;

	};



	struct Indice {
		std::vector<int> v;
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