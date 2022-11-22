#include "Shadow.h"

namespace render {
	void Shadow::VS(Indice* indice, Mesh* mesh)
	{
		Eigen::Matrix4f mvp = uniform.lightMatrix * uniform.modelMatrix;
		ST_Pos.resize(3);
		for (int i = 0; i < 3; i++)
		{
			Vertex& vertex = mesh->vertices[indice->v[i]];

			Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);

			ST_Pos[i] = mvp * hp;
		}

	}
	Eigen::Vector3f Shadow::PS(int x,int y)
	{
		int pos = y * uniform.shadowMap->width + x;
		

		if (uniform.shadowMap->data[pos] > pixelIn)
			uniform.shadowMap->data[pos] = pixelIn;

		float shade = pixelIn * 0.5f + 1.0f;
		//std::cout << shade << '\n';
		return Eigen::Vector3f(shade, shade, shade) * 255.0f;
	}
	void Shadow::InterpolateValue(std::array<float, 3> weight, int mid)
	{
		Eigen::Vector4f test = weight[0] * ST_Pos[0] + weight[1] * ST_Pos[mid] + weight[2] * ST_Pos[mid + 1];
		//VectorPrint(test);
		pixelIn = weight[0] * ST_Pos[0].z() + weight[1] * ST_Pos[mid].z() + weight[2] * ST_Pos[mid + 1].z();

	}
	void Shadow::ClipAddPoint(float weight, int last, int cur)
	{

		Eigen::Vector4f st_pos;
		st_pos = weight * ST_Pos[last] + (1.0f - weight) * ST_Pos[cur];
		clipST_Pos.push_back(st_pos);
	}
	void Shadow::ClipAddPoint(int index)
	{
		clipST_Pos.push_back(ST_Pos[index]);
	}
	void Shadow::ClipBegin()
	{
		clipST_Pos = std::vector<Eigen::Vector4f>();
	}
	void Shadow::ClipOver()
	{
		ST_Pos.swap(clipST_Pos);
	}
}