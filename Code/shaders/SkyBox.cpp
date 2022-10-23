#include "SkyBox.h"

namespace render {
	void SkyBox::VS(Indice* indice, Mesh* mesh)
	{
		Eigen::Matrix4f viewInver = uniform.view.inverse();

		vertexOut.resize(3);
		ST_Pos.resize(3);

		for (int i = 0; i < 3; i++)
		{
			Vertex& vertex = mesh->vertices[indice->v[i]];
			Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);

			Eigen::Vector4f vec;
			vec = viewInver * hp;
			//VectorPrint(vec);
			vertexOut[i].WS_Pos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());
			ST_Pos[i] = uniform.project * hp;
		
		}

	}
	Eigen::Vector3f SkyBox::PS()
	{
		Eigen::Vector3f res;
		Eigen::Vector3f dir = (pixelIn.WS_Pos - uniform.cameraPos).normalized();
		//VectorPrint(dir);
		return SampleCubeMap(uniform.texture, dir);

	}
	void SkyBox::InterpolateValue(std::array<float, 3> weight, int mid)
	{
		pixelIn.WS_Pos = vertexOut[0].WS_Pos * weight[0] + vertexOut[mid].WS_Pos * weight[1] + vertexOut[mid + 1].WS_Pos * weight[2];
	}
}