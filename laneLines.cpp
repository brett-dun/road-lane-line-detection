
//this code is a c++ port of: https://github.com/georgesung/road_lane_line_detection
//todo: maybe just always use float, I don't see a reason to use doubles for this.

//Bugs:
// - does not detect yellow lines (but should be)

#include "laneLines.hpp"

#include <stdlib.h>

#include <iostream>

#include "opencv2/opencv.hpp"

#include "linearRegression.hpp"


//smoothing
const int KERNAL_SIZE = 3;
const cv::Size KERNAL = cv::Size(KERNAL_SIZE, KERNAL_SIZE);

//Canny Edge Detector
const int LOW_THRESHOLD = 50;
const int HIGH_THRESHOLD = 150;

//Region of interest vertices
//We want a trapezoid with the bottom edge at the bottom of the image
const float TRAP_BOTTOM_WIDTH = 0.85; //width of the bottom edge of the trapezoid as a percentage of image width
const float TRAP_TOP_WIDTH = 0.07; //width of the top edge of the trapezoid as a percentage of image width
const float TRAP_HEIGHT = 0.4; //height of the trapezoid as a percentage of image height

//Hough Transform
const float RHO = 2.0; //???
const float THETA = 1.0 * CV_PI / 180.0;
const int THRESHOLD = 15;
const int MIN_LINE_LENGTH = 10; //todo: this constant is related to image size
const int MIN_LINE_GAP = 20; //todo: this constant is related to image size


void regionOfInterest(cv::Mat& src, cv::Mat& dst, const cv::Point** vertices, int nVertices);
void drawLines(cv::Mat& img, cv::Mat& dst, std::vector<cv::Vec4i>& lines, cv::Scalar color = cv::Scalar(0, 0, 255), int thickness = 5);
void weightedImage(cv::Mat& src0, cv::Mat& src1, cv::Mat& dst, float alpha = 0.8, float beta = 1., float gamma = 0.);
void filterColors(cv::Mat& src, cv::Mat& dst);
void annotateMat(cv::Mat& src, cv::Mat& dst);
// void annotateImage(string inputFileName, string outputFileName = "out.png");
// void annotateVideo(string inputFileName, string outputFileName = "out.avi");


void regionOfInterest(cv::Mat& src, cv::Mat& dst, const cv::Point** vertices, int nVertices)
{
	//todo: handle different numbers of channels

	//create a mask that starts out as black
	cv::Mat mask = cv::Mat(src.size(), src.type(), cv::Scalar(0, 0, 0));

	cv::fillPoly(mask, vertices, &nVertices, 1, cv::Scalar(255, 255, 255));

	//output image will only have color in region of interest
	cv::bitwise_and(src, src, dst, mask);
}


void drawLines(cv::Mat& img, cv::Mat& dst, std::vector<cv::Vec4i>& lines, cv::Scalar color, int thickness)
{
	//todo
	// if (lines == NULL) { return; }

	if (lines.size() == 0) { return; }

	bool drawRight = true;
	bool drawLeft = true;

	const float SLOPE_THRESHOLD = 0.5;
	std::vector<float> slopes;
	std::vector<cv::Vec4i> newLines;

	for (cv::Vec4i line : lines)
	{
		float x0 = line[0];
		float y0 = line[1];
		float x1 = line[2];
		float y1 = line[3];

		float slope;

		if (x1 - x0 == 0.0)
		{
			slope = 999999999; //very high slope
		} 
		else
		{
			slope = (y1 - y0) / (x1 - x0);
		}

		if (fabs(slope) > SLOPE_THRESHOLD)
		{
			slopes.push_back(slope);
			newLines.push_back(line);
		}

	}

	std::vector<cv::Vec4i> rightLines;
	std::vector<cv::Vec4i> leftLines;

	for (int i = 0; i < lines.size(); ++i)
	{

		cv::Vec4f line = lines[i];

		float x0 = line[0];
		float y0 = line[1];
		float x1 = line[2];
		float y1 = line[3];

		float imgXCenter = img.size().width / 2.;

		if (slopes[i] > 0 && x0 > imgXCenter && x1 > imgXCenter)
		{
			rightLines.push_back(line);
		}
		else if (slopes[i] < 0 && x0 < imgXCenter && x1 < imgXCenter)
		{
			leftLines.push_back(line);
		}

	}

	std::vector<float> rightLinesX, rightLinesY;

	for (cv::Vec4i line : rightLines)
	{
		float x0 = line[0];
		float y0 = line[1];
		float x1 = line[2];
		float y1 = line[3];

		rightLinesX.push_back(x0);
		rightLinesX.push_back(x1);

		rightLinesY.push_back(y0);
		rightLinesY.push_back(y1);
	}

	float rightM, rightB;

	if (rightLinesX.size() > 0)
	{
		//use linear regression to find the slope and intercept of the right lane line
		linearRegression(rightLinesX, rightLinesY, rightM, rightB);
	}
	else
	{
		rightM = 1;
		rightB = 1;

		drawRight = false;
	}

	std::vector<float> leftLinesX, leftLinesY;

	for (cv::Vec4i line : leftLines)
	{
		float x0 = line[0];
		float y0 = line[1];
		float x1 = line[2];
		float y1 = line[3];

		leftLinesX.push_back(x0);
		leftLinesX.push_back(x1);

		leftLinesY.push_back(y0);
		leftLinesY.push_back(y1);
	}

	float leftM, leftB;

	if (leftLines.size() > 0)
	{
		//use linear regression to find the slope and intercept of the left lane line
		linearRegression(leftLinesX, leftLinesY, leftM, leftB);
	}
	else
	{
		leftM = 1;
		leftB = 1;

		drawLeft = false;
	}

	int y0 = img.size().height;
	float y1 = y0 * (1.0 - TRAP_HEIGHT);

	int y1Int = static_cast<int>(y1);

	if (drawRight)
	{
		int rightX0 = static_cast<int>( (y0 - rightB) / rightM );
		int rightX1 = static_cast<int>( (y1 - rightB) / rightM );

		cv::line(dst, cv::Point(rightX0, y0), cv::Point(rightX1, y1), color, thickness);
	}

	if (drawLeft)
	{
		int leftX0 = static_cast<int>( (y0 - leftB) / leftM );
		int leftX1 = static_cast<int>( (y1 - leftB) / leftM );

		cv::line(dst, cv::Point(leftX0, y0), cv::Point(leftX1, y1), color, thickness);
	}

}


//helper function with constants predefined
void weightedImage(cv::Mat& src0, cv::Mat& src1, cv::Mat& dst, float alpha, float beta, float gamma)
{
	cv::addWeighted(src0, alpha, src1, beta, gamma, dst);
}


void filterColors(cv::Mat& src, cv::Mat& dst)
{
	cv::Mat whiteMask, whiteImage, yellowMask, yellowImage, hsv;

	//Filter white pixels
	const int WHITE_THRESHOLD = 200;
	const cv::Scalar LOWER_WHITE = cv::Scalar(WHITE_THRESHOLD, WHITE_THRESHOLD, WHITE_THRESHOLD); //todo: do we want the same threshold for each channel?
	const cv::Scalar UPPER_WHITE = cv::Scalar(255, 255, 255);

	cv::inRange(src, LOWER_WHITE, UPPER_WHITE, whiteMask);
	cv::bitwise_and(src, src, whiteImage, whiteMask);

	//Convert to hsv for use in identifying yellow regions
	cv::cvtColor(src, hsv, CV_BGR2HSV);

	//Filter yellow pixels
	const cv::Scalar LOWER_YELLOW = cv::Scalar(90/2, 100, 100);
	const cv::Scalar UPPER_YELLOW = cv::Scalar(110/2, 255, 255);

	cv::inRange(hsv, LOWER_YELLOW, UPPER_YELLOW, yellowMask);
	cv::bitwise_and(src, src, yellowImage, yellowMask);

	//add the white and yellow image together with equal weights
	weightedImage(whiteImage, yellowImage, dst, 1., 1.);

}


void annotateMat(cv::Mat& src, cv::Mat& dst)
{
	cv::Mat image, gray, blurGray, edges, maskedEdges, lineImage;

	filterColors(src, image);

	cv::cvtColor(image, gray, CV_BGR2GRAY);

	cv::blur(gray, blurGray, KERNAL);

	//todo: decide on what kernal size is to be used here
	cv::Canny(blurGray, edges, LOW_THRESHOLD, HIGH_THRESHOLD, KERNAL_SIZE);

	cv::Point vertices[4];

	cv::Size size = src.size();

	//opencv coordinates are reversed for the x axis, so size.height is at the bottom
	int topY = static_cast<int>(size.height - size.height * TRAP_HEIGHT);

	//the image width * ratio of width not part of trapezoid / 2 (to split the distance evenly on each side) 
	float bottomLeftX = size.width * (1 - TRAP_BOTTOM_WIDTH) / 2.;
	float topLeftX = size.width * (1 - TRAP_TOP_WIDTH) / 2.;

	vertices[0] = cv::Point(static_cast<int>(bottomLeftX), size.height); //bottom left
	vertices[1] = cv::Point(static_cast<int>(topLeftX), topY); //top ?
	vertices[2] = cv::Point(static_cast<int>(size.width - topLeftX), topY); //top ?
	vertices[3] = cv::Point(static_cast<int>(size.width - bottomLeftX), size.height); //bottom right

	const cv::Point* v[4] = {&vertices[0], &vertices[1], &vertices[2], &vertices[3]};

	//todo: does doing this sooner reduce the computations required?
	//black out the parts of image that are outside of where we will look for the lane
	regionOfInterest(edges, maskedEdges, v, 4);

	//vector to store the lines (each line is represented by its begining and end x and y coords)
	std::vector<cv::Vec4i> lines;

	//use probabilistic hough lines to save time 
	cv::HoughLinesP(maskedEdges, lines, RHO, THETA, THRESHOLD, MIN_LINE_LENGTH, MIN_LINE_GAP);

	//image defaults to black
	lineImage = cv::Mat(src.size(), src.type(), cv::Scalar(0, 0, 0));

	//draw the lines
	drawLines(image, lineImage, lines);

	//combine the image with lines with the original image
	weightedImage(lineImage, src, dst);

}


void annotateImage(const std::string& inputFileName, const std::string& outputFileName)
{
	cv::Mat image = cv::imread(inputFileName, cv::IMREAD_COLOR);

	if (image.empty())
	{
		//todo: cout
		exit(-1);
	}

	cv::Mat annotated;

	annotateMat(image, annotated);

	cv::imwrite(outputFileName, annotated);

}


//todo: test and debug
void annotateVideo(const std::string& inputFileName, const std::string& outputFileName)
{

	std::cout << "Starting..." << std::endl;

	cv::VideoCapture cap = cv::VideoCapture(inputFileName);

	if (!cap.isOpened())
	{
		//todo: cout
		exit(-1);
	}

	const int FRAME_WIDTH = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	const int FRAME_HEIGHT = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	//todo: determine frame rate
	cv::VideoWriter video = cv::VideoWriter(outputFileName, CV_FOURCC('M', 'J', 'P', 'G'), 30, cv::Size(FRAME_WIDTH, FRAME_HEIGHT));

	while (1)
	{
		cv::Mat frame, annotated;

		cap >> frame;

		if (frame.empty()) { break; }

		annotateMat(frame, annotated);

		video.write(annotated);

	}

	std::cout << "[Done]" << std::endl;

}
