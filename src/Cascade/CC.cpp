#include "CC.h"
#include "opencv2\opencv.hpp"
#include "../../Main.h"

CC::CC()
{

}

CC::~CC()
{

}

void CC::setCC(struct Config config)
{
	if (_cascadeClassifier.empty())
	{
		if (!_cascadeClassifier.load(config.cascadeClassifier))
		{
			std::cerr << "Error: Can not load a cascade classifier." << std::endl;
			exit(1);
		}
	}

	_line = config.typeOfLine;
	_left = config.leftPoint;
	_middle = config.middlePoint;
	_right = config.rightPoint;
	_minWidth = config.minWidthBox;
	_maxWidth = config.maxWidthBox;
	_minHeight = config.minHeightBox;
	_maxHeight = config.maxHeightBox;
	//_averageWidth = config.widthOfBox;
	//_averageHeight = config.heightOfBox;
}

void CC::detect()
{
	vector <Rect> people;
	
	
	
	// HAAR	
	/*
	cvtColor(_RGBframe, _BWframe, CV_BGR2GRAY);
	equalizeHist(_BWframe, _BWframe);
	_cascadeClassifier.detectMultiScale(_BWframe, people, 1.3, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(300, 300)); // HAAR

	for (unsigned int i = 0; i < people.size(); i++)
	{
		people[i] = decreaseRect(people[i], 0.5, 0.5);
		rectangle(_RGBframe, people[i], Scalar(0,255,0), 2);
		boxes.push_back(Box(DETECTOR, people[i]));
	}
	*/

	// HOG
	
	_cascadeClassifier.detectMultiScale(_RGBframe, people, 1.5, 3, 0); // HOG
	
	for (unsigned int i = 0; i < people.size(); i++)
	{	
		if (isInArea(people[i]))
		{

			//rectangle(_RGBframe, people[i], Scalar(255, 0, 0), 2); // blue
			people[i] = decreaseRect(people[i], 0.6, 0.9);
			//rectangle(_RGBframe, people[i], Scalar(0, 255, 0), 2); // green
			//cout << people[i].width << " / " << people[i].height << endl;
			if (isGoodSize(people[i]))
			{
				rectangle(_RGBframe, people[i], Scalar(0, 255, 0), 2); // green
				boxes.push_back(Box(DETECTOR, people[i]));
			}
		}
	}
	
	
	people.clear();
}

bool CC::isInArea(Rect rect)
{
	switch (_line)
	{
	case VERTICAL:
		return ((rect.x + rect.width / 2 >= _left)
			&&
			(rect.x + rect.width / 2 <= _right));
		break;
	case HORIZONTAL:
		return ((rect.y + rect.height / 2 >= _left)
			&&
			(rect.y + rect.height / 2 <= _right));
		break;
	}
	return false;
}

bool CC::isGoodSize(Rect rect) // 20, 20, 40, 70
{	
	return (_minWidth < rect.width && rect.width < _maxWidth)
		&&
		(_minHeight < rect.height && rect.height < _maxHeight);
}

Rect CC::decreaseRect(Rect bigRect, float coefWidth, float coefHeight)
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



void CC::setFrame(Mat RGBframe)
{
	_RGBframe = RGBframe;
}

