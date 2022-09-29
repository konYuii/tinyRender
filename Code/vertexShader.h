#pragma once

#include "global.h"
#include "model.h"

namespace render {
	enum class VertexShaderType
	{
		MESH,SKYBOX
	};

	class vertexShader {
	public:
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;
		

		bool clipOn;

		vertexShader();
		void SetProjectionMat(float znear,float zfar,float aspect_ratio,float fov);
		void SetViewMat(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt);
		void SetViewMat(Eigen::Matrix4f view);
		void doVertex(Indice indice, Mesh* mesh, std::vector<VertexOut>& vout, VertexShaderType vs);

	private:
		float znear;
		float zfar;
		
		void MeshTransform(Vertex vertex, VertexOut& v);
		void SkyBoxTransform(Vertex vertex, VertexOut& v);

		inline Eigen::Vector4f ModelTransform(Eigen::Vector4f vec);
		inline Eigen::Vector4f ViewTransform(Eigen::Vector4f vec);
		inline Eigen::Vector4f ProjectTransform(Eigen::Vector4f vec);
		inline Eigen::Vector4f NormalTransform(Eigen::Vector4f vec);

		inline Eigen::Vector4f ModelInverTransform(Eigen::Vector4f vec);
		inline Eigen::Vector4f ViewInverTransform(Eigen::Vector4f vec);
		inline Eigen::Vector4f ProjectInverTransform(Eigen::Vector4f vec);
		void Clipping(std::vector<VertexOut>& out);
		VertexOut Intersect(VertexOut& v1, VertexOut& v2, float line);

		inline Eigen::Vector4f LinearInterpolate(Eigen::Vector4f v1, Eigen::Vector4f v2, float weight);
		inline Eigen::Vector3f LinearInterpolate(Eigen::Vector3f v1, Eigen::Vector3f v2, float weight);
		inline Eigen::Vector2f LinearInterpolate(Eigen::Vector2f v1, Eigen::Vector2f v2, float weight);
		
	};
}