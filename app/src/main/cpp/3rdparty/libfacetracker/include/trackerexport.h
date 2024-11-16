#ifndef TRACKEREXPORT_H
#define TRACKEREXPORT_H


#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include "ofvec2f.h"


#ifdef __cplusplus
extern "C" {
#endif


bool FACETRACKER_API_init_facetracker_resources(const char* m_path, const char* c_path);

void FACETRACKER_API_release_all_resources();

void FACETRACKER_API_facetracker_obj_reset();

bool FACETRACKER_API_facetracker_obj_track(cv::Mat &captureImage );

ofVec2f FACETRACKER_API_getPosition(cv::Mat &currentShape);

float FACETRACKER_API_getDistance(ofVec2f p1, ofVec2f p2);

float FACETRACKER_API_getScale(cv::Mat &currentShape);

void  FACETRACKER_API_setTrackingHidden();

#ifdef __cplusplus
}
#endif



#endif // TRACKEREXPORT_H
