#include "util.h"

const char* PixelFormatToStr(PixelFormat pixelformat) {
	switch (pixelformat) {
	case PIXEL_FORMAT_DEPTH_100_UM:
		return "PIXEL_FORMAT_DEPTH_100_UM";
		break;
	case PIXEL_FORMAT_DEPTH_1_MM:
		return "PIXEL_FORMAT_DEPTH_1_MM";
		break;
	case PIXEL_FORMAT_GRAY16:
		return "PIXEL_FORMAT_GRAY16";
		break;
	case PIXEL_FORMAT_GRAY8:
		return "PIXEL_FORMAT_GRAY8";
		break;
	case PIXEL_FORMAT_JPEG:
		return "PIXEL_FORMAT_JPEG";
		break;
	case PIXEL_FORMAT_RGB888:
		return "PIXEL_FORMAT_RGB888";
		break;
	case PIXEL_FORMAT_SHIFT_9_2:
		return "PIXEL_FORMAT_SHIFT_9_2";
		break;
	case PIXEL_FORMAT_SHIFT_9_3:
		return "PIXEL_FORMAT_SHIFT_9_3";
		break;
	case PIXEL_FORMAT_YUV422:
		return "PIXEL_FORMAT_YUV422";
		break;
	case PIXEL_FORMAT_YUYV:
		return "PIXEL_FORMAT_YUVV";
		break;
	default:
		return "unknown";
		break;
	}
}


void initColorSettings(VideoStream& videos)
{
	const SensorInfo& colorInfo = videos.getSensorInfo();
	videos.setVideoMode(colorInfo.getSupportedVideoModes()[COL_VIDEO_MODE]);
	videos.getCameraSettings()->setAutoExposureEnabled(true);
	videos.getCameraSettings()->setAutoWhiteBalanceEnabled(true);
	videos.setMirroringEnabled(false);
}

void initDepthSettings(VideoStream& videos)
{
	const SensorInfo& depthInfo = videos.getSensorInfo();
	videos.setVideoMode(depthInfo.getSupportedVideoModes()[DEP_VIDEO_MODE]);
	videos.setMirroringEnabled(false);
	videos.setProperty(XN_STREAM_PROPERTY_CLOSE_RANGE, true);
}

Mat	getColorImage(VideoStream& videos) 
{
	VideoFrameRef	   frame;
	const RGB888Pixel* pixel;
	Mat img;
	
	img.create(Size(WIN_XSIZE, WIN_YSIZE), CV_8UC3);
	videos.readFrame(&frame);
	pixel = (const RGB888Pixel*)frame.getData();
	/*unsigned char* tmp = (unsigned char*)pixel;
	int j = 0;

	for (int i = 0; i < WIN_XSIZE*WIN_YSIZE; i++)
	{
		img.data[j * 3] = tmp[i * 3 + 2];
		img.data[j * 3 + 1] = tmp[i * 3 + 1];
		img.data[j * 3 + 2] = tmp[i * 3];
		j++;
	}*/
	img.data = (unsigned char*)pixel;
	cvtColor(img, img, CV_BGR2RGB);
	return img;
}
/*
const DepthPixel* getDepthImage(VideoStream& videos)
{
	VideoFrameRef		frame;
	const DepthPixel*	pixel;
	Mat img;

	videos.readFrame(&frame);
	return (const DepthPixel*)frame.getData();
}
*/
Mat convertColorSpace(const Mat& src, unsigned int conversion)
{
	Mat dest;
	cvtColor(src, dest, conversion);
	return dest;
}

void getPointerPos(const Mat& image, const VideoStream& d, const DepthPixel* depth, float& xpos, float& ypos, float& zpos, int& cx, int& cy)
{
	const float ALPHA = 0.8;
	char buffer[WIN_XSIZE*WIN_YSIZE], buffer2[WIN_XSIZE*WIN_YSIZE];
	int counter, max = 0, max_i, max_j;
	float x = 0, y = 0, z = 0;
	static float oldxpos = 0, oldypos = 0, oldzpos = 0;

	for (int j = 0; j < WIN_YSIZE; j++)					/* Left to Right */
	{
		counter = 0;
		for (int i = 0; i < WIN_XSIZE; i++)				
		{
			counter = (image.data[i + j*WIN_XSIZE]) ? counter + 1 : (counter > 0) ? counter - 1 : counter;
			buffer[i + j*WIN_XSIZE] = counter;
		}
	}
	for (int j = 0; j < WIN_YSIZE; j++)					/* Right to Left */
	{
		counter = 0;
		for (int i = WIN_XSIZE-1; i >= 0; i--)		
		{
			counter = (image.data[i + j*WIN_XSIZE]) ? counter + 1 : (counter > 0) ? counter - 1 : counter;
			buffer[i + j*WIN_XSIZE] = MIN(buffer[i + j*WIN_XSIZE], counter);
		}
	}
	for (int i = 0; i < WIN_XSIZE; i++)					/* Top to Bottom */
	{	
		counter = 0;
		for (int j = 0; j < WIN_YSIZE; j++)
		{
			counter = (image.data[i + j*WIN_XSIZE]) ? counter + 1 : (counter > 0) ? counter - 1 : counter;
			buffer2[i + j*WIN_XSIZE] = counter;
		}
	}
	for (int i = 0; i < WIN_XSIZE; i++)					/* Bottom to Top */
	{
		counter = 0;
		for (int j = WIN_YSIZE-1; j >= 0; j--)
		{
			counter = (image.data[i + j*WIN_XSIZE]) ? counter + 1 : (counter > 0) ? counter - 1 : counter;
			buffer2[i + j*WIN_XSIZE] = MIN(buffer2[i + j*WIN_XSIZE],counter);
		}
	}
	for (int i = 0; i < WIN_XSIZE; i++)
	{
		counter = 0;
		for (int j = 0; j < WIN_YSIZE; j++)
		{
			buffer[i + j*WIN_XSIZE] += buffer2[i + j*WIN_XSIZE];
			if (buffer[i + j*WIN_XSIZE] > max)
			{
				max = buffer[i + j*WIN_XSIZE];
				max_i = i;
				max_j = j;
			}
		}
	}
	if (max > 2)
	{
		unsigned short int val = depth[max_i + max_j*WIN_XSIZE];
		if (val == 0) val = depth[max_i + 1 + max_j * 320];
		if (val == 0) val = depth[max_i - 1 + max_j * 320];
		if (val == 0) val = depth[max_i + (max_j + 1) * 320];
		if (val == 0) val = depth[max_i + (max_j - 1) * 320];

		if (val == 0) val = depth[max_i + 1 + (max_j + 1) * 320];
		if (val == 0) val = depth[max_i - 1 + (max_j + 1) * 320];
		if (val == 0) val = depth[max_i + 1 + (max_j - 1) * 320];
		if (val == 0) val = depth[max_i - 1 + (max_j - 1) * 320];

		if (val == 0) val = depth[max_i + 2 + max_j * 320];
		if (val == 0) val = depth[max_i - 2 + max_j * 320];
		if (val == 0) val = depth[max_i + (max_j + 2) * 320];
		if (val == 0) val = depth[max_i + (max_j - 2) * 320];

		if (val == 0) val = depth[max_i + 2 + (max_j + 2) * 320];
		if (val == 0) val = depth[max_i - 2 + (max_j + 2) * 320];
		if (val == 0) val = depth[max_i + 2 + (max_j - 2) * 320];
		if (val == 0) val = depth[max_i - 2 + (max_j - 2) * 320];

		cx = max_i;
		cy = max_j;

		if (val > 0)
		{
			CoordinateConverter::convertDepthToWorld(d, max_i, max_j, val, &x, &y, &z);
			xpos = x / 1000.0;
			ypos = y / 1000.0;
			zpos = z / 1000.0;
		}
		else
		{
			xpos = oldxpos;
			ypos = oldypos;
			zpos = oldzpos;
		}
	}
	else
	{
		xpos = oldxpos;
		ypos = oldypos;
		zpos = oldzpos;
	}
	xpos = ALPHA*xpos + (1 - ALPHA)*oldxpos;
	ypos = ALPHA*ypos + (1 - ALPHA)*oldypos;
	zpos = ALPHA*zpos + (1 - ALPHA)*oldzpos;

	oldxpos = xpos;
	oldypos = ypos;
	oldzpos = zpos;
}

void dropDepthVal(Mat& src, const DepthPixel* depth, unsigned int threshold)
{
	short unsigned int* buffer = (short unsigned int*)depth;

	for (int i = 0; i < WIN_XSIZE*WIN_YSIZE; i++)
		if (buffer[i] > threshold)	buffer[i] = 0;

	for (int i = 0; i < WIN_XSIZE; i++)
		for (int j = 0; j < WIN_YSIZE; j++)
			if (!buffer[i + j*WIN_XSIZE])
			{
				src.data[(i + j*WIN_XSIZE) * 3 + 0] = 0;	/* RED pixel   */
				src.data[(i + j*WIN_XSIZE) * 3 + 1] = 0;	/* GREEN pixel */
				src.data[(i + j*WIN_XSIZE) * 3 + 2] = 0;	/* BLUE pixel  */
			}
}

void callback(int, void*) {}

vector<Scalar> calibrateColor()
{
	static int rY_min = 1, rY_max = 1, rCr_min = 1, rCr_max = 1, rCb_min = 1, rCb_max = 1;
	static int gY_min = 1, gY_max = 1, gCr_min = 1, gCr_max = 1, gCb_min = 1, gCb_max = 1;
	
	const char* ww1 = "Arancione";
	namedWindow(ww1, CV_WINDOW_AUTOSIZE);

	createTrackbar("Y_min:", ww1, &rY_min, 254, callback);
	createTrackbar("Y_max:", ww1, &rY_max, 254, callback);
	createTrackbar("Cb_min:", ww1, &rCb_min, 254, callback);
	createTrackbar("Cb_max:", ww1, &rCb_max, 254, callback);
	createTrackbar("Cr_min:", ww1, &rCr_min, 254, callback);
	createTrackbar("Cr_max:", ww1, &rCr_max, 254, callback);

	const char * ww = "Verde";
	namedWindow(ww, CV_WINDOW_AUTOSIZE);

	createTrackbar("Y_min:", ww, &gY_min, 254, callback);
	createTrackbar("Y_max:", ww, &gY_max, 254, callback);
	createTrackbar("Cbmin:", ww, &gCb_min, 254, callback);
	createTrackbar("Cb_max:", ww, &gCb_max, 254, callback);
	createTrackbar("Cr_min:", ww, &gCr_min, 254, callback);
	createTrackbar("Cr_max:", ww, &gCr_max, 254, callback);

	vector<Scalar> ret(4);
	ret[2] = Scalar(rY_min, rCb_min, rCr_min);
	ret[3] = Scalar(rY_max, rCb_max, rCr_max);
	ret[0] = Scalar(gY_min, gCb_min, gCr_min);
	ret[1] = Scalar(gY_max, gCb_max, gCr_max);
	return ret;
}

vector<Scalar> readCalibValues(fstream& inFile)
{
	char s1[200];
	vector<Scalar> ret(4);
	vector<string> values;
	string valueSeparator = " ";

	while (!inFile.eof())
	{
		string line;
		getline(inFile, line);
		size_t pos = 0;
		while ((pos = line.find(valueSeparator)) != string::npos)
		{
			values.push_back(line.substr(0, pos));
			line = line.substr(pos + 1);
		}
	}
	int j = 0;
	for (int i = 0; i < ret.size(); i++)
	{
		ret[i] = Scalar(stoi(values[j]), stoi(values[j + 1]), stoi(values[j + 2]));
		j += 3;
	}
	inFile.close();
	return ret;
}

int saveCalibValues(vector<Scalar> data, const string filename)
{
	ofstream os(filename);
	if (os == nullptr)
	{
		cout << "Unable to open file" << endl;
		return -1;
	}
	for (int i = 0; i < data.size(); i++)
	{
		os << data[i][0] << " " << data[i][1] << " " << data[i][2] << " ";
	}
	os.close();
	return 0;
}