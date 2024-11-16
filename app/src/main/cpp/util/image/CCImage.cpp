//
// Created by chenchao on 2021/8/12.
//

#include "CCImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdlib.h>
#include <string.h>


CCImage::CCImage():m_width(0),m_height(0),m_type(0),m_pImgData(NULL)
{

}

CCImage::~CCImage()
{
    if(m_pImgData)
    {
        free(m_pImgData);
    }
}


void CCImage::ReadFromFile(U8_t* fileName)
{
    int			type = 0;
    int			width = 0;
    int			height = 0;

    stbi_set_flip_vertically_on_load(true);

    U8_t* picData = stbi_load((char const *)fileName, &width, &height, &type, STBI_rgb_alpha);

    int imgSize = width * height * 4;

    if(imgSize >0 && picData != nullptr){

        m_pImgData =(U8_t*) malloc(imgSize);
        memcpy(m_pImgData,picData,imgSize);

        m_width =width;
        m_height =height;
        m_type = type;
    }

    stbi_image_free(picData);

}

void CCImage::ReadFromBuffer(U8_t* dataBuff,off_t length)
{
    int			type = 0;
    int			width = 0;
    int			height = 0;

    //翻转图像,stbimage读取的图片是上下颠倒的，所以这里需要进行翻转
    stbi_set_flip_vertically_on_load(true);

    U8_t* picData = stbi_load_from_memory((U8_t const *)dataBuff, length, &width, &height, &type, 0);

    int imgSize = width * height * 4;

    if(imgSize >0 && picData != nullptr){

        m_pImgData =(U8_t*) malloc(imgSize);
        memcpy(m_pImgData,picData,imgSize);

        m_width =width;
        m_height =height;
        m_type = type;
    }

    stbi_image_free(picData);

}


int CCImage::GetWidth() const
{
    return m_width;
}

int CCImage::GetHeight() const
{
    return m_height;
}

int CCImage::GetType() const
{
    return m_type;
}

U8_t* CCImage::GetData() const
{
    return m_pImgData;
}