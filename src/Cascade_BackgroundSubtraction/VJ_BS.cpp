#include "VJ_BS.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../Box.h"



using namespace std;
using namespace cv;

VJ_BS::VJ_BS()
{
}


VJ_BS::~VJ_BS()
{
}

/*detekcia detektorom z OpenCV */
void VJ_BS::detect()
{
	vector <Rect> people;
	
	vector <Rect> boundRect;

	boundRect = this->getBoundRects();

	Rect largeRect;
	Mat roi;

	for (vector<Rect>::iterator it = boundRect.begin(); it != boundRect.end(); ++it)	
	{

		if (((*it).width >= this->minSize) && ((*it).height >= this->minSize))
		{
			largeRect = this->enlargeRect(*it, 2.5, 2);
			//rectangle(this->RGBFrame, largeRect, Scalar(255, 0, 0));

			roi = (this->RGBFrame)(largeRect);

			this->cascadeClassifier.detectMultiScale(roi, people, 2, 3, 0, Size(this->minSize, this->minSize), Size((this->RGBFrame).size().width, (this->RGBFrame).size().height));

			for (unsigned int j = 0; j < people.size(); j++)
			{
				people[j].x += largeRect.x;
				people[j].y += largeRect.y;
				// zmensit ludi
				people[j] = decreaseRect(people[j], 0.4, 0.3);
				rectangle(this->RGBFrame, people[j], Scalar(0, 255, 0), 2);
				this->boxes.push_back(Box(DETECTOR, people[j]));
				
			}

		}
	}

	
}

vector<Rect> VJ_BS::getBoundRects()
{
	vector<vector <Point>> contours;

	erode(this->BWFrame, this->BWFrame, Mat());
	dilate(this->BWFrame, this->BWFrame, Mat());
	
	findContours(this->BWFrame, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)	{

		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		rectangle(this->BWFrame, boundRect[i], Scalar(0,0,250));
		
		
	}
	
	return boundRect;
}

// zvacsi o tolkokrat Rect
Rect VJ_BS::enlargeRect(Rect littleRect, int coefWidth, int coefHeight)
{
	Rect largeRect;
	// sirka
	largeRect.width = ((littleRect.x + littleRect.width * coefWidth) < this->RGBFrame.size().width) ? littleRect.width * coefWidth : this->RGBFrame.size().width - littleRect.x;
	
	// vyska
	largeRect.height = ((littleRect.y + littleRect.height * coefHeight) < this->RGBFrame.size().height) ? littleRect.height * coefHeight : this->RGBFrame.size().height - littleRect.y;

	largeRect.x = ((littleRect.x - ((largeRect.width - littleRect.width) / 2)) > 0) ? littleRect.x - ((largeRect.width - littleRect.width) / 2) : 0;
	largeRect.y = ((littleRect.y - ((largeRect.height - littleRect.height) / 2)) > 0) ? littleRect.y - ((largeRect.height - littleRect.height) / 2) : 0;

	return largeRect;
}

// zmensi o tolkokrat Rect (coef < 1)
Rect VJ_BS::decreaseRect(Rect bigRect, float coefWidth, float coefHeight)
{
	Rect littleRect;

	//sirka
	littleRect.width = bigRect.width * (1 - coefWidth);

	// vyska
	littleRect.height = bigRect.height * (1 - coefHeight);

	littleRect.x = bigRect.x + (bigRect.width - littleRect.width) / 2;
	//littleRect.y = bigRect.y + (bigRect.height - littleRect.height) / 2;
	littleRect.y = bigRect.y;

	return littleRect;
}

void VJ_BS::setVJ_BS(string cascadeName, int size)
{
	if (cascadeClassifier.empty())
	{
		if (!cascadeClassifier.load(cascadeName))
		{
			std::cerr << "Error: Can not load a cascade classifier." << std::endl;
			exit(1);
		}
	}
	this->minSize = size;

}

void VJ_BS::setFrames(Mat BWframe, Mat RGBframe)
{
	this->BWframe = BWframe;
	this->RGBframe = RGBframe;
	
}

