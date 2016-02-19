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
	_averageWidth = config.widthOfBox;
	_averageHeight = config.heightOfBox;
}

void CC::detect()
{
	vector <Rect> people;
	
//	_cascadeClassifier.detectMultiScale(_RGBframe, people, 2, 3, 0, Size(_averageWidth - 10, _averageHeight - 30), Size(_averageWidth + 50, _averageHeight + 40));
	_cascadeClassifier.detectMultiScale(_RGBframe, people, 2, 3, 0);
	for (unsigned int i = 0; i < people.size(); i++)
	{
		people[i] = decreaseRect(people[i], 0.4, 0.3);
		//cout << people[i].width << ", " << people[i].height << endl;
		if (isInArea(people[i]) && isGoodSize(people[i]))
		{
			rectangle(_RGBframe, people[i], Scalar(0, 0, 255), 2); // red
			this->boxes.push_back(Box(DETECTOR, people[i]));
		}
	}
}

bool CC::isInArea(Rect rect)
{
	switch (_line)
	{
	case VERTICAL:
		return ((rect.x + rect.width / 2 > _left)
			&&
			(rect.x + rect.width / 2 < _right));
		break;
	case HORIZONTAL:
		break;
	}
	return false;
}

bool CC::isGoodSize(Rect rect)
{	
	return (((_averageWidth + 20) > rect.width) && (rect.width > (_averageWidth - 20))
		&&
		((_averageHeight + 40) > rect.height) && (rect.height > (_averageHeight - 40)));
}

Rect CC::decreaseRect(Rect bigRect, float coefWidth, float coefHeight)
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



void CC::setFrame(Mat RGBframe)
{
	_RGBframe = RGBframe;
}

