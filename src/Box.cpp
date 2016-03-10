#include "Box.h"
#include <opencv2/opencv.hpp>

using namespace cv;

Box::Box()
{
}


Box::Box(int type, cv::Rect box)
{
	this->type = type;
	this->bbox = Rect(box);
	
}

Box::Box(int type, cv::Rect box, int number)
{
	this->type = type;
	this->bbox = Rect(box);
	this->frameNO = number;
}


Box::Box(const Box& pBox)
{
	
	this->bbox = pBox.bbox;
	this->frameNO = pBox.frameNO;
	this->type = pBox.type;
}


void Box::operator=(const Box& pBox)
{
	bbox = pBox.bbox;
	type = pBox.type;
	frameNO = pBox.frameNO;

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
