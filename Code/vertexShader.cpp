#include"vertexShader.h"

namespace render {
	vertexShader::vertexShader()
	{
		this->projection = Eigen::Matrix4f::Identity();
		this->view = Eigen::Matrix4f::Identity();
		this->clipOn = true;
	}
	void vertexShader::SetProjectionMat(float znear, float zfar, float aspect_ratio, float fov)
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

		this->projection = scale * move * P2O;
	}
	void vertexShader::SetViewMat(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt)
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
	}

	void vertexShader::SetViewMat(Eigen::Matrix4f view)
	{
		this->view = view;
	}

	void vertexShader::doVertex(Indice indice, Mesh* mesh, std::vector<VertexOut>& out, VertexShaderType vs)
	{
		//out.resize(indice.v.size());
		this->model = mesh->modelMatrix;
		for (auto& ver : indice.v)
		{
			VertexOut vo;
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

		if (out.size() == 0)
			return;
		int cnt = 0;
		for (auto& v : out)
		{
			v.hpos = v.hpos / v.hpos.w();
		}
	}

	void vertexShader::MeshTransform(Vertex vertex, VertexOut& v)
	{
		
		v.texcoord = vertex.texcoords;

		Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);
		Eigen::Vector4f hn = Eigen::Vector4f(vertex.normal.x(), vertex.normal.y(), vertex.normal.z(), 1.0f);
		
		Eigen::Vector4f vec;
		vec = ModelTransform(hp);
		v.worldPos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		vec = ViewTransform(ModelTransform(hp));
		v.csPos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		v.hpos = ProjectTransform(ViewTransform(ModelTransform(hp)));

		vec = NormalTransform(hn);
		v.normal = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		//std::cout << glm::to_string(v.hpos) << '\n';
		//std::cout << v.hpos;
		//std::cout <<"*****\n";
	}

	void vertexShader::SkyBoxTransform(Vertex vertex, VertexOut& v)
	{
		Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);

		Eigen::Vector4f vec;
		vec = ViewInverTransform(hp);
		v.worldPos = Eigen::Vector3f(vec.x(), vec.y(), vec.z());

		v.csPos = vertex.position;
		
		v.hpos = ProjectTransform(hp);
	}

	inline Eigen::Vector4f vertexShader::ModelTransform(Eigen::Vector4f vec)
	{
		return model * vec;
	}

	inline Eigen::Vector4f vertexShader::ViewTransform(Eigen::Vector4f vec)
	{
		return view * vec;
	}

	inline Eigen::Vector4f vertexShader::ProjectTransform(Eigen::Vector4f vec)
	{
		return projection * vec;
	}

	inline Eigen::Vector4f vertexShader::NormalTransform(Eigen::Vector4f vec)
	{
		
		return (model.inverse().transpose()) * vec;

	}

	inline Eigen::Vector4f vertexShader::ModelInverTransform(Eigen::Vector4f vec)
	{
		return model.inverse() * vec;
	}

	inline Eigen::Vector4f vertexShader::ViewInverTransform(Eigen::Vector4f vec)
	{
		return view.inverse() * vec;
	}

	inline Eigen::Vector4f vertexShader::ProjectInverTransform(Eigen::Vector4f vec)
	{
		return projection.inverse() * vec;
	}

	void vertexShader::Clipping(std::vector<VertexOut>& out)
	{ 
		float line;
		//zÆ½ÃæÌÞ³ý
		std::vector<VertexOut> clip(out);
		out.clear();		
		line = znear - 0.2f;
		for (int i = 0; i < clip.size(); i++)
		{
			VertexOut& current = clip[i];
			VertexOut& last = clip[(i + clip.size() - 1) % clip.size()];

			if (current.hpos.w() == last.hpos.w())
			{
				if(current.hpos.w() < line)
					out.push_back(current);
				continue;
			}

			if (current.hpos.w() <= line)
			{
				if (last.hpos.w() > line)
				{
					VertexOut Intersection = Intersect(last, current, line);
					out.push_back(Intersection);
				}
				out.push_back(current);
			}
			else if (last.hpos.w() <= line)
			{
				VertexOut Intersection = Intersect(last, current, line);
				out.push_back(Intersection);
			}
		}

		std::vector<VertexOut> clip2(out);
		out.clear();
		for (int i = 0; i < clip2.size(); i++)
		{
			VertexOut& current = clip2[i];
			VertexOut& last = clip2[(i + clip2.size() - 1) % clip2.size()];

			if (current.hpos.w() == last.hpos.w())
			{
				if (current.hpos.w() > zfar)
					out.push_back(current);
				continue;
			}

			if (current.hpos.w() >= zfar)
			{
				if (last.hpos.w() <= zfar)
				{
					VertexOut Intersection = Intersect(last, current, zfar);
					out.push_back(Intersection);
				}
				out.push_back(current);
			}
			else if (last.hpos.w() >= zfar)
			{
				VertexOut Intersection = Intersect(last, current, zfar);
				out.push_back(Intersection);
			}
		}



	}

	VertexOut vertexShader::Intersect(VertexOut& v1, VertexOut& v2, float line)
	{
		Vertex vertex;
		float weight = (line - v1.hpos.w()) / (v2.hpos.w() - v1.hpos.w());

		vertex.position = LinearInterpolate(v1.worldPos, v2.worldPos, weight);
		vertex.texcoords = LinearInterpolate(v1.texcoord, v2.texcoord, weight);
		vertex.normal = LinearInterpolate(v1.normal, v2.normal, weight);

		VertexOut handle;
		handle.textureIndex = v2.textureIndex;
		handle.csPos = LinearInterpolate(v1.csPos, v2.csPos, weight);

		Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);
		handle.hpos = ProjectTransform(ViewTransform(hp));


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