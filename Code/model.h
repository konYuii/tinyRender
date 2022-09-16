#pragma once

#include "global.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace render {
	struct Vertex {
		Eigen::Vector3f position;
		Eigen::Vector3f normal;
		Eigen::Vector2f texcoords;

	};


	struct Texture {
		cv::Mat data;
		std::string type;
		std::string path;
	};



	class Mesh {
	public:
		std::vector<Vertex> vertices;
		std::vector<Indice> indices;
		std::vector<Texture *> textures;
		Eigen::Matrix4f modelMatrix;

		Mesh(std::vector<Vertex> vertices, std::vector<Indice> indices, std::vector<Texture*> textures);

	};

	class Model {
	public:
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> textures_loaded;

		Model(std::string path)
		{
			loadModel(path);
		}
	private:

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	};
}