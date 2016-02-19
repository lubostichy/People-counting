#include "HT.h"
#include "DenseGreedyDetection\GreedyDetection.h"
#include <opencv2\opencv.hpp>
#include "Forests\HoughForest.h"

using namespace cv;

HT::HT()
{
	

	PatchSize = 16;

	blur_radius = 5;

	PatchBgThreshold = 0.7;

	ProbVoteThreshold = 0.00005;

	BBoxWidth = 40; //40

	BBoxHeight = 140; // 80

	HalfBBoxWidth = 30; // 30

	HalfBBoxHeight = 60;  // 60

	iNumberOfScales = 3;
	
	ResizeCoef = 0.85;

	bg_bias = 8.5;

	hyp_penalty = 1500;

	MaxObjectsCount = 8;

	koef = 0.647;
	//koef = 1;

	forest_path = "pedestrian.dat";
}


HT::~HT()
{
}

void HT::setDetector()
{
	gd.SetForest(forest_path.c_str(),
		PatchSize, blur_radius,
		PatchBgThreshold, ProbVoteThreshold,
		BBoxWidth, BBoxHeight,
		HalfBBoxWidth, HalfBBoxHeight,
		iNumberOfScales, ResizeCoef);
}

void HT::detect()
{
	gd.Detect(RGBFrame,
		koef, bg_bias, hyp_penalty,		
		MaxObjectsCount);
	this->boxes = gd.boxes;
}

void HT::detect(cv::Rect rect)
{
	Rect largeRect = this->enlargeRect(rect, 2.5, 2);

	// zeleny 
	//rectangle(this->RGBFrame, largeRect, Scalar(0, 255, 0), 2);
	
	Mat roi = (this->RGBFrame)(largeRect);
	
	
	int roi_width = int(largeRect.size().width * this->koef);
	int roi_height = int(largeRect.size().height * this->koef);
	int roi_x = int(largeRect.x * this->koef);
	int roi_y = int(largeRect.y * this->koef);
	double koef_width = largeRect.size().width / double(roi_width);
	double koef_height = largeRect.size().height / double(roi_height);
	double koef_x = largeRect.x / double(roi_x);
	double koef_y = largeRect.y / double(roi_y);
	
	
	gd.Detect(roi,
		koef, bg_bias, hyp_penalty,
		MaxObjectsCount);


	for (int j = 0; j < gd.boxes.size(); j++)
	{
		gd.boxes[j].bbox.x = largeRect.x + gd.boxes[j].bbox.x * koef_x;
		gd.boxes[j].bbox.y = largeRect.y + gd.boxes[j].bbox.y * koef_y;
		gd.boxes[j].bbox.width = gd.boxes[j].bbox.width * koef_width;
		gd.boxes[j].bbox.height = gd.boxes[j].bbox.height * koef_height;
		
	}
	this->boxes = gd.boxes;
}

Rect HT::enlargeRect(Rect littleRect, int coefWidth, int coefHeight)
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

void HT::setRGBFrame(Mat frame)
{
	this->RGBFrame = frame;
}

void HT::setBWFrame(Mat frame)
{
	this->BWFrame = frame;
}