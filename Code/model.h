#pragma once

#include "global.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace render {



	class Mesh {
	public:
		std::vector<Vertex> vertices;
		std::vector<Indice> indices;
		Texture_loaded* textures_load;
		std::vector<int> textures_mesh;
		
		Eigen::Matrix4f modelMatrix;
		Mesh(std::vector<Vertex> vertices, std::vector<Indice> indices, std::vector<int> textures, Texture_loaded* texturesLoad);
		void SetModelMatrix(Eigen::Vector3f move, Eigen::Vector3f scale);

	};

	class Model {
	public:
		std::vector<Mesh> meshes;
		std::string directory;
		Texture_loaded textures_model;
		Eigen::Matrix4f modelMatrix;

		Model(std::string path)
		{
			modelMatrix = Eigen::Matrix4f::Identity();
			loadModel(path);
		}
	private:

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<int> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType myType);
	};
}