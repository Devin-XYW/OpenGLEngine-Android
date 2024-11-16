#ifndef CCMODELLOADER_H
#define CCMODELLOADER_H

#include "CCMesh.h"


class CCModelLoader
{

public:
    explicit CCModelLoader();
    virtual ~CCModelLoader();

    void    InitRenderResources(AAssetManager *pManager,std::string path);
    CCMesh* GetMesh();
    void    SetupAssetManager(AAssetManager *pManager);
    void    Render(CCGLCamera* camera);
    void    UpdateTracking(CCGLCamera* pCamera,glm::vec2 pos,float scale);
private:
    void    calculateTBN(std::vector<CCVertexData> &vertData);
    std::vector<std::string> ccStringSplit(const std::string& src,  const std::string& splitStr);

private:
    CCMesh*               m_pMesh;
    AAssetManager*        m_pAssetManager;

};

#endif // CCMODELLOADER_H
