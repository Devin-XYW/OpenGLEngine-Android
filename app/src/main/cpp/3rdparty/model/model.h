#ifndef MODEL_H
#define MODEL_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../assimp/Importer.hpp"
#include "../assimp/scene.h"
#include "../assimp/postprocess.h"
#include <shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>
#include "mesh.h"
#include "LogUtil.h"
#include "opencv2/imgproc/types_c.h"

using namespace std;

//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
//定义一个Model全局管理类，用于加载assimp数据，实现Draw函数，绘制所有网格
//其中有两个关键函数
// loadModel加载assimp数据，生成渲染所需要的数据；
// Draw 绘制一帧画面
class Model 
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;//模型所包含的网格
    string directory;//模型文件所在目录
    glm::vec3 maxXyz, minXyz;
    bool gammaCorrection;
    bool hasTexture;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) :
    gammaCorrection(gamma),
    hasTexture(false)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    //模型渲染，即依次渲染各个网格
    void Draw(Shader shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    float GetMaxViewDistance()
    {
        //glm::vec3 vec3 = (abs(minXyz) + abs(maxXyz)) / 2.0f;
        glm::vec3 vec3 = maxXyz;
        float maxDis = fmax(vec3.x, fmax(vec3.y, vec3.z));
        LOGCATE("Model::GetMaxViewDistance maxDis=%f", maxDis);
        return maxDis;
    }

    glm::vec3 GetAdjustModelPosVec()
    {
        glm::vec3 vec3 = (minXyz + maxXyz) / 2.0f;
        LOGCATE("Model::GetAdjustModelPosVec vec3(%f, %f, %f)", vec3.x, vec3.y, vec3.z);
        return (minXyz + maxXyz) / 2.0f;
    }

    bool ContainsTextures()
    {
        return hasTexture;
    }

    void Destroy()
    {
        for (Mesh &mesh : meshes) {
            mesh.Destroy();
        }
    }

private:
    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    //加载模型
    void loadModel(string const &path)
    {
        DEBUG_LOGCATE();
        // read file via ASSIMP
        //读取路径下path的一个模型文件
        Assimp::Importer importer;//用于读取模型文件
        //使用importer读取模型文件，并进行一些后处理
        //aiProcess_Triangulate:表示将顶点生成三角形后处理
        //aiProcess_FlipUVs：表示将UV读取的图片进行翻转，这里翻转的原因是UV原点和屏幕原点不同
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        DEBUG_LOGCATE();
        // check for errors
        //检测到出错则返回
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            LOGCATE("Model::loadModel path=%s, assimpError=%s", path.c_str(), importer.GetErrorString());
            return;
        }
        DEBUG_LOGCATE();
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        //遍历节点
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    //处理aiScene对象包含的节点和子节点Mesh数据
    //递归函数对节点进行处理
    void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        //遍历当前节点的mesh数组
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            //根据数组存的index，从scene拿到原始的aiMesh
            //node中的mMeshes存的是索引，scene中的mMeshes存的是数据
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            //处理mesh中的数据，处理后存入mash数组
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        //递归遍历处理子节点
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

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

    //处理一个mesh数据
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        //声明填充顶点数组、索引数组、纹理数组
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        // Walk through each of the mesh's vertices
        //遍历mash中所有的vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            //把assimp的坐标格式转换为glm::vec3 方便OpenGL读取
            glm::vec3 _pos; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions 顶点坐标数据
            _pos.x = mesh->mVertices[i].x;
            _pos.y = mesh->mVertices[i].y;
            _pos.z = mesh->mVertices[i].z;
            vertex.Position = _pos;
            updateMaxMinXyz(_pos);
            // normals 法线向量数据
            // normals
            if (mesh->HasNormals())
            {
                glm::vec3 _normal;
                _normal.x = mesh->mNormals[i].x;
                _normal.y = mesh->mNormals[i].y;
                _normal.z = mesh->mNormals[i].z;
                vertex.Normal = _normal;
            }
            // texture coordinates 这里取diffuse纹理坐标数据
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 _texCoord;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                //顶点最大可以指向8个纹理坐标，但一般只有一个
                _texCoord.x = mesh->mTextureCoords[0][i].x;
                _texCoord.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = _texCoord;
            }
//            else
//                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            // tangent 切线信息
            glm::vec3 _tangents;
            _tangents.x = mesh->mTangents[i].x;
            _tangents.y = mesh->mTangents[i].y;
            _tangents.z = mesh->mTangents[i].z;
            vertex.Tangent = _tangents;
            // bitangent 双切线信息
            glm::vec3 bitangents;
            bitangents.x = mesh->mBitangents[i].x;
            bitangents.y = mesh->mBitangents[i].y;
            bitangents.z = mesh->mBitangents[i].z;
            vertex.Bitangent = bitangents;
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        //遍历所有的面，即三角形，解析index
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            //检索一个面的顶点索引，并保存到顶点索引变量
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        //解析材质
        //判断有材质才考虑开始处理网格使用的材质
        if(mesh->mMaterialIndex >= 0){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN
            // 遍历各种类型的纹理，根据不同的类型，规范纹理名称，后面的shader会使用这种纹理名称
            // 例如所有漫反射纹理，在shader中的名字为'texture_diffuseN', N 从1 到 MAX_SAMPLER_NUMBER.
            // diffuse: texture_diffuseN 漫反射纹理
            // specular: texture_specularN 镜面反射纹理
            // normal: texture_normalN 法线纹理

            // 1. diffuse maps
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // return a mesh object created from the extracted mesh data
        //最后生成一个纹理，包含了坐标和纹理数据
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    //创建纹理并加载材质数据
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        DEBUG_LOGCATE();
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            //检查当前纹理texture是否加载过，否则就直接使用加载过的数据，不需要重复加载
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false)
    {
        string filename = string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID); //生成纹理id

//        int width, height, nrComponents;
//        unsigned char *data = nullptr;

//        // load the texture using OpenCV
//        //使用OpenCV加载纹理
//        LOGCATE("TextureFromFile Loading texture %s", filename.c_str());
//        cv::Mat textureImage = cv::imread(filename);
//        if (!textureImage.empty()){
//            hasTexture = true;
//            // opencv reads textures in BGR format, change to RGB for GL
//            //使用opencv去读取纹理图片
//            cv::cvtColor(textureImage, textureImage, CV_BGR2RGB);
//            // opencv reads image from top-left, while GL expects it from bottom-left
//            // vertically flip the image
//            //cv::flip(textureImage, textureImage, 0);
//
//            glBindTexture(GL_TEXTURE_2D, textureID);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureImage.cols,
//                         textureImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE,
//                         textureImage.data);
//            glGenerateMipmap(GL_TEXTURE_2D);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            GO_CHECK_GL_ERROR();
//        } else {
//            LOGCATE("TextureFromFile Texture failed to load at path: %s", path);
//        }
//
//        return textureID;
        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data){
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};

#endif
