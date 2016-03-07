#include "HT.h"
#include "DenseGreedyDetection\GreedyDetection.h"
#include <opencv2\opencv.hpp>
#include "Forests\HoughForest.h"
#include "../../Main.h"

using namespace cv;

HT::HT()
{
}


HT::~HT()
{
}

void HT::setDetector(struct Config config)// (int width, int height, int line, int left, int right)
{
	PatchSize = 16; // 16

	blur_radius = 5;

	PatchBgThreshold = 0.7; //0.7

	ProbVoteThreshold = 0.00005; //0.00005

	koef = 0.647;

	iNumberOfScales = 3;

	ResizeCoef = 0.85;

	bg_bias = 8.5;

	hyp_penalty = 1500;

	MaxObjectsCount = 8;

	forest_path = "pedestrian.dat";

	BBoxWidth = (config.maxWidthBox + config.minWidthBox) / 2 * koef; //50

	BBoxHeight = (config.maxHeightBox + config.minHeightBox) / 2 * koef; // 100

	HalfBBoxWidth = BBoxWidth / 1.5; // 30

	HalfBBoxHeight = BBoxHeight / 1.5;  // 50

	gd.SetForest(forest_path.c_str(),
		PatchSize, blur_radius,
		PatchBgThreshold, ProbVoteThreshold,
		BBoxWidth, BBoxHeight,
		HalfBBoxWidth, HalfBBoxHeight,
		iNumberOfScales, ResizeCoef);

	_line = config.typeOfLine;
	_left = config.leftPoint;
	_right = config.rightPoint;
}

void HT::detect()
{
	gd.Detect(_RGBFrame,
		koef, bg_bias, hyp_penalty,		
		MaxObjectsCount);
	

	int roi_width = int(_RGBFrame.size().width * koef);
	int roi_height = int(_RGBFrame.size().height * koef);
	
	double koef_width = _RGBFrame.size().width / double(roi_width);
	double koef_height = _RGBFrame.size().height / double(roi_height);
	double koef_x;//= RGBFrame.x / double(roi_x);
	double koef_y;// = RGBFrame.y / double(roi_y);

	for (unsigned int d = 0; d < gd.boxes.size(); d++)
	{
		gd.boxes[d].bbox.width *= koef_width;
		gd.boxes[d].bbox.height *= koef_height;
		gd.boxes[d].bbox.x = gd.boxes[d].bbox.x * koef_width + _left - BBoxWidth;
		gd.boxes[d].bbox.y *= koef_height;	
		if (isInArea(gd.boxes[d].bbox)) 
		{
			//rectangle(_RGBFrame, gd.boxes[d].bbox, Scalar(0, 255, 0), 2);
			boxes.push_back(Box(DETECTOR,gd.boxes[d].bbox));			
		}
	}
}

void HT::detect(cv::Rect rect)
{
	//Rect largeRect = enlargeRect(rect, 2.5, 2);

	
	Mat roi = _RGBFrame(rect);
	
	gd.Detect(roi,
		koef, bg_bias, hyp_penalty,
		MaxObjectsCount);

	int roi_width = int(rect.size().width * koef);
	int roi_height = int(rect.size().height * koef);
	int roi_x = int(rect.x * koef);
	int roi_y = int(rect.y * koef);
	double koef_width = rect.size().width / double(roi_width);
	double koef_height = rect.size().height / double(roi_height);
	double koef_x = rect.x / double(roi_x);
	double koef_y = rect.y / double(roi_y);
	
	

	for (int j = 0; j < gd.boxes.size(); j++)
	{
		gd.boxes[j].bbox.x = rect.x + gd.boxes[j].bbox.x * koef_x;
		gd.boxes[j].bbox.y = rect.y + gd.boxes[j].bbox.y * koef_y;
		gd.boxes[j].bbox.width *= koef_width;
		gd.boxes[j].bbox.height *= koef_height;
		if (isInArea(gd.boxes[j].bbox))
		{
			//rectangle(_RGBFrame, gd.boxes[d].bbox, Scalar(0, 255, 0), 2);
			boxes.push_back(Box(DETECTOR, gd.boxes[j].bbox));
		}
	}
	
}

bool HT::isInArea(Rect rect)
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

Rect HT::enlargeRect(Rect littleRect, int coefWidth, int coefHeight)
{
	Rect largeRect;
	// sirka
	largeRect.width = ((littleRect.x + littleRect.width * coefWidth) < _RGBFrame.size().width) ? littleRect.width * coefWidth : _RGBFrame.size().width - littleRect.x;

	// vyska
	largeRect.height = ((littleRect.y + littleRect.height * coefHeight) < _RGBFrame.size().height) ? littleRect.height * coefHeight : _RGBFrame.size().height - littleRect.y;

	largeRect.x = ((littleRect.x - ((largeRect.width - littleRect.width) / 2)) > 0) ? littleRect.x - ((largeRect.width - littleRect.width) / 2) : 0;
	largeRect.y = ((littleRect.y - ((largeRect.height - littleRect.height) / 2)) > 0) ? littleRect.y - ((largeRect.height - littleRect.height) / 2) : 0;

	return largeRect;
}

void HT::setRGBFrame(Mat frame)
{	
	//_RGBFrame = frame(Rect(_left - BBoxWidth, 0, _right + BBoxWidth - (_left - BBoxWidth), frame.size().height)).clone();
	_RGBFrame = frame;
	/*
	imshow("RGBroi", _RGBFrame);
	waitKey(0);
	*/
}

void HT::setBWFrame(Mat frame)
{
	_BWFrame = frame;
}