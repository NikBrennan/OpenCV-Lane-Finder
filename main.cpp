/*
Name: Nikolas Brennan
Stu#: 1109583
*/

#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// initial min and max HSV filter values
// these will be changed using trackbars
int MinH = 0;
int MaxH = 179;

int MinS = 0;
int MaxS = 255;

int MinV = 0;
int MaxV = 255;

int MinC = 120;
int MaxC = 150;

// Window Names
const string windowName = "Original";
const string windowName1 = "HSV Image";
const string windowName2 = "Threshold Image";
const string adjustmentWindow = "Fine Tuning";

bool Road = true;

// Functions
// Create a Window for the Fine Tuning

void on_trackbar(int, void*) {}; // Function called when track bar pos gets moved

void setRoad(int MinHue = 70, int MaxHue = 179, int MinSat = 0, int MaxSat = 87, int MinValue = 0, int MaxValue = 77, int MinCanny = 50, int MaxCanny = 150)
{
	MinH = MinHue;
	MaxH = MaxHue;

	MinS = MinSat;
	MaxS = MaxSat;

	MinV = MinValue;
	MaxV = MaxValue;

	MinC = MinCanny;
	MaxC = MaxCanny;
}

void createTrackBars()
{
	if (Road == true) { setRoad(); }
	namedWindow(adjustmentWindow, 0);
	// create memory to store track bar names on the window
	char TrackBarName[50];
	// create trackbars to insert into a window to fine tune values
	sprintf(TrackBarName, "MinH", MinH);
	sprintf(TrackBarName, "MaxH", MaxH);
	sprintf(TrackBarName, "MinS", MinS);
	sprintf(TrackBarName, "MaxS", MaxS);
	sprintf(TrackBarName, "MinV", MinV);
	sprintf(TrackBarName, "MaxV", MaxV);
	// canny thresholds
	sprintf(TrackBarName, "MinCanny", MinC);
	sprintf(TrackBarName, "MaxCanny", MaxC);

	// 3 parameters are address variables that change when the tracking is changed
	// max and min values tracking
	// the function that is called when bars are moved
	createTrackbar("MinH", adjustmentWindow, &MinH, MaxH, on_trackbar);
	createTrackbar("MaxH", adjustmentWindow, &MaxH, MaxH, on_trackbar);
	createTrackbar("MinS", adjustmentWindow, &MinS, MaxS, on_trackbar);
	createTrackbar("MaxS", adjustmentWindow, &MaxS, MaxS, on_trackbar);
	createTrackbar("MinV", adjustmentWindow, &MinV, MaxV, on_trackbar);
	createTrackbar("MaxV", adjustmentWindow, &MaxV, MaxV, on_trackbar);

	// canny
	createTrackbar("MinC", adjustmentWindow, &MinC, MaxC, on_trackbar);
	createTrackbar("MaxC", adjustmentWindow, &MaxC, MaxC, on_trackbar);
}

int main()
{
	VideoCapture capture("roadLane.mp4");

	if (!capture.isOpened())
	{
		cout << "error";
		return -1;
	}

	createTrackBars(); // calls and creates the rackbar slider box

	Mat threshhold, capt_Image, HSV_Img, dst;

	while (1)
	{
		// step 1 - get the image/video/camera feed
		capture >> capt_Image;

		resize(capt_Image, capt_Image, Size(500,640));
		
		// here we define our region of interest box (x, y, b, c);
		// (x,y) = (100,295)
		// and the second corner is (x + b, y + c) = (100 + 220, 295 + 185)
		Rect const box(100, 295, 320, 185);

		// step 2 - define a box
		Mat ROI = capt_Image(box); // region of interest

		// step 3 - canny
		// edge detection using canny detection
		GaussianBlur(ROI, dst, Size(3, 3), 1.5, 1.5);

		// Morphological opertaions
		Mat k = getStructuringElement(MORPH_RECT, Size(3, 3));
		morphologyEx(ROI, ROI, MORPH_CLOSE, k);

		Canny(ROI, dst, MinC, MaxC);

		// step 4 - convert image to hsv
		cvtColor(ROI, HSV_Img, COLOR_RGB2HSV);

		inRange(ROI, Scalar(70, 179, 0), Scalar(87, 0, 77), threshhold);

		// step 5 - use a vector to get HoughLinesP
		// now applying hough transform to detect lines in our image
		vector<Vec4i> lines;
		double rho = 1;
		double theta = CV_PI / 180;
		int threshold = 50;
		double minLineLength = 10;
		double maxLineGap = 300;
		HoughLinesP(dst, lines, rho, theta, threshold, minLineLength, maxLineGap);

		// draw lines on screen over the road
		int lineThickness = 10;
		int lineType = LINE_4;

		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			Scalar greenColor = Scalar(0, 250, 0);
			line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, lineThickness, lineType);
		}

		//imshow(windowName, capt_Image);
		//imshow(windowName1, HSV_Img);
		imshow(windowName2, ROI);

		if (waitKey(20) == 27)
		{
			cout << "esc";
			break;
		}

	}
	return 0;
}
