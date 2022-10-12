#pragma once

#include "global.h"
#include "model.h"

namespace render {
	enum class VertexShaderType
	{
		MESH,SKYBOX,SHADOW
	};

	class vertexShader {
	public:
		bool clipOn;
		


		vertexShader();
		Eigen::Matrix4f SetProjectionMat(float znear,float zfar,float aspect_ratio,float fov);
		Eigen::Matrix4f SetOrthoMat(float znear, float zfar, float width, float height);
		Eigen::Matrix4f SetViewMat(Eigen::Vector3f cameraPos, Eigen::Vector3f cameraUp, Eigen::Vector3f lookAt);
		void SetViewMat(Eigen::Matrix4f view);
		void SetModelMat(Eigen::Matrix4f model);

		void doVertex(Indice indice, Mesh* mesh, std::vector<ShadingData>& vout, VertexShaderType vs);

	private:
		bool orthoProject;
		float znear;
		float zfar;
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f project;

		Eigen::Matrix4f mvp;
		Eigen::Matrix4f	mv;
		Eigen::Matrix4f normalMat;
		
		
		void MeshTransform(Vertex vertex, ShadingData& v);
		void SkyBoxTransform(Vertex vertex, ShadingData& v);


		void Clipping(std::vector<ShadingData>& out);
		ShadingData Intersect(ShadingData& v1, ShadingData& v2, float line);

		inline Eigen::Vector4f LinearInterpolate(Eigen::Vector4f v1, Eigen::Vector4f v2, float weight);
		inline Eigen::Vector3f LinearInterpolate(Eigen::Vector3f v1, Eigen::Vector3f v2, float weight);
		inline Eigen::Vector2f LinearInterpolate(Eigen::Vector2f v1, Eigen::Vector2f v2, float weight);
		
	};
}