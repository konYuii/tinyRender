#include "rasterizer.h"

namespace render {
	rasterizer::rasterizer()
	{
		this->clipOn = true;
		this->zTest = true;
	}
	void rasterizer::BeginRasterize(size_t width, size_t height)
	{
		this->bufferWidth = width;
		this->bufferHeight = height;
		this->zbuffer.resize(height * width);
		this->frameBuffer.resize(height * width);
		for (int i = 0; i < this->zbuffer.size(); i++)
		{
			this->zbuffer[i] = -std::numeric_limits<float>::max();
			this->frameBuffer[i] = Eigen::Vector3f::Zero();
		}

	}
	void rasterizer::Rasterize(IShader* shader)
	{

		if (clipOn)
		{
			for (auto& plane : planes)
			{
				shader->ClipBegin();
				//std::cout << (int)plane << "   pre:" << shader->ST_Pos.size() << '\n';
				int size = shader->ST_Pos.size();
				for (int i = 0; i < size; i++)
				{
					Eigen::Vector4f cur = shader->ST_Pos[i];
					Eigen::Vector4f last = shader->ST_Pos[(size + i - 1) % size];
					//VectorPrint(cur);
					if (insidePlane(cur, plane))
					{
						if (!insidePlane(last, plane))
						{
							float weight = getClipRatio(last, cur, plane);
							weight = Clamp(weight, 0.0f, 1.0f);
							//std::cout << weight << '\n';
							shader->ClipAddPoint(weight, i, (size + i - 1) % size);
						}
						shader->ClipAddPoint(i);
					}
					else
					{
						if (insidePlane(last, plane))
						{
							float weight = getClipRatio(last, cur, plane);
							weight = Clamp(weight, 0.0f, 1.0f);
							shader->ClipAddPoint(weight, i, (size + i - 1) % size);
						}
					}
				}
				//std::cout << "out : " << shader->ST_Pos.size() << '\n';
				shader->ClipOver();
			}
		}

		std::vector<Eigen::Vector2f> screenPos;
		for (auto& vec : shader->ST_Pos)
		{
			vec /= vec.w() + EPS;
			Eigen::Vector2f pos = getScreenPos(vec);
			screenPos.push_back(pos);
		}


		float xmin, xmax, ymin, ymax;
		for (int i = 0; i < int(screenPos.size() - 2); i++)
		{

			std::array<Eigen::Vector2f, 3> triangle;
			triangle[0] = screenPos[0];
			triangle[1] = screenPos[i + 1];
			triangle[2] = screenPos[i + 2];


			xmin = std::min(triangle[0].x(), std::min(triangle[1].x(), triangle[2].x()));
			xmax = std::max(triangle[0].x(), std::max(triangle[1].x(), triangle[2].x()));
			ymin = std::min(triangle[0].y(), std::min(triangle[1].y(), triangle[2].y()));
			ymax = std::max(triangle[0].y(), std::max(triangle[1].y(), triangle[2].y()));

			xmin = std::max(xmin, 0.0f);
			ymin = std::max(ymin, 0.0f);
			xmax = std::min(xmax, static_cast<float>(bufferWidth));
			ymax = std::min(ymax, static_cast<float>(bufferHeight));


			//std::cout << "rasterizing!\n";
			for (int x = xmin; x < xmax; x++)
			{
				for (int y = ymin; y < ymax; y++)
				{
					float xpos = static_cast<float>(x) + 0.5f;
					float ypos = static_cast<float>(y) + 0.5f;

					Eigen::Vector2f pixelPos = Eigen::Vector2f(xpos, ypos);
					if (insideTriangle(triangle, pixelPos))
					{
						std::array<float, 3> weight = barycentricInterpolation(triangle, pixelPos);

						//透视矫正

						float zip;
						zip = 1.0f / (weight[0] / shader->ST_Pos[0].w() + weight[1] / shader->ST_Pos[i + 1].w() + weight[2] / shader->ST_Pos[i + 2].w());

						weight = std::array<float, 3>({ weight[0] * zip / shader->ST_Pos[0].w(),
													  weight[1] * zip / shader->ST_Pos[i + 1].w(),
													  weight[2] * zip / shader->ST_Pos[i + 2].w() });
						//std::cout << "weight：" << weight[0] << ' ' << weight[1] << ' ' << weight[2] << '\n';

						int bPos = getBufferPos(x, y);

						//std::cout << zip << "\n";
						if (bPos >= 0 && bPos < zbuffer.size() && zip > zbuffer[bPos])
						{
							if (zTest)
								this->zbuffer[bPos] = zip;

							shader->InterpolateValue(weight, i + 1);
							Eigen::Vector3f color = shader->PS();

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
	Eigen::Vector2f rasterizer::getScreenPos(Eigen::Vector4f vec)
	{

		Eigen::Vector2f pos = Eigen::Vector2f(vec.x(), vec.y());

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

	bool rasterizer::insidePlane(Eigen::Vector4f coord, ClipPlane plane)
	{
		switch (plane) {
		case ClipPlane::POSITIVE_W:
			return coord.w() <= -EPS;
		case ClipPlane::POSITIVE_X:
			return coord.x() >= +coord.w();
		case ClipPlane::NEGATIVE_X:
			return coord.x() <= -coord.w();
		case ClipPlane::POSITIVE_Y:
			return coord.y() >= +coord.w();
		case ClipPlane::NEGATIVE_Y:
			return coord.y() <= -coord.w();
		case ClipPlane::POSITIVE_Z:
			return coord.z() >= +coord.w();
		case ClipPlane::NEGATIVE_Z:
			return coord.z() <= -coord.w();
		default:
			assert(0);
			return 0;
		}
	}

	float rasterizer::getClipRatio(Eigen::Vector4f last, Eigen::Vector4f cur, ClipPlane plane)
	{
		switch (plane) {
		case ClipPlane::POSITIVE_W:
			return (last.w() - EPS) / (last.w() - cur.w());
		case ClipPlane::POSITIVE_X:
			return (last.w() - last.x()) / ((last.w() - last.x()) - (cur.w() - cur.x()));
		case ClipPlane::NEGATIVE_X:
			return (last.w() + last.x()) / ((last.w() + last.x()) - (cur.w() + cur.x()));
		case ClipPlane::POSITIVE_Y:
			return (last.w() - last.y()) / ((last.w() - last.y()) - (cur.w() - cur.y()));
		case ClipPlane::NEGATIVE_Y:
			return (last.w() + last.y()) / ((last.w() + last.y()) - (cur.w() + cur.y()));
		case ClipPlane::POSITIVE_Z:
			return (last.w() - last.z()) / ((last.w() - last.z()) - (cur.w() - cur.z()));
		case ClipPlane::NEGATIVE_Z:
			return (last.w() + last.z()) / ((last.w() + last.z()) - (cur.w() + cur.z()));
		default:
			assert(0);
			return 0;
		}
	}

}