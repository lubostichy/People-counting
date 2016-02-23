#include "Box.h"
#include <opencv2/opencv.hpp>

Box::Box()
{
}


Box::Box(int type, cv::Rect box)
{
	this->type = type;
	this->bbox = box;
	
}

Box::Box(int type, cv::Rect box, int number)
{
	this->type = type;
	this->bbox = box;
	this->frameNO = number;
}


Box::Box(const Box& pBox)
{
	
	this->bbox = pBox.bbox;
	this->frameNO = pBox.frameNO;
	this->type = pBox.type;
}

Box::~Box()
{
}

void Box::setFrameNO(int NO)
{
	this->frameNO = NO;
}

std::string Box::printBox()
{
	std::stringstream ss;
	ss << "#" << this->frameNO << " [" << this->bbox.x << "," << this->bbox.y << "," << this->bbox.width << "," << this->bbox.height << "]";
	return ss.str();
}