//
// Created by xuwen on 2022-07-20.
//

#ifndef OPENGLENGINE_CCYUVDATADEFINE_H
#define OPENGLENGINE_CCYUVDATADEFINE_H

#include <stdio.h>

#pragma pack(push,1)

typedef struct YUVChannelDef{
    unsigned int length;
    unsigned char* dataBuffer;
}YUVChannel;

typedef struct YUVFrameDef{
    unsigned int width;
    unsigned int height;
    YUVChannelDef luma;
    YUVChannelDef chromaB;
    YUVChannelDef chromaR;
    long long pts;
}YUVData_Frame;

typedef struct YUVBuffDef{
    unsigned int width;
    unsigned int height;
    unsigned int ylength;
    unsigned int ulength;
    unsigned int vlength;
    unsigned char* buffer;
}CCYUV_Buffer;

typedef struct DecodedAudiodataDef{
    unsigned char* dataBuff;
    unsigned int dataLength;
}JCDecodedAudioData;

#pragma pack(pop)

#endif //OPENGLENGINE_CCYUVDATADEFINE_H
