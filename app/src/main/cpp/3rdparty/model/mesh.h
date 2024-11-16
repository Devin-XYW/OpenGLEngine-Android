#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "glm/vec3.hpp"

using namespace std;
#define MAX_BONE_INFLUENCE 8

//一个3D模型一般由很多小模型组成，小模型也称为网格，即Mesh
//网格是独立的渲染单元，Mesh对象本身包含渲染的所有相关数据，比如顶点位置、法向量、纹理坐标、物体的材质
//渲染一个网格，就是一次DrawCall。渲染一帧完整的图像，就会有多次的DrawCall
//Assimp也是按上面的规则来解析数据。最后生成的是多个Mesh对象，方便开发者使用，甚至修改某个网格
//obj文件声明了网格的所有几何信息，如顶点&纹理坐标，法向量，以及使用哪些材质等
//mtl文件声明了材质的详细信息，如光照参数，纹理图片的路径等
//这和Assimp的数据结构有点匹配
struct Vertex {
    // position顶点坐标
    glm::vec3 Position;
    // normal 法线向量
    glm::vec3 Normal;
    // texCoords 纹理坐标
    glm::vec2 TexCoords;
    // tangent 切线坐标
    glm::vec3 Tangent;
    // bitangent 双切线
    glm::vec3 Bitangent;

    //bone indexes which will influence this vertex
    //存储顶点所关联的骨骼id，代表该点被哪些骨骼所影响，最多受到8个骨骼影响
    int m_BoneIDs[4] = {-1,-1,-1,-1};
    int m_BoneIDs_plus[4] = {-1,-1,-1,-1};
    vector<int> v_BoneIds;
    //weights from each bone
    //存储骨骼对应的权重数据，最多受到4个骨骼影响
    float m_Weights[4] = {0.0f,0.0f,0.0f,0.0f};
    float m_Weights_plus[4] = {0.0f,0.0f,0.0f,0.0f};
    vector<float> v_Weights;

    Vertex(){
        Position = glm::vec3(0.0f);
        Normal = glm::vec3(0.0f);
        TexCoords = glm::vec2(0.0f);
    }
};

//定义当前贴图相关信息
struct Texture {
    uint id;//纹理Id，OpenGL环境下生成
    string type;//纹理类型，也对应shader中的变量名，如diffuse纹理或者specular纹理
    string path;//纹理的路径，用于判断是否被加载过，可以防止做重复加载
};

//定义一个网格类，包括几何信息和纹理信息
//同时实现一个方法可以绘制网格Mesh，（Mesh是最小绘制单位）
class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;//一组顶点
    vector<unsigned int> indices;//顶点对应的索引
    vector<Texture> textures;//纹理：贴图、法线贴图、光照贴图等
    unsigned int VAO;//VAO 顶点数组对象

    /*  Functions  */
    //constructor 构造函数，主要是初始化Mesh
    //初始化当前顶点数组、索引数组、纹理数组
    //生成当前VAO、VBO、EBO
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        //加载VAO、VBO、EBO
        setupMesh();
    }

    // render the mesh 渲染函数，绘制当前Mesh
    void Draw(Shader &shader)
    {
        DEBUG_LOGCATE();
        // bind appropriate textures
        //绑定贴图：颜色贴图、法线贴图、高光贴图
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            //拼装shader传参字符串
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if(name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
			    number = std::to_string(heightNr++); // transfer unsigned int to stream

													 // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            //shader.setFloat((name + number).c_str(),i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
        for(unsigned int i = 0; i < textures.size(); i++) {
            glBindTexture(GL_TEXTURE_2D + i, 0);//解绑
        }
    }

    void Destroy()
    {
        for (int i = 0; i < textures.size(); ++i) {
            glDeleteTextures(1, &textures[i].id);
        }
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        VAO = EBO = VBO = GL_NONE;
    }

private:
    // render data
    unsigned int VBO, EBO;
    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh()
    {

        // create buffers/arrays生成VAO、VBO、EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        //绑定VAO
        glBindVertexArray(VAO);
        // load data into vertex buffers
        //绑定VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        //向VBO中填充顶点数据
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

        //绑定EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //向EBO中填充索引数据
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

        // set the vertex attribute pointers
        //绑定shader中的数据并进行启用锚点
        // vertex Positions 顶点数据
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals 法线数据
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords 纹理坐标数据
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        //骨骼动画
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        //骨骼动画
        // ids
        glEnableVertexAttribArray(7);
        glVertexAttribIPointer(7, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs_plus));

        // weights
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights_plus));


        glBindVertexArray(0);
    }
};
#endif
