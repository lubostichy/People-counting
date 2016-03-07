#pragma once

#include <opencv2/opencv.hpp>
#include "../Box.h"

using namespace cv;

class VJ_BS
{
	unsigned int minSize;
	Mat RGBFrame;
	Mat BWFrame;

public:
	VJ_BS();
	~VJ_BS();
	std::vector <Box> boxes;
	void detect();
	void setFrames(Mat, Mat);	
	void setVJ_BS(struct Config);
	
private:
	Mat _RGBframe;
	Mat _BWframe;
	CascadeClassifier _cascadeClassifier;
	int _line, _left, _middle, _right;
	int _minWidth, _maxWidth;
	int _minHeight, _maxHeight;
	//int _averageWidth, _averageHeight;
	int _range;

	vector<Rect> getBoundRects();
	Rect enlargeRect(Rect, int, int);
	Rect decreaseRect(Rect, float, float);
	bool isInArea(Rect);
	bool isGoodSize(Rect);
};

