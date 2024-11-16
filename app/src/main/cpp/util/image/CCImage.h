//
// Created by chenchao on 2021/8/12.
//

#ifndef CCOPENGLES_CCIMAGE_H
#define CCOPENGLES_CCIMAGE_H
#include "stb_image.h"

#include <stdlib.h>
#include <string.h>

#ifndef U8_t
#define U8_t unsigned char
#endif


class CCImage {

public:
    CCImage();
    ~CCImage();

    int         GetWidth() const;
    int         GetHeight() const;
    int         GetType() const ;
    U8_t*       GetData() const;

    void        ReadFromFile(U8_t* fileName);
    void        ReadFromBuffer(U8_t* dataBuff,off_t length);

private:
    int         m_width;
    int         m_height;
    int         m_type;
    U8_t*       m_pImgData;
};


#endif //CCOPENGLES_CCIMAGE_H
