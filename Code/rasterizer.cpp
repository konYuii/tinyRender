#include "rasterizer.h"

namespace render {
	rasterizer::rasterizer(size_t width, size_t height)
	{
		this->ps = new pixelShader();

		this->bufferHeight = height;
		this->bufferWidth = width;
		this->zbuffer.resize(height * width);
		this->frameBuffer.resize(height * width);
		for (int i = 0; i < this->zbuffer.size(); i++)
			this->zbuffer[i] = -std::numeric_limits<float>::max();
	}
	void rasterizer::Rasterize(std::vector<VertexOut> in ,Shader shader)
	{
		std::vector<Eigen::Vector2f> screenPos;
		//screenPos.resize(in.size());



		for (int i = 0; i < in.size(); i++)
		{

			Eigen::Vector2f pos = getScreenPos(in[i]);
			screenPos.push_back(pos);
		}


		float xmin, xmax, ymin, ymax;
		for (int i = 0; i < in.size() - 2; i++)
		{
			std::array<Eigen::Vector2f, 3> triangle;
			std::array<VertexOut, 3> inTriangle;
			triangle[0] = screenPos[0];
			inTriangle[0] = in[0];
			triangle[1] = screenPos[i + 1];
			inTriangle[1] = in[i + 1];
			triangle[2] = screenPos[i + 2];
			inTriangle[2] = in[i + 2];

			
			xmin = std::min(screenPos[0].x(), std::min(screenPos[i + 1].x(), screenPos[i + 2].x()));
			xmax = std::max(screenPos[0].x(), std::max(screenPos[i + 1].x(), screenPos[i + 2].x()));
			ymin = std::min(screenPos[0].y(), std::min(screenPos[i + 1].y(), screenPos[i + 2].y()));
			ymax = std::max(screenPos[0].y(), std::max(screenPos[i + 1].y(), screenPos[i + 2].y()));

			xmin = std::max(xmin, 0.0f);
			ymin = std::max(ymin, 0.0f);
			xmax = std::min(xmax, static_cast<float>(bufferWidth));
			ymax = std::min(ymax, static_cast<float>(bufferHeight));

			if (xmin == 0.0f && xmax == 700.0f)
			{
				std::cout << "1：\n";
				VectorPrint(inTriangle[0].csPos);
				VectorPrint(inTriangle[0].hpos);
				std::cout << "2：\n";
				VectorPrint(inTriangle[1].csPos);
				VectorPrint(inTriangle[1].hpos);
				std::cout << "3：\n";
				VectorPrint(inTriangle[2].csPos);
				VectorPrint(inTriangle[2].hpos);
			}

			for (int x = xmin; x < xmax; x++)
			{
				for (int y = ymin; y < ymax; y++)
				{
					float xpos = static_cast<float>(x) + 0.5f;
					float ypos = static_cast<float>(y) + 0.5f;

					Eigen::Vector2f pixelPos = Eigen::Vector2f(xpos, ypos);
					if (insideTriangle(triangle, pixelPos))
					{
						std::array<float, 3> bary = barycentricInterpolation(triangle, pixelPos);

						//透视矫正
						//std::cout << "bary：" << bary[0] << ' ' << bary[1] << ' ' << bary[2] << '\n';
						float zip = 1.0f / (bary[0] / inTriangle[0].csPos.z() + bary[1] / inTriangle[1].csPos.z() + bary[2] / inTriangle[2].csPos.z());


						int bPos = getBufferPos(x, y);
						//std::cout << "z值：" << zip << '\n';
						//std::cout << "zbuffer：" << this->zbuffer[bPos];
						//if (zip > this->zbuffer[bPos])
						//	std::cout << "get\n";

						if (bPos >= 0 && bPos < zbuffer.size() && zip > zbuffer[bPos])
						{
							bary = std::array<float, 3>({ bary[0] * zip / inTriangle[0].csPos.z(),
														  bary[1] * zip / inTriangle[1].csPos.z(),
														  bary[2] * zip / inTriangle[2].csPos.z() });

							this->zbuffer[bPos] = zip;

							ShadingData pixelIn = InterpolateVertex(inTriangle, bary);
							pixelIn.cameraPos = cameraPos;
							//pixelIn.color = (pixelIn.normal + Eigen::Vector3f::Identity()) * 0.5f * 255.0f;
							pixelIn.textureIndex = inTriangle[0].textureIndex;

							Eigen::Vector3f color = ps->Shading(pixelIn, shader);
							//std::cout << "Shading颜色：";
							//VectorPrint(color);
							this->frameBuffer[bPos] = color;
						}

					}
				}
		}

		}
	}


	void rasterizer::ClearBuffer()
	{
		for (int i = 0; i < this->zbuffer.size(); i++)
		{
			this->zbuffer[i] = -std::numeric_limits<float>::max();
			this->frameBuffer[i] = Eigen::Vector3f::Zero();
		}

	}
	Eigen::Vector2f rasterizer::getScreenPos(VertexOut vo)
	{
		
		Eigen::Vector2f pos = Eigen::Vector2f(vo.hpos.x(), vo.hpos.y());

		pos.x() = ((pos.x() + 1.0f) * 0.5f) * static_cast<float>(bufferWidth);
		pos.y() = ((1.0f - pos.y()) * 0.5f) * static_cast<float>(bufferHeight);
		//VectorPrint(pos);
		//if (pos.x() > bufferWidth)
		//	VectorPrint(pos);
		return pos;
	}
	inline size_t rasterizer::getBufferPos(size_t x, size_t y)
	{

		return y * bufferWidth + x;
	}
	bool rasterizer::insideTriangle(std::array<Eigen::Vector2f, 3> v, Eigen::Vector2f pixel)
	{
		Eigen::Vector2f AB = v[1] - v[0];
		Eigen::Vector2f BC = v[2] - v[1];
		Eigen::Vector2f CA = v[0] - v[2];

		Eigen::Vector2f AP = pixel - v[0];
		Eigen::Vector2f BP = pixel - v[1];
		Eigen::Vector2f CP = pixel - v[2];

		float z1 = AB.x() * AP.y() - AB.y() * AP.x();
		float z2 = BC.x() * BP.y() - BC.y() * BP.x();
		float z3 = CA.x() * CP.y() - CA.y() * CP.x();
		
		//std::cout << "向量：" << z1 << ' ' << z2 << ' ' << z3 << '\n';


		if ((z1 >= 0 && z2 >= 0 && z3 >= 0) || (z1 <= 0 && z2 <= 0 && z3 <= 0))
			return true;

		return false;
	}
	std::array<float, 3> rasterizer::barycentricInterpolation(std::array<Eigen::Vector2f, 3> v, Eigen::Vector2f pixel)
	{
		Eigen::Vector2f BP = pixel - v[1];
		Eigen::Vector2f CP = pixel - v[2];

		Eigen::Vector2f BC = v[2] - v[1];
		Eigen::Vector2f CA = v[0] - v[2];

		float s2BPC = BC.x() * BP.y() - BC.y() * BP.x();
		float s2APC = CA.x() * CP.y() - CA.y() * CP.x();

		float s2ABC = BC.x() * CA.y() - BC.y() * CA.x();

		float alpha = s2BPC / s2ABC;
		float beta = s2APC / s2ABC;

		//VectorPrint(AC);
		//VectorPrint(AB);

		//保持系数的顺序
		return std::array<float, 3>({ alpha, beta, 1.0f - alpha - beta });
	}
	ShadingData rasterizer::InterpolateVertex(std::array<VertexOut, 3> v, std::array<float, 3> bary)
	{
		ShadingData out;
		out.normal = (Interpolate(std::array<Eigen::Vector3f, 3>({ v[0].normal,v[1].normal,v[2].normal }), bary));
		out.texcoord = Interpolate(std::array<Eigen::Vector2f, 3>({ v[0].texcoord,v[1].texcoord,v[2].texcoord }), bary);
		out.worldPos = Interpolate(std::array<Eigen::Vector3f, 3>({ v[0].worldPos,v[1].worldPos,v[2].worldPos }), bary);
		out.csPos = Interpolate(std::array<Eigen::Vector3f, 3>({ v[0].csPos,v[1].csPos,v[2].csPos }), bary);

		return out;
	}
	inline Eigen::Vector3f rasterizer::Interpolate(std::array<Eigen::Vector3f, 3> vec, std::array<float, 3> bary)
	{
		return vec[0] * bary[0] + vec[1] * bary[1] + vec[2] * bary[2];
	}
	inline Eigen::Vector2f rasterizer::Interpolate(std::array<Eigen::Vector2f, 3> vec, std::array<float, 3> bary)
	{
		return vec[0] * bary[0] + vec[1] * bary[1] + vec[2] * bary[2];
	}
}