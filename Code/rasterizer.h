#pragma once

#include "global.h"
#include "light.h"
#include "shaders/shaderProgram.h"
#include "tool.h"

namespace render {
	enum class ClipPlane
	{
		POSITIVE_X,NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z,POSITIVE_W
	};

	class rasterizer {
	public:
		size_t bufferWidth;
		size_t bufferHeight;
		


		bool zTest;
		bool clipOn;
		std::vector<ClipPlane> planes;

		std::vector<float> zbuffer;
		std::vector<Eigen::Vector3f> frameBuffer;

		rasterizer();
		void BeginRasterize(size_t bufferWidth, size_t bufferHeight);
		void Rasterize(IShader* shader);
		void ClearBuffer();

	private:


		Eigen::Vector2f getScreenPos(Eigen::Vector4f vec);
		inline size_t getBufferPos(size_t x, size_t y);
		bool insideTriangle(std::array<Eigen::Vector2f,3> v, Eigen::Vector2f pixel);
		std::array<float, 3> barycentricInterpolation(std::array<Eigen::Vector2f, 3> v, Eigen::Vector2f pixel);

		void clip(IShader* shader, ClipPlane plane);
		bool insidePlane(Eigen::Vector4f vec, ClipPlane plane);
		float getClipRatio(Eigen::Vector4f last, Eigen::Vector4f cur, ClipPlane plane);

	};
}