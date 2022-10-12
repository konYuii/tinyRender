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
	constexpr auto PI = 3.1415926;
	static Eigen::Vector3f testColor = Eigen::Vector3f(123.0f, 191.0f, 234.0f);
	static Eigen::Vector3f errorColor = Eigen::Vector3f(255.0f, 182.0f, 193.0f);


	struct Vertex {
		Eigen::Vector3f position;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoords;

	};

	//struct ShadingData {
	//	std::unordered_map<std::string, Eigen::Vector2f> vec2;
	//	std::unordered_map<std::string, Eigen::Vector3f> vec3;
	//	std::unordered_map<std::string, Eigen::Vector4f> vec4;
	//};
	struct ShadingData {
		int textureIndex;
		Eigen::Vector2f texcoord;
		Eigen::Vector3f WS_Pos;
		Eigen::Vector3f CS_Pos;
		Eigen::Vector4f ST_Pos;
		
		Eigen::Vector3f normal;

		Eigen::Vector3f cameraPos;
		Eigen::Vector4f LS_Pos;
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