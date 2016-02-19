#pragma once

#include <opencv2/opencv.hpp>

#define DETECTOR 1
#define TRACKER 2
#define COUNTER 3

class Box
{
public:
	Box();
	Box(int type, cv::Rect box);
	Box(const Box& pBox);
	~Box();
	void setFrameNO(int NO);
	int type;
	int frameNO;
	cv::Rect bbox;
	std::string printBox();

};

