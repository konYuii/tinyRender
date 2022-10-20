#include "BlinPhong.h"

namespace render {
	void BlinPhong::VS(Indice* indice, Mesh* mesh)
	{
		Eigen::Matrix4f mvpMat = uniform.projectMat * uniform.viewMat * uniform.modelMat;
		Eigen::Matrix4f normalMat = uniform.modelMat.inverse().transpose();
		vertexOut.resize(3);
		ST_Pos.resize(3);
		for (int i = 0; i < 3; i++)
		{
			Vertex& vertex = mesh->vertices[indice->v[i]];

			vertexOut[i].UV = vertex.texcoords;

			Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);
			Eigen::Vector4f hn = Eigen::Vector4f(vertex.normal.x(), vertex.normal.y(), vertex.normal.z(), 1.0f);

			Eigen::Vector4f vec;
			vec = uniform.modelMat * hp;
			vertexOut[i].WS_Pos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());
			ST_Pos[i] = mvpMat * hp;

			vec = normalMat * hn;
			vertexOut[i].Normal = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		}
		


	}
	Eigen::Vector3f BlinPhong::PS()
	{
		Eigen::Vector3f res;
		res = Eigen::Vector3f(pixelIn.Normal.x() *0.5f + 0.5f, pixelIn.Normal.y() * 0.5f + 0.5f, pixelIn.Normal.z() * 0.5f + 0.5f);
		//res = Eigen::Vector3f(pixelIn.WS_Pos.x() * 0.5f + 0.5f, pixelIn.WS_Pos.y() * 0.5f + 0.5f, pixelIn.WS_Pos.z() * 0.5f + 0.5f);
		return res * 255.0f;
	}
	void BlinPhong::InterpolateValue(std::array<float, 3> weight, int mid)
	{
		pixelIn.Normal = vertexOut[0].Normal * weight[0] + vertexOut[mid].Normal * weight[1] + vertexOut[mid + 1].Normal * weight[2];
		pixelIn.UV = vertexOut[0].UV * weight[0] + vertexOut[mid].UV * weight[1] + vertexOut[mid + 1].UV * weight[2];
		pixelIn.WS_Pos = vertexOut[0].WS_Pos * weight[0] + vertexOut[mid].WS_Pos * weight[1] + vertexOut[mid + 1].WS_Pos * weight[2];
	}

	void BlinPhong::ClipAddPoint(float weight, int last, int cur)
	{
		BlinPhongV2F v2f;
		v2f.Normal = weight * vertexOut[last].Normal + (1.0f - weight) * vertexOut[cur].Normal;
		v2f.UV = weight * vertexOut[last].UV + (1.0f - weight) * vertexOut[cur].UV;
		v2f.WS_Pos = weight * vertexOut[last].WS_Pos + (1.0f - weight) * vertexOut[cur].WS_Pos;
		clipOut.push_back(v2f);

		Eigen::Vector4f st_pos;
		st_pos = weight * ST_Pos[last] + (1.0f - weight) * ST_Pos[cur];
		clipST_Pos.push_back(st_pos);
		
	}

	void BlinPhong::ClipAddPoint(int index)
	{
		//std::cout << "Add Point\n";
		clipOut.push_back(vertexOut[index]);
		clipST_Pos.push_back(ST_Pos[index]);
	}

	void BlinPhong::ClipBegin()
	{
		clipOut = std::vector<BlinPhongV2F>();
		clipST_Pos = std::vector<Eigen::Vector4f>();
	}
	void BlinPhong::ClipOver()
	{
		vertexOut.swap(clipOut);
		ST_Pos.swap(clipST_Pos);
	}
}