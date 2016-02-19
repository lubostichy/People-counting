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
	void setVJ_BS(string, int);

private:
	Mat RGBframe;
	Mat BWframe;
	CascadeClassifier cascadeClassifier;
	//std::string cascadeClassifier; // = "hogcascade_pedestrians.xml";
	vector<Rect> getBoundRects();
	Rect enlargeRect(Rect, int, int);
	Rect decreaseRect(Rect, float, float);
};

