#include "model.h"

namespace render {


	void Model::loadModel(std::string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<Indice> indices;
		std::vector<int> textures;

		vertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			Eigen::Vector3f pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.position = pos;

			if (mesh->HasNormals())
			{
				Eigen::Vector3f normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertex.normal = normal;
			}

			//VectorPrint(pos);
			Eigen::Vector2f texcoord;
			

			if (mesh->mTextureCoords[0])
				texcoord = Eigen::Vector2f(static_cast<float>(mesh->mTextureCoords[0][i].x), static_cast<float>(mesh->mTextureCoords[0][i].y));
			else
				texcoord = Eigen::Vector2f(0.0f, 0.0f);

			vertex.texcoords = texcoord;
			vertices.push_back(vertex);

		}

		indices.reserve(mesh->mNumFaces);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			Indice tr;
			tr.v.reserve(face.mNumIndices);
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				tr.v.push_back(face.mIndices[j]);
			}

			indices.push_back(tr);
		}


		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<int> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<int> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<int> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<int> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data


		return Mesh(vertices, indices, textures, &textures_model);
	}

	std::vector<int> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType myType)
	{
		std::vector<int> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (auto texture :textures_model.data)
			{
				if (std::strcmp( texture.path.data(), str.C_Str()) == 0)
				{
					textures.push_back(i);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				bool sucess = texture.LoadTexture(str.C_Str(), this->directory);
				if (sucess)
				{
					texture.type = myType;
					texture.path = str.C_Str();
					textures_model.data.push_back(texture);
					textures.push_back(textures_model.data.size() - 1);

				}
			}
		}
		return textures;
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Indice> indices, std::vector<int> textures, Texture_loaded* texturesLoad)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures_mesh = textures;
		this->textures_load = texturesLoad;
		//if (!textures.empty())
		//	std::cout << textures.size() << '\n';

		this->modelMatrix = Eigen::Matrix4f::Identity();
	}
	void Mesh::SetModelMatrix(Eigen::Vector3f move, Eigen::Vector3f scale)
	{
		this->modelMatrix << scale.x(), 0.0f, 0.0f, move.x(),
			0.0f, scale.y(), 0.0f, move.y(),
			0.0f, 0.0f, scale.z(), move.z(),
			0.0f, 0.0f, 0.0f, 1.0f;
	}
}