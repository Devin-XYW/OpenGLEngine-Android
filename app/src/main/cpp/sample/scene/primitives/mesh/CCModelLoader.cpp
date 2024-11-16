#include "CCModelLoader.h"

#include <iostream>
#include <fstream>
#include <string>


CCModelLoader::CCModelLoader()
{

}
CCModelLoader::~CCModelLoader()
{
    CCGLSafePtrDelete(m_pMesh);
}


void CCModelLoader::InitRenderResources(AAssetManager *pManager,std::string dirPath)
{
    std::string fileName = dirPath + "/monkey.obj";
//    std::string fileName = dirPath + "/tortoise.obj";
//    std::string fileName = dirPath + "/dragon.obj";

    //生成一个IO，读取OBj文件里面的数据
    std::ifstream inputStream(fileName, std::ifstream::in | std::ifstream::binary);

    if (!inputStream.is_open()){
        std::cerr << "Error opening file:"<<fileName<<std::endl;
    }


    std::vector<glm::vec3> coords;
    std::vector<glm::vec2> texturCoords;
    std::vector<glm::vec3> normals;

    std::vector<CCVertexData> vertexes;
    std::vector<GLuint> indexes;

    CCMesh *mesh = nullptr;

    std::string mtlName;
    std::string lineString;

    //按行读取文件中的数据，每一行都是一行String
    while (std::getline(inputStream,lineString))
    {
        //将每行的元素以空格分开
        std::vector<std::string> list = ccStringSplit(lineString," ");
        if (list[0]  == "#") {
            std::cout<< "This is comment:" << lineString;
            continue;
        } else if (list[0]  == "mtllib") {
            std::cout<< "File with materials:" << list[1];
            continue;
        } else if (list[0]  == "v") {
            //顶点坐标
            coords.emplace_back(glm::vec3(atof(list[1].c_str()), atof(list[2].c_str()), atof(list[3].c_str())));
            continue;
        } else if (list[0]  == "vt") {
            //纹理坐标
            texturCoords.emplace_back(glm::vec2(atof(list[1].c_str()), atof(list[2].c_str())));
            continue;
        } else if (list[0]  == "vn") {
            //法线向量
            normals.emplace_back(glm::vec3(atof(list[1].c_str()), atof(list[2].c_str()), atof(list[3].c_str())));
            continue;
        } else if (list[0]  == "f") {
            //f表示当前面，顶点索引/uv索引/法线索引
            for (int i = 1; i <= 3; ++i){
                std::vector<std::string> vert = ccStringSplit(list[i],"/");
                vertexes.emplace_back(VertexData(
                        //根据位置索引，获取顶点坐标
                        coords[static_cast<int>(atol(vert[0].c_str())) - 1],
                        //根据纹理索引，获取纹理坐标
                        texturCoords[static_cast<int>(atol(vert[1].c_str())) - 1],
                        //根据法线索引，获取法线向量
                        normals[static_cast<int>(atol(vert[2].c_str())) -1 ])
                        );
                //index表示面的顺序：是个递增的变量，标注当前面的索引
                indexes.emplace_back(static_cast<unsigned>(indexes.size()));
            }
            continue;
        } else if (list[0] == "usemtl") {
            mtlName = list[1];
            std::cout<< "This is used naterial:" << mtlName;

        }
    }

    if (mesh == nullptr) {

        calculateTBN(vertexes);
        mesh = new CCMesh();
        mesh->InitRenderResources(pManager,vertexes, indexes);
        vertexes.clear();
        indexes.clear();

    }

    m_pMesh = mesh;

}

void CCModelLoader::SetupAssetManager(AAssetManager *pManager)
{
    m_pAssetManager = pManager;
}

CCMesh* CCModelLoader::GetMesh()
{
    if(!m_pMesh){
        return NULL;
    }

    return m_pMesh;
}
void CCModelLoader::Render(CCGLCamera* camera)
{
    if(!m_pMesh){
        return;
    }

    m_pMesh->Render(camera);
}

void CCModelLoader::UpdateTracking(CCGLCamera* pCamera,glm::vec2 pos,float scale)
{
    m_pMesh->UpdateTrackingInfo(pCamera,pos,scale);
}

void CCModelLoader::calculateTBN(std::vector<CCVertexData> &vertData)
{
    //计算TBN矩阵：用于计算光照时，得到切线空间的法线向量
    //这里引出切线空间：无论什么朝向，所有片段法向量都不变，
    //               并且通过空间变换就可以得到世界坐标下的正确的法向量
    //               所有的面，都可以用同样的法线贴图，这就是切线空间
    for (int i = 0; i < (int)vertData.size(); i += 3) {

        //三个顶点的顶点坐标
        glm::vec3 &v1 = vertData[i].position;
        glm::vec3 &v2 = vertData[i + 1].position;
        glm::vec3 &v3 = vertData[i + 2].position;

        glm::vec2 &uv1 = vertData[i].textCoord;
        glm::vec2 &uv2 = vertData[i + 1].textCoord;
        glm::vec2 &uv3 = vertData[i + 2].textCoord;

        // https://youtu.be/ef3XR0ZttDU?t=1097
        // deltaPos1 = deltaUV1.x * T + deltaUV1.y * B;
        // deltaPos2 = deltaUV2.x * T + deltaUV2.y * B;

        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        vertData[i].tangent = tangent;
        vertData[i + 1].tangent = tangent;
        vertData[i + 2].tangent = tangent;

        vertData[i].bitangent = bitangent;
        vertData[i + 1].bitangent = bitangent;
        vertData[i + 2].bitangent = bitangent;

    }
}

std::vector<std::string> CCModelLoader::ccStringSplit(const std::string& src,  const std::string& splitStr)
{
    std::string::size_type pos1, pos2;
    size_t len = src.length();
    pos2 = src.find(splitStr);
    pos1 = 0;

    std::vector<std::string> strList;

    while(std::string::npos != pos2)
    {
        strList.emplace_back(src.substr(pos1, pos2-pos1));

        pos1 = pos2 + splitStr.size();
        pos2 = src.find(splitStr, pos1);
    }
    if(pos1 != len){
        strList.emplace_back(src.substr(pos1));
    }

    return strList;
}
