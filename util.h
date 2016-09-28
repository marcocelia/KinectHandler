#pragma once

#include "KinectHandler.h"
#include "netHandler.h"

#define COL_VIDEO_MODE	0
#define DEP_VIDEO_MODE	0

#define WIN_XSIZE		320
#define WIN_YSIZE		240

#define GREEN_ID		2
#define ORANGE_ID		1
#define ARUCO_ID		3

using namespace std;
using namespace cv;
using namespace openni;
using namespace aruco;


const char* PixelFormatToStr(PixelFormat);

void initColorSettings(VideoStream&);
void initDepthSettings(VideoStream&);

Mat	getColorImage(VideoStream&);
//Mat	getDepthImage(VideoStream&);
Mat convertColorSpace(const Mat&, unsigned int);
void getPointerPos(const Mat&, const VideoStream&, const DepthPixel*, float&, float&, float&, int&, int&);
void dropDepthVal(Mat&, const DepthPixel*, unsigned int);
vector<Scalar> calibrateColor();
vector<Scalar> readCalibValues(fstream&);
int saveCalibValues(vector<Scalar>, const string);
