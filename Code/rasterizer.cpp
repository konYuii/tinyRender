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
		this->zTest = true;
	}
	void rasterizer::BeginRasterize(size_t width, size_t height)
	{
		this->zbuffer.resize(height * width);
		this->frameBuffer.resize(height * width);
		for (int i = 0; i < this->zbuffer.size(); i++)
		{
			this->zbuffer[i] = -std::numeric_limits<float>::max();
			this->frameBuffer[i] = Eigen::Vector3f::Zero();
		}

	}
	void rasterizer::Rasterize(std::vector<ShadingData> in ,PixelShaderType shader)
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
			std::array<ShadingData, 3> inTriangle;
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


			//VectorPrint(inTriangle[0].ST_Pos);

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

						float zip;
						zip = 1.0f / (bary[0] / inTriangle[0].ST_Pos.w() + bary[1] / inTriangle[1].ST_Pos.w() + bary[2] / inTriangle[2].ST_Pos.w());
					
						bary = std::array<float, 3>({ bary[0] * zip / inTriangle[0].ST_Pos.w(),
													  bary[1] * zip / inTriangle[1].ST_Pos.w(),
													  bary[2] * zip / inTriangle[2].ST_Pos.w() });
						//std::cout << "bary：" << bary[0] << ' ' << bary[1] << ' ' << bary[2] << '\n';
						
						int bPos = getBufferPos(x, y);


						if (bPos >= 0 && bPos < zbuffer.size() && zip > zbuffer[bPos])
						{
							if(zTest)
								this->zbuffer[bPos] = zip;
							ShadingData pixelIn = InterpolateVertex(inTriangle, bary);

							Eigen::Vector3f color = ps->Shading(pixelIn, shader);

							this->frameBuffer[bPos] = color;
							//VectorPrint(color);
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
	Eigen::Vector2f rasterizer::getScreenPos(ShadingData vo)
	{
		
		Eigen::Vector2f pos = Eigen::Vector2f(vo.ST_Pos.x(), vo.ST_Pos.y());

		pos.x() = ((pos.x() + 1.0f) * 0.5f) * static_cast<float>(bufferWidth);
		pos.y() = ((1.0f - pos.y()) * 0.5f) * static_cast<float>(bufferHeight);
		//VectorPrint(pos);

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
	ShadingData rasterizer::InterpolateVertex(std::array<ShadingData, 3> v, std::array<float, 3> bary)
	{
		ShadingData out;
		out.normal = (Interpolate(std::array<Eigen::Vector3f, 3>({ v[0].normal,v[1].normal,v[2].normal }), bary));
		out.texcoord = Interpolate(std::array<Eigen::Vector2f, 3>({ v[0].texcoord,v[1].texcoord,v[2].texcoord }), bary);
		out.WS_Pos = Interpolate(std::array<Eigen::Vector3f, 3>({ v[0].WS_Pos,v[1].WS_Pos,v[2].WS_Pos }), bary);
		out.cameraPos = cameraPos;
		out.textureIndex = v[0].textureIndex;
		
		out.LS_Pos = Interpolate(std::array<Eigen::Vector4f, 3>({ v[0].LS_Pos,v[1].LS_Pos,v[2].LS_Pos }), bary);
		//VectorPrint(out.LS_Pos);
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
	inline Eigen::Vector4f rasterizer::Interpolate(std::array<Eigen::Vector4f, 3> vec, std::array<float, 3> bary)
	{
		return vec[0] * bary[0] + vec[1] * bary[1] + vec[2] * bary[2];
	}
}