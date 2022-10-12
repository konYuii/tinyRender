#include"vertexShader.h"

namespace render {
	vertexShader::vertexShader()
	{
		this->project = Eigen::Matrix4f::Identity();
		this->view = Eigen::Matrix4f::Identity();

		this->clipOn = true;
	}
	Eigen::Matrix4f vertexShader::SetProjectionMat(float znear, float zfar, float aspect_ratio, float fov)
	{
		znear = znear > 0 ? -znear : znear;
		zfar = zfar > 0 ? -zfar : zfar;
		this->znear = znear;
		this->zfar = zfar;

		Eigen::Matrix4f P2O;
		P2O << znear, 0.0f, 0.0f, 0.0f,
			0.0f, znear, 0.0f, 0.0f,
			0.0f, 0.0f, znear + zfar, -znear* zfar,
			0.0f, 0.0f, 1.0f, 0.0f;

		float yTop = tan(fov * PI / 360) * -znear;
		float xRight = yTop * aspect_ratio;

		Eigen::Matrix4f move;
		move << 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, -(znear + zfar) * 0.5f,
			0.0f, 0.0f, 0.0f, 1.0f;

		Eigen::Matrix4f scale;
		scale << 1.0f / xRight, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / yTop, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (-zfar + znear), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

		this->project = scale * move * P2O;
		this->orthoProject = false;
		return this->project;
	}
	Eigen::Matrix4f vertexShader::SetOrthoMat(float znear, float zfar, float width, float height)
	{
		znear = znear > 0 ? -znear : znear;
		zfar = zfar > 0 ? -zfar : zfar;
		this->znear = znear;
		this->zfar = zfar;


		this->project << 1.0f / width, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / height, 0.0f, 0.0f,
			0.0f, 0.0f, 2.0f / (-zfar + znear), -(znear + zfar) / (znear - zfar),
			0.0f, 0.0f, 1.0f, 0.0f;
		this->orthoProject = true;
		return this->project;
	}
	Eigen::Matrix4f vertexShader::SetViewMat(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt)
	{
		Eigen::Matrix4f move;
		move << 1.0f, 0.0f, 0.0f, -cameraPos.x(),
			0.0f, 1.0f, 0.0f, -cameraPos.y(),
			0.0f, 0.0f, 1.0f, -cameraPos.z(),
			0.0f, 0.0f, 0.0f, 1.0f;

		Eigen::Vector3f cameraRight = lookAt.cross(cameraUp);
		Eigen::Matrix4f project;
		project << cameraRight.x(), cameraRight.y(), cameraRight.z(), 0.0f,
			cameraUp.x(), cameraUp.y(), cameraUp.z(), 0.0f,
			-lookAt.x(), -lookAt.y(), -lookAt.z(), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

		this->view = project * move;
		return this->view;
	}

	void vertexShader::SetViewMat(Eigen::Matrix4f view)
	{
		this->view = view;
	}

	void vertexShader::SetModelMat(Eigen::Matrix4f model)
	{
		this->model = model;
		this->mvp = this->project * this->view * this->model;
		this->mv = this->view * this->model;
		this->normalMat = this->model.inverse().transpose();
	}

	void vertexShader::doVertex(Indice indice, Mesh* mesh, std::vector<ShadingData>& out, VertexShaderType vs)
	{
		//out.resize(indice.v.size());

		for (auto& ver : indice.v)
		{
			ShadingData vo;
			if (mesh->textures_mesh.empty())
				vo.textureIndex = -1;
			else
				vo.textureIndex = mesh->textures_mesh[0];

			switch (vs)
			{
			case render::VertexShaderType::MESH:
				MeshTransform(mesh->vertices[ver], vo);
				break;
			case render::VertexShaderType::SKYBOX:
				SkyBoxTransform(mesh->vertices[ver], vo);
				break;
			default:
				break;
			}
			
			out.push_back(vo);
		}

		if(clipOn)
			Clipping(out);
		if (out.size() == 0 || orthoProject)
			return;
		for (auto& v : out)
		{
			v.ST_Pos.x() /= v.ST_Pos.w();
			v.ST_Pos.y() /= v.ST_Pos.w();
			v.ST_Pos.z() /= v.ST_Pos.w();
		}
	}

	void vertexShader::MeshTransform(Vertex vertex, ShadingData& v)
	{
		
		v.texcoord = vertex.texcoords;

		Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);
		Eigen::Vector4f hn = Eigen::Vector4f(vertex.normal.x(), vertex.normal.y(), vertex.normal.z(), 1.0f);
		
		Eigen::Vector4f vec;
		vec = model * hp;
		v.WS_Pos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		vec = mv * hp;
		v.CS_Pos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		v.ST_Pos = mvp * hp;

		vec = normalMat * hn;
		v.normal = Eigen::Vector3f(vec.x(), vec.y(), vec.z());


	}

	void vertexShader::SkyBoxTransform(Vertex vertex, ShadingData& v)
	{
		Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);

		Eigen::Vector4f vec;
		vec = view.inverse() * hp;
		v.WS_Pos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		v.CS_Pos = vertex.position;
		
		v.ST_Pos = project * hp;
	}




	void vertexShader::Clipping(std::vector<ShadingData>& out)
	{ 
		float line;
		//zÆ½ÃæÌÞ³ý
		std::vector<ShadingData> clip(out);
		out.clear();		
		for (int i = 0; i < clip.size(); i++)
		{
			ShadingData& current = clip[i];
			ShadingData& last = clip[(i + clip.size() - 1) % clip.size()];

			if (current.ST_Pos.w() == last.ST_Pos.w())
			{
				if(current.ST_Pos.w() < znear)
					out.push_back(current);
				continue;
			}

			if (current.ST_Pos.w() <= znear)
			{
				if (last.ST_Pos.w() > znear)
				{
					ShadingData Intersection = Intersect(last, current, znear);
					out.push_back(Intersection);
				}
				out.push_back(current);
			}
			else if (last.ST_Pos.w() <= znear)
			{
				ShadingData Intersection = Intersect(last, current, znear);
				out.push_back(Intersection);
			}
		}

		std::vector<ShadingData> clip2(out);
		out.clear();
		for (int i = 0; i < clip2.size(); i++)
		{
			ShadingData& current = clip2[i];
			ShadingData& last = clip2[(i + clip2.size() - 1) % clip2.size()];

			if (current.ST_Pos.w() == last.ST_Pos.w())
			{
				if (current.ST_Pos.w() > zfar)
					out.push_back(current);
				continue;
			}

			if (current.ST_Pos.w() >= zfar)
			{
				if (last.ST_Pos.w() <= zfar)
				{
					ShadingData Intersection = Intersect(last, current, zfar);
					out.push_back(Intersection);
				}
				out.push_back(current);
			}
			else if (last.ST_Pos.w() >= zfar)
			{
				ShadingData Intersection = Intersect(last, current, zfar);
				out.push_back(Intersection);
			}
		}



	}

	ShadingData vertexShader::Intersect(ShadingData& v1, ShadingData& v2, float line)
	{

		float weight = (line - v1.ST_Pos.w()) / (v2.ST_Pos.w() - v1.ST_Pos.w());

		ShadingData handle;
		handle.texcoord = LinearInterpolate(v1.texcoord, v2.texcoord, weight);
		handle.textureIndex = v2.textureIndex;
		handle.normal = LinearInterpolate(v1.normal, v2.normal, weight);
		handle.CS_Pos = LinearInterpolate(v1.CS_Pos, v2.CS_Pos, weight);

		Eigen::Vector4f vec;
		Eigen::Vector4f hp = Eigen::Vector4f(handle.CS_Pos[0], handle.CS_Pos[1], handle.CS_Pos[2], 1.0f);
		handle.ST_Pos = project * hp;
		vec = view.inverse() * hp;
		handle.WS_Pos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());
		
		return handle;
	}

	inline Eigen::Vector4f vertexShader::LinearInterpolate(Eigen::Vector4f v1, Eigen::Vector4f v2, float weight)
	{
		return (1.0f - weight) * v1 + weight * v2;
	}

	inline Eigen::Vector3f vertexShader::LinearInterpolate(Eigen::Vector3f v1, Eigen::Vector3f v2, float weight)
	{
		return (1.0f - weight) * v1 + weight * v2;
	}

	inline Eigen::Vector2f vertexShader::LinearInterpolate(Eigen::Vector2f v1, Eigen::Vector2f v2, float weight)
	{
		return (1.0f - weight) * v1 + weight * v2;
	}






}