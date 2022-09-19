#include"vertexShader.h"

namespace render {
	vertexShader::vertexShader()
	{
		this->projection = Eigen::Matrix4f::Identity();
		this->view = Eigen::Matrix4f::Identity();
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
		//std::cout << glm::to_string(orth) << '\n';
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

	void vertexShader::doVertex(Indice indice, Mesh* mesh, std::vector<VertexOut>& out)
	{
		//out.resize(indice.v.size());
		this->model = mesh->modelMatrix;
		for (auto& ver : indice.v)
		{
			VertexOut vo;
			TransformVertex(mesh->vertices[ver], vo);
			out.push_back(vo);
		}

		Clipping(out);

	}

	void vertexShader::TransformVertex(Vertex vertex, VertexOut& v)
	{
		
		v.texcoord = vertex.texcoords;

		Eigen::Vector4f hp = Eigen::Vector4f(vertex.position.x(), vertex.position.y(), vertex.position.z(), 1.0f);
		Eigen::Vector4f hn = Eigen::Vector4f(vertex.normal.x(), vertex.normal.y(), vertex.normal.z(), 1.0f);
		
		Eigen::Vector4f vec4;
		vec4 = model * hp;
		v.worldPos = Eigen::Vector3f(vec4.x(), vec4.y(), vec4.z());

		vec4 = this->view * model * hp;
		v.csPos = Eigen::Vector3f(vec4.x(), vec4.y(), vec4.z());

		v.hpos = this->projection * this->view * model * hp;

		vec4 = ((model).inverse()).transpose() * hn;
		v.normal = Eigen::Vector3f(vec4.x(), vec4.y(), vec4.z());

		//std::cout << glm::to_string(v.hpos) << '\n';
		//std::cout << v.hpos;
		//std::cout <<"*****\n";
	}

	void vertexShader::Clipping(std::vector<VertexOut>& out)
	{ 

		std::vector<VertexOut> clip(out);
		out.clear();		
		for (int i = 0; i < clip.size(); i++)
		{
			VertexOut& current = clip[i];
			VertexOut& last = clip[(i + clip.size() - 1) % clip.size()];

			if (current.hpos.w() == last.hpos.w())
			{
				if(current.hpos.w() < znear)
					out.push_back(current);
				continue;
			}

			if (current.hpos.w() < znear)
			{
				if (last.hpos.w() > znear)
				{
					VertexOut Intersection = Intersect(last, current, znear);
					out.push_back(Intersection);
				}
				out.push_back(current);
			}
			else if (last.hpos.w() < znear)
			{
				VertexOut Intersection = Intersect(last, current, znear);
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


		if (out.size() == 0)
			return;
		for (auto& v : out)
		{
			if (std::abs(v.hpos.w()) < 0.1f)
			{
				std::cout << "!!!!\n";
			}
			v.hpos = v.hpos / v.hpos.w();
		}
	}

	VertexOut vertexShader::Intersect(VertexOut& v1, VertexOut& v2, float line)
	{
		Vertex inter;
		float weight = (line - v1.hpos.w()) / (v2.hpos.w() - v1.hpos.w());

		inter.position = LinearInterpolate(v1.worldPos, v2.worldPos, weight);
		inter.texcoords = LinearInterpolate(v1.texcoord, v2.texcoord, weight);
		inter.normal = LinearInterpolate(v1.normal, v2.normal, weight);

		VertexOut handle;
		TransformVertex(inter, handle);

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