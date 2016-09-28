#pragma once

#include "stdafx.h"
#include <iostream>
#include <fstream>

/*********************	Inc.-OpenNI2 ************************/
#include <OpenNI.h>
#include <PS1080.h>
#pragma comment(lib,"OpenNI2.lib")
/************************************************************/

/*********************	Inc.-OpenCv	************************/
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv25/opencv2/highgui/highgui.hpp"
#include "opencv25/opencv2/imgproc/imgproc.hpp"
#include "opencv25/opencv2/photo/photo.hpp"
#include "opencv25/opencv2/features2d/features2d.hpp"
#ifdef _DEBUG 
	#pragma comment(lib, "opencv_photo245d.lib") 
	#pragma comment(lib, "opencv_imgproc245d.lib") 
	#pragma comment(lib, "opencv_core245d.lib") 
	#pragma comment(lib, "opencv_highgui245d.lib") 
	#pragma comment(lib, "opencv_calib3d245d.lib") 
	#pragma comment(lib, "opencv_features2d245d.lib") 
	#pragma comment(lib, "opencv_flann245d.lib") 
#else
	#pragma comment(lib, "opencv_photo245.lib") 
	#pragma comment(lib, "opencv_imgproc245.lib") 
	#pragma comment(lib, "opencv_core245.lib") 
	#pragma comment(lib, "opencv_highgui245.lib") 
	#pragma comment(lib, "opencv_calib3d245.lib") 
	#pragma comment(lib, "opencv_features2d245.lib") 
	#pragma comment(lib, "opencv_flann245.lib") 
#endif
#pragma comment(lib, "zlib.lib") 
#pragma comment(lib, "libjpeg.lib") 
#pragma comment(lib, "libpng.lib") 
#pragma comment(lib, "libtiff.lib") 
#pragma comment(lib, "libjasper.lib") 
/***********************************************************/


// opencv
/*#pragma comment(lib, "cxcore210d.lib")
#pragma comment(lib, "cv210d.lib")
#pragma comment(lib, "highgui210d.lib")*/

/*********************	Inc.-ARUCO ************************/
#include "aruco/aruco.h"
#ifdef _DEBUG
	#pragma comment(lib, "aruco/opencv_imgproc242d.lib")
	#pragma comment(lib, "aruco/opencv_core242d.lib")
	#pragma comment(lib, "aruco/opencv_calib3d242d.lib")
	#pragma comment(lib, "aruco/aruco124d.lib")
#else
	#pragma comment(lib, "aruco/opencv_imgproc242.lib")
	#pragma comment(lib, "aruco/opencv_core242.lib")
	#pragma comment(lib, "aruco/opencv_calib3d242.lib")
	#pragma comment(lib, "aruco/aruco124.lib")
#endif 
#pragma comment(lib, "aruco/zlib.lib")
/***********************************************************/

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"winmm.lib")


