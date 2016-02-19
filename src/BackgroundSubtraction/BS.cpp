#include "opencv2\opencv.hpp"
#include "BS.h"

using namespace cv;
using namespace std;

BS::BS()
{
		
}

BS::~BS()
{
}


void BS::setBS(int averageWidth, int averageHeight)
{

	this->averageWidth = averageWidth;
	this->averageHeight = averageHeight;
	this->range = 40;
}

void BS::detect()
{
	vector<vector <Point>> contours; // kontury

	
	int erosionSize = 1;
	cv::Mat elementErosion = cv::getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));
	
	int dilationSize = 7;
	Mat elementDilation = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilationSize + 1, 2 * dilationSize + 1),
		Point(dilationSize, dilationSize));

	threshold(this->BWframe, this->BWframe, 250, 255, 0);

	erode(this->BWframe, this->BWframe, elementErosion);
	dilate(this->BWframe, this->BWframe, elementDilation);

	imshow("MOG2, erode, dilate",this->BWframe);
	waitKey(1);
	
	findContours(this->BWframe.clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<Point> approx;

	
	
	vector<Rect> boundRect(contours.size());
	vector<vector<Point> > contours_poly(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		//rectangle(this->RGBframe, boundRect[i], Scalar(255, 0, 0), 1);
		
		if (this->isGoodSize(boundRect[i]))
		{
			
			rectangle(this->RGBframe, boundRect[i], Scalar(0, 255, 0), 4);
			this->boxes.push_back(Box(DETECTOR, boundRect[i]));
		}
		
		//if (contourArea(contours[i]) > 2000) // 2000 pri kastieli
		{			
		//	boundRect[i] = boundingRect(Mat(contours[i]));
			
		}
	}

	/*

	for (vector<Rect>::iterator it = boundRect.begin(); it != boundRect.end(); ++it)
	{
		//if (((*it).width > this->averageWidth) && ((*it).width < 100)
		//	&& ((*it).height > 120) && ((*it).height < 200))
		if ((*it).width == 0)
			continue;
		
		if (this->isGoodSize(*it))
		{
			rectangle(this->RGBframe, *it, Scalar(0, 255, 0), 1);
			this->boxes.push_back(Box(DETECTOR, *it));
		}
	}
	*/


	

}


void BS::setFrames(Mat BWframe, Mat RGBframe)
{
	this->BWframe = BWframe;
	this->RGBframe = RGBframe;

}

bool BS::isGoodSize(Rect rect)
{
	
	return (((averageWidth + range) > rect.width) && (rect.width > (averageWidth - range))
		&&
		((averageHeight + range) > rect.height) && (rect.height > (averageHeight - range)));

}

/*
void BS::detect(Mat BWFrame, Mat RGBFrame)
{
	this->BWFrame = BWFrame;
	this->RGBFrame = RGBFrame;
	this->Xmin = 200;
	this->Ymin = 50;
	this->Xmax = this->RGBFrame.size().width - 150;
	this->Ymax = this->RGBFrame.size().height - 50;

	vector<vector <Point>> contours;

	erode(this->BWFrame, this->BWFrame, Mat());
	dilate(this->BWFrame, this->BWFrame, Mat());
	

	findContours(this->BWFrame, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)	{

		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));

	}

	Mat roi;

	for (vector<Rect>::iterator it = boundRect.begin(); it != boundRect.end(); ++it)
	{
		if (((*it).width >= this->minSize) && ((*it).height >= this->minSize))
		{
			// oblast zaujmu
			if	   (((*it).x > this->Xmin)
				&& ((*it).x + (*it).width < this->Xmax)
				&& ((*it).y > this->Ymin)
				&& ((*it).y + (*it).height < this->Ymax))
			{

				//rectangle(this->RGBFrame, *it, Scalar(0, 255, 0), 2);
				this->boxes.push_back(Box(DETECTOR, *it));
			}
			//rectangle(this->RGBFrame, Rect(Point(200, 50), Point(this->RGBFrame.size().width - 150, this->RGBFrame.size().height - 50)), Scalar(255, 0, 0), 2);
		
		}
	}

}
*/

