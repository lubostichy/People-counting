/*
* Po��t�n� pr�chod� osob dve�mi s vyu�it�m stacion�rn� kamery
* Bakal�rska pr�ca, 2016
* �ubo� Tich�, xtichy23
* Box.cpp
*/

#include "Box.h"
#include <opencv2/opencv.hpp>

using namespace cv;

/* Kon�truktor bounding boxu */
Box::Box()
{
}

/* Kon�truktor bounding boxu */
Box::Box(int type, cv::Rect box)
{
	this->type = type;
	this->bbox = Rect(box);
	
}

/* Kon�truktor bounding boxu */
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


/* De�truktor bounding boxu */
Box::~Box()
{
}

/* Nastav� poradov� ��slo bounding boxu. */
void Box::setFrameNO(int NO)
{
	this->frameNO = NO;
}

/* Vytla�� inform�cie o bounding boxe na obrazovku. */
std::string Box::printBox()
{
	std::stringstream ss;
	ss << "#" << this->frameNO << " [" << this->bbox.x << "," << this->bbox.y << "," << this->bbox.width << "," << this->bbox.height << "]";
	return ss.str();
}
