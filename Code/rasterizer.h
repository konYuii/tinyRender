#pragma once

#include "vertexShader.h"
#include "pixelShader.h"
#include "light.h"

namespace render {

	class rasterizer {
	public:
		size_t bufferWidth;
		size_t bufferHeight;
		
		pixelShader* ps;

		Eigen::Vector3f cameraPos;

		bool zTest;
		std::vector<float> zbuffer;
		std::vector<Eigen::Vector3f> frameBuffer;

		rasterizer(size_t width, size_t height);
		void Rasterize(std::vector<VertexOut> in, PixelShaderType shader);
		void ClearBuffer();

	private:


		Eigen::Vector2f getScreenPos(VertexOut vo);
		inline size_t getBufferPos(size_t x, size_t y);
		bool insideTriangle(std::array<Eigen::Vector2f,3> v, Eigen::Vector2f pixel);
		std::array<float, 3> barycentricInterpolation(std::array<Eigen::Vector2f, 3> v, Eigen::Vector2f pixel);
		ShadingData InterpolateVertex(std::array<VertexOut, 3> v, std::array<float, 3> bary);
		inline Eigen::Vector3f Interpolate(std::array<Eigen::Vector3f, 3> vec, std::array<float, 3> bary);
		inline Eigen::Vector2f Interpolate(std::array<Eigen::Vector2f, 3> vec, std::array<float, 3> bary);
	};
}