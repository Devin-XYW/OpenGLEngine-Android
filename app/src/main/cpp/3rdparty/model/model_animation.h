#ifndef MODEL_ANIM_H
#define MODEL_ANIM_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../assimp/Importer.hpp"
#include "../assimp/scene.h"
#include "../assimp/postprocess.h"

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "assimp_glm_helpers.h"
#include "animdata.h"
#include <opencv2/opencv.hpp>

using namespace std;

/**
 * support a model which contains skeleton animation
 */
class ModelAnim
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;
	bool hasTexture;
    glm::vec3 maxXyz, minXyz;




    // constructor, expects a filepath to a 3D model.
    ModelAnim(string const &path, bool gamma = false) : gammaCorrection(gamma), hasTexture(false)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader)
    {
    	DEBUG_LOGCATE();
		//挨个Mesh渲染
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    float GetMaxViewDistance()
    {
        glm::vec3 vec3 = (abs(minXyz) + abs(maxXyz)) / 2.0f;
        //glm::vec3 vec3 = maxXyz;
        float maxDis = fmax(vec3.x, fmax(vec3.y, vec3.z));
        LOGCATE("Model::GetMaxViewDistance maxDis=%f", maxDis);
        return maxDis;
    }

	std::map<string, BoneInfo> GetBoneInfoMap() {
		LOGCATE("GetBoneInfoMap, address %p, size=%d",&m_BoneInfoMap, m_BoneInfoMap.size());
		return m_BoneInfoMap;
    }

	int& GetBoneCount() { return m_BoneCounter; }

    bool ContainsTextures()
    {
        return hasTexture;
    }


    glm::vec3 GetAdjustModelPosVec()
    {
        glm::vec3 vec3 = (minXyz + maxXyz) / 2.0f;
        LOGCATE("Model::GetAdjustModelPosVec vec3(%f, %f, %f)", vec3.x, vec3.y, vec3.z);
        return (minXyz + maxXyz) / 2.0f;
    }

    void Destroy()
    {
        for (Mesh &mesh : meshes) {
            mesh.Destroy();
        }
    }

private:

	//存储所有骨骼，每个骨骼都有对应的name
	std::map<string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path)
    {
        LOGCATE("loadModel start");
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    /**
     * 递归读取如下信息：
     * 1.骨骼OffsetMatrix
     * 2.每个顶点被影响的骨骼ID及Weight
     */
	void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			//处理mesh中的数据，处理后存入mash数组
			if(mesh != nullptr)
				meshes.push_back(processMesh(mesh, scene));
        }
		LOGCATE("processNode, finish processMesh");
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
		LOGCATE("processNode DONE");
    }

	void SetVertexBoneDataToDefault(Vertex& vertex)
	{
		for (int i = 0; i < 4; i++)
		{
			vertex.m_BoneIDs[i] = -1;
			vertex.m_Weights[i] = 0.0f;
			vertex.m_BoneIDs_plus[i] = -1;
			vertex.m_Weights_plus[i] = 0.0f;
		}
		vertex.v_BoneIds.clear();
		vertex.v_Weights.clear();
	}


	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

            SetVertexBoneDataToDefault(vertex);

			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			updateMaxMinXyz(vector);
			// normals
            if (mesh->HasNormals()){
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

        //加载模型材质
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 遍历各种类型的纹理，根据不同的类型，规范纹理名称，后面的shader会使用这种纹理名称
        // 例如所有漫反射纹理，在shader中的名字为'texture_diffuseN', N 从1 到 MAX_SAMPLER_NUMBER.
        // diffuse: texture_diffuseN 漫反射纹理
        // specular: texture_specularN 镜面反射纹理
        // normal: texture_normalN 法线纹理

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		//解析mesh为基础的骨骼信息
		LoadBoneWeightForVertices(vertices, mesh, scene);

		return Mesh(vertices, indices, textures);
	}

	//填充数据
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		LOGCATE("SetVertexBoneData, boneID %d, weight %f", boneID, weight);
		LOGCATE("SetVertexBoneData, vertex position %p, boneID %d, weight %f", &vertex, boneID, weight);
		vertex.v_BoneIds.push_back(boneID);
		vertex.v_Weights.push_back(weight);
		int bone_size = vertex.v_BoneIds.size();
		if(bone_size >0 && bone_size <= 4){
			vertex.m_BoneIDs[bone_size-1] = boneID;
			vertex.m_Weights[bone_size-1] = weight;
		}else{
			vertex.m_BoneIDs_plus[bone_size-5] = boneID;
			vertex.m_Weights_plus[bone_size-5] = weight;
		}
		if(bone_size > 6){
			LOGCATE("xuwen vertex.v_BoneIds.size = %d",vertex.v_BoneIds.size());
		}
		if(bone_size > 7){
			LOGCATE("xuwen vertex.v_BoneIds.size = %d",vertex.v_BoneIds.size());
		}
	}

	//提取一个mesh下的骨骼数据
	//其中参数vertices代表当前mesh的所有顶点数据结构Vertex数组
	void LoadBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
    	LOGCATE("LoadBoneWeightForVertices, mesh->mNumBones %d", mesh->mNumBones);
		auto& boneInfoMap = m_BoneInfoMap;
		int& boneCount = m_BoneCounter;//start from 0

		//一个Mesh可以有多个骨骼
		for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
		{
			//1. 为这根骨骼分配一个id，方便后续计算
			int boneID = -1;
            aiBone* aiBonePtr = mesh->mBones[boneIndex];//接下来针对这根骨骼提取数据
			std::string boneName = aiBonePtr->mName.C_Str();
			//如果当前获取的bone未加载过
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				//分配id
				newBoneInfo.id = boneCount;
				//提取offset矩阵，将assimp的矩阵数据转换为glm格式的矩阵
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(aiBonePtr->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;//assign an id
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			LOGCATE("boneName %s, boneID %d, boneCount %d", boneName.c_str(), boneID, boneCount);

			//读取顶点骨骼权重
			//骨骼的权重数组，用指针表示，数组长度为numWeights
			auto weightsArray = aiBonePtr->mWeights;
			uint numWeights = aiBonePtr->mNumWeights;

			LOGCATE("numWeights %d", numWeights);

			//2. 遍历所有的权重数组，提取出weight，来放到顶点数据结构中
			//一根骨骼，可以影响多个顶点，通过权重参数来影响，不同的顶点的权重不同
			//一个顶点，也可以被多个骨骼影响，特别是关节处(2个骨骼交界处)，但最多4个
			for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
			{
				//当前影响到了哪个顶点
				int vertexId = weightsArray[weightIndex].mVertexId;
				//当前影响的权重
				float weight = weightsArray[weightIndex].mWeight;
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}

	unsigned int GenerateTex(unsigned char* data, int width, int height, int nrComponents, bool gama)
	{
		unsigned int textureID;
        glGenTextures(1, &textureID);
		// 设置放大缩小模式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);

			//伽马矫正需要设置内部格式（第三个参数）为GL_SRGB或者GL_SRGB_ALPHA，这里未设置，可参考伽马矫正那一章节
			//https://learnopengl-cn.github.io/05%20Advanced%20Lighting/02%20Gamma%20Correction/
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
	{
		string filename = string(path);
		filename = directory + '/' + filename;

		int width, height, nrComponents;//通道数

		//obj格式模型用stbi_load加载路径下的图片。
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		return GenerateTex(data, width, height, nrComponents, false);
	}

	unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture* aiTex, bool gama)
	{
		int width, height, channels;
		//unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		unsigned char* data = nullptr;

		//FBX模型用stbi_load_form_memory加载
		if (aiTex->mHeight == 0)
		{
			data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, &width, &height, &channels, 0);
		}
		else
		{
			data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &width, &height, &channels, 0);
		}

		return GenerateTex(data, width, height, channels, false);
	}


	//load FBX
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString imagePath;
			mat->GetTexture(type, i, &imagePath);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), imagePath.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{
				// if texture hasn't been loaded already, load it
				Texture texture;

				//auto filePath = directory + str.C_Str() ;

				//利用此方法判断是否是FBX模型内嵌贴图
				auto tex = scene->GetEmbeddedTexture(imagePath.C_Str());

				if (tex != nullptr)
				{
					//有内嵌贴图
					texture.id = TextureFrom_FBX_EmbeddedTexture(tex,false);
				}
				else
				{
					//无内嵌贴图，就按外部图片路径加载
					texture.id = TextureFromFile(imagePath.C_Str(), this->directory,false);
				}

				texture.type = typeName;
				texture.path = imagePath.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}

    void updateMaxMinXyz(glm::vec3 pos)
    {
        maxXyz.x = pos.x > maxXyz.x ? pos.x : maxXyz.x;
        maxXyz.y = pos.y > maxXyz.y ? pos.y : maxXyz.y;
        maxXyz.z = pos.z > maxXyz.z ? pos.z : maxXyz.z;

        minXyz.x = pos.x < minXyz.x ? pos.x : minXyz.x;
        minXyz.y = pos.y < minXyz.y ? pos.y : minXyz.y;
        minXyz.z = pos.z < minXyz.z ? pos.z : minXyz.z;
    }

};



#endif
