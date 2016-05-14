/*
* Poèítání prùchodù osob dveømi s využitím stacionární kamery
* Bakalárska práca, 2016
* ¼uboš Tichý, xtichy23
* Box.cpp
*/

#include "Box.h"
#include <opencv2/opencv.hpp>

using namespace cv;

/* Konštruktor bounding boxu */
Box::Box()
{
}

/* Konštruktor bounding boxu */
Box::Box(int type, cv::Rect box)
{
	this->type = type;
	this->bbox = Rect(box);
	
}

/* Konštruktor bounding boxu */
Box::Box(int type, cv::Rect box, int number)
{
	this->type = type;
	this->bbox = Rect(box);
	this->frameNO = number;
}

/* Copy constructor */
Box::Box(const Box& pBox)
{
	
	this->bbox = pBox.bbox;
	this->frameNO = pBox.frameNO;
	this->type = pBox.type;
}

/* assignment operator */
void Box::operator=(const Box& pBox)
{
	bbox = pBox.bbox;
	type = pBox.type;
	frameNO = pBox.frameNO;

}


/* Deštruktor bounding boxu */
Box::~Box()
{
}

/* Nastaví poradové èíslo bounding boxu. */
void Box::setFrameNO(int NO)
{
	this->frameNO = NO;
}

/* Vytlaèí informácie o bounding boxe na obrazovku. */
std::string Box::printBox()
{
	std::stringstream ss;
	ss << "#" << this->frameNO << " [" << this->bbox.x << "," << this->bbox.y << "," << this->bbox.width << "," << this->bbox.height << "]";
	return ss.str();
}
