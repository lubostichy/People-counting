#include "VJ_BS.h"
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../Main.h"



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

	boundRect = getBoundRects();

	Rect largeRect;
	Mat roi;
	
	for (vector<Rect>::iterator it = boundRect.begin(); it != boundRect.end(); ++it)	
	{
		largeRect = enlargeRect(*it, 2.5, 2);
		if (((largeRect).width > _minWidth) && ((largeRect).height > _minHeight))
		{			
			roi = _RGBframe(largeRect);
			
			_cascadeClassifier.detectMultiScale(roi, people, 1.5, 3, 0); //, Size(_minSize, _minSize), Size((_RGBFrame).size().width, (_RGBFrame).size().height));

			for (unsigned int j = 0; j < people.size(); j++)
			{
				people[j].x += largeRect.x;
				people[j].y += largeRect.y;				
				
				if (isInArea(people[j])) 
				{
					people[j] = decreaseRect(people[j], 0.6, 0.9);
					
					if (isGoodSize(people[j]))
					{
						rectangle(_RGBframe, people[j], Scalar(0, 255, 0), 2); // green
						boxes.push_back(Box(DETECTOR, people[j]));
					}
				}
				
			}

		}
	}

	
}

vector<Rect> VJ_BS::getBoundRects()
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

	for (unsigned int i = 0; i < contours.size(); i++)	{

		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		rectangle(_BWframe, boundRect[i], Scalar(0,0,250));
		
		
	}
	
	return boundRect;
}

bool VJ_BS::isInArea(Rect rect)
{
	switch (_line)
	{
	case VERTICAL:
		return ((rect.x + rect.width / 2 > _left)
			&&
			(rect.x + rect.width / 2 < _right));
		break;
	case HORIZONTAL:
		return ((rect.y + rect.height / 2 > _left)
			&&
			(rect.y + rect.height / 2 < _right));
		break;
	}
	return false;
}

bool VJ_BS::isGoodSize(Rect rect) 
{
	return (_minWidth < rect.width && rect.width < _maxWidth)
		&&
		(_minHeight < rect.height && rect.height < _maxHeight);
}

// zvacsi o tolkokrat Rect
Rect VJ_BS::enlargeRect(Rect littleRect, int coefWidth, int coefHeight)
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

// zmensi o tolkokrat Rect (coef < 1)
Rect VJ_BS::decreaseRect(Rect bigRect, float coefWidth, float coefHeight)
{
	Rect littleRect;

	//sirka
	littleRect.width = bigRect.width * coefWidth;

	// vyska
	littleRect.height = bigRect.height * coefHeight;

	littleRect.x = bigRect.x + (bigRect.width - littleRect.width) / 2;
	
	littleRect.y = bigRect.y + (bigRect.height - littleRect.height) / 2;
	

	return littleRect;
}

void VJ_BS::setVJ_BS(struct Config config)
{
	if (_cascadeClassifier.empty())
	{
		if (!_cascadeClassifier.load(config.cascadeClassifier))
		{
			std::cerr << "Error: Can not load a cascade classifier." << std::endl;
			exit(1);
		}
	}
	_minWidth = config.minWidthBox;
	_maxWidth = config.maxWidthBox;
	_minHeight = config.minHeightBox;
	_maxHeight = config.maxHeightBox;
	_line = config.typeOfLine;
	_left = config.leftPoint;
	_middle = config.middlePoint;
	_right = config.rightPoint;
}

void VJ_BS::setFrames(Mat BWframe, Mat RGBframe)
{
	_BWframe = BWframe;
	_RGBframe = RGBframe;
	
}

