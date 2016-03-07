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

	_averageWidth = averageWidth;
	_averageHeight = averageHeight;
	//_range = 40;
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

	threshold(_BWframe, _BWframe, 250, 255, 0);

	erode(_BWframe, _BWframe, elementErosion);
	dilate(_BWframe, _BWframe, elementDilation);

	imshow("MOG2, erode, dilate", _BWframe);
	waitKey(1);
	
	findContours(_BWframe.clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<Point> approx;

	
	
	vector<Rect> boundRect(contours.size());
	vector<vector<Point> > contours_poly(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		//rectangle(_RGBframe, boundRect[i], Scalar(255, 0, 0), 1);
		
		if (isGoodSize(boundRect[i]))
		{
			
			rectangle(_RGBframe, boundRect[i], Scalar(0, 255, 0), 4);
			boxes.push_back(Box(DETECTOR, boundRect[i]));
		}
		
		//if (contourArea(contours[i]) > 2000) // 2000 pri kastieli
		{			
		//	boundRect[i] = boundingRect(Mat(contours[i]));
			
		}
	}
}

void BS::findBoundingBoxes()
{
	vector<vector <Point>> contours;

	int erosionSize = 1;
	cv::Mat elementErosion = cv::getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));

	int dilationSize = 7;
	Mat elementDilation = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilationSize + 1, 2 * dilationSize + 1),
		Point(dilationSize, dilationSize));

	threshold(_BWframe, _BWframe, 250, 255, 0);

	erode(_BWframe, _BWframe, elementErosion);
	dilate(_BWframe, _BWframe, elementDilation);

	findContours(_BWframe, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)	
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		boundRect[i] = enlargeRect(boundRect[i], 2.5, 2);
		if ((boundRect[i].width > _averageWidth) && (boundRect[i].height > _averageHeight))
		{
			rectangle(_RGBframe, boundRect[i], Scalar(255, 0, 0));
			boundingBoxes.push_back(boundRect[i]);
		}
	}
}

Rect BS::enlargeRect(Rect littleRect, int coefWidth, int coefHeight)
{
	Rect largeRect;
	// sirka
	largeRect.width = ((littleRect.x + littleRect.width * coefWidth) < _RGBframe.size().width) ? littleRect.width * coefWidth : _RGBframe.size().width - littleRect.x;

	// vyska
	largeRect.height = ((littleRect.y + littleRect.height * coefHeight) < _RGBframe.size().height) ? littleRect.height * coefHeight : _RGBframe.size().height - littleRect.y;

	largeRect.x = ((littleRect.x - ((largeRect.width - littleRect.width) / 2)) > 0) ? littleRect.x - ((largeRect.width - littleRect.width) / 2) : 0;
	largeRect.y = ((littleRect.y - ((largeRect.height - littleRect.height) / 2)) > 0) ? littleRect.y - ((largeRect.height - littleRect.height) / 2) : 0;

	return largeRect;
}

void BS::setFrames(Mat BWframe, Mat RGBframe)
{
	_BWframe = BWframe;
	_RGBframe = RGBframe;

}

bool BS::isGoodSize(Rect rect)
{
	
	return (((_averageWidth + _averageWidth * 0.2) > rect.width) && (rect.width > (_averageWidth - _averageWidth * 0.3))
		&&
		((_averageHeight + _averageHeight * 0.2) > rect.height) && (rect.height > (_averageHeight - _averageHeight * 0.2)));

}

/*
void BS::detect(Mat BWFrame, Mat RGBFrame)
{
	_BWFrame = BWFrame;
	_RGBFrame = RGBFrame;
	_Xmin = 200;
	_Ymin = 50;
	_Xmax = _RGBFrame.size().width - 150;
	_Ymax = _RGBFrame.size().height - 50;

	vector<vector <Point>> contours;

	erode(_BWFrame, _BWFrame, Mat());
	dilate(_BWFrame, _BWFrame, Mat());
	

	findContours(_BWFrame, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)	{

		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));

	}

	Mat roi;

	for (vector<Rect>::iterator it = boundRect.begin(); it != boundRect.end(); ++it)
	{
		if (((*it).width >= _minSize) && ((*it).height >= _minSize))
		{
			// oblast zaujmu
			if	   (((*it).x > _Xmin)
				&& ((*it).x + (*it).width < _Xmax)
				&& ((*it).y > _Ymin)
				&& ((*it).y + (*it).height < _Ymax))
			{

				//rectangle(_RGBFrame, *it, Scalar(0, 255, 0), 2);
				_boxes.push_back(Box(DETECTOR, *it));
			}
			//rectangle(_RGBFrame, Rect(Point(200, 50), Point(_RGBFrame.size().width - 150, _RGBFrame.size().height - 50)), Scalar(255, 0, 0), 2);
		
		}
	}

}
*/

