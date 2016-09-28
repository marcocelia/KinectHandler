// KinectHandler.cpp : Defines the entry point for the console application.
//
#include "util.h"

const string FILENAME = "calib.txt";
const unsigned int THRESHOLD = 2000;
const char DEST_IP[16] = "127.0.0.1";
const unsigned int DEST_PORT = 45000;

int _tmain(int argc, _TCHAR* argv[])
{
	VideoStream color, depth;
	int res = OpenNI::initialize();

	Array<DeviceInfo> camera;
	OpenNI::enumerateDevices(&camera);

	if (camera.getSize() < 1)
	{
		cout << "No devices Connected" << endl;
		OpenNI::shutdown();
		return 1;
	}

	int device_id = 0;
	Device device;
	res = device.open(camera[device_id].getUri());

	res = depth.create(device, SENSOR_DEPTH);
	res = color.create(device, SENSOR_COLOR);

	device.setDepthColorSyncEnabled(TRUE);
	device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);

	initColorSettings(color);
	initDepthSettings(depth);

	openni::VideoStream** input_streams = new openni::VideoStream*[2];
	input_streams[0] = &color;
	input_streams[1] = &depth;

	color.start();
	depth.start();

	int depthIndex = 0, colorIndex = 0;
	
	VideoFrameRef		depthFrame;
	const DepthPixel*	depthpixel;

	NetConn connection(DEST_IP, DEST_PORT);
	res = connection.getResult();
	if (res != 0)
	{
		cout << "Connection errors\nreturning..." << endl;
		return 1;
	}

	Mat colorImage, green_destframe, orange_destframe;
	vector<Scalar> min_max(4);
	
	bool calibration = false;

	fstream calibFile(FILENAME);
	if (calibFile == nullptr)
	{
		min_max[0] = Scalar(1, 1, 1);
		min_max[1] = Scalar(254, 254, 254);
		min_max[2] = Scalar(1, 1, 1);
		min_max[3] = Scalar(254, 254, 254);
		calibration = true;
		cout << "No calibration file detected" << endl;
	}
	else min_max = readCalibValues(calibFile);

	int MarkerId = 0, MarkerIdOld = 0;
	MarkerDetector MDetect;
	vector<Marker> Markers;

	while (true)
	{
		do
		{
			int streamIndex = -1;
			OpenNI::waitForAnyStream(input_streams, 2, &streamIndex);
			switch(streamIndex)
			{
			case 0:
				colorImage = getColorImage(color);
				colorIndex = 1;
				break;
			case 1:
				depth.readFrame(&depthFrame);
				depthpixel = (const DepthPixel*)depthFrame.getData();
				depthIndex = 1;
				break;
			}
		} while (!depthIndex || !colorIndex);
		depthIndex = colorIndex = 0;

		if (calibration)
		{
			min_max = calibrateColor();
			Mat colorImage_YCrCb = convertColorSpace(colorImage, CV_BGR2YCrCb);
			inRange(colorImage_YCrCb, min_max[0], min_max[1], green_destframe);
			inRange(colorImage_YCrCb, min_max[2], min_max[3], orange_destframe);
			imshow("Filtering Green", green_destframe);
			imshow("Filtering Orange", orange_destframe);
		}
		else	
		{
			MDetect.detect(colorImage, Markers);
			dropDepthVal(colorImage, depthpixel, THRESHOLD);
			for (int i = 0; i < Markers.size(); i++) Markers[i].draw(colorImage, Scalar(0,0,255), 2);
			if (Markers.size() > 0)
			{
				MarkerId = Markers[0].id;
				if (MarkerId != MarkerIdOld) connection.mysend(ARUCO_ID, (float)MarkerId, (float)0, (float)0);
				MarkerIdOld = MarkerId;
			}
		
			Mat colorImage_YCrCb = convertColorSpace(colorImage, CV_BGR2YCrCb);
			
			Mat orangeComponent;
			inRange(colorImage_YCrCb, min_max[2], min_max[3], orangeComponent);
		
			Mat greenComponent;
			inRange(colorImage_YCrCb, min_max[0], min_max[1], greenComponent);

			int cx = 0, cy = 0;
			float xpos, ypos, zpos;

			getPointerPos(greenComponent, depth, depthpixel, xpos, ypos, zpos, cx, cy);
			circle(colorImage, Point(cx, cy), 3, Scalar(0, 255, 255));
			if (connection.mysend(GREEN_ID, -xpos, ypos, zpos) < 0)
			{
				cout << "Error in mysend: " << res << endl;
				return 1;
			}
				
			getPointerPos(orangeComponent, depth, depthpixel, xpos, ypos, zpos, cx, cy);
			circle(colorImage, Point(cx, cy), 3, Scalar(0, 255, 255));
			if (connection.mysend(ORANGE_ID, -xpos, ypos, zpos) < 0)
			{
				cout << "Error in mysend: " << res << endl;
				return 1;
			}

			imshow("Color Image YCrCb", colorImage_YCrCb);
			imshow("Orange", orangeComponent);
			imshow("Green", greenComponent);
		}
		resize(colorImage, colorImage, Size(WIN_XSIZE * 2, WIN_YSIZE * 2));
		imshow("Color Image RGB", colorImage);					//Shows Camera Image
		resize(colorImage, colorImage, Size(WIN_XSIZE, WIN_YSIZE));

		int c = waitKey(5);
		switch (c)
		{
		case 's':
			if (!saveCalibValues(min_max, FILENAME)) cout << "Calibration saved..." << endl;
			calibration = false;
			break;
		case 'c':
			calibration = true;
			break;
		}
	}
	return 0;
}