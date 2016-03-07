#pragma once
#include "DenseGreedyDetection\GreedyDetection.h"
#include <opencv2\opencv.hpp>
#include "Forests\HoughForest.h"
#include "..\Box.h"

using namespace cv;
using namespace std;

class HT
{
public:
	HT();
	~HT();

	/* size of patch in a forest */
	int PatchSize;
	/* only patches with probabilities to belong to background lower than this threshold can vote */
	double PatchBgThreshold;
	/* minimal allowed probability of a patch to belong to object, if probability is lesser the vote is ignored */
	double ProbVoteThreshold;
	/* width of a bounding box at each scale corresponding to detection */
	int BBoxWidth;
	/* height of a bounding box at each scale corresponding to detection */
	int BBoxHeight;
	/* number of scales for multi - scale detection */
	int iNumberOfScales;
	/* half of maximum width of object in the data used for training the forest */
	float HalfBBoxWidth;
	/* half of maximum height of object in the data used for training the forest */
	float HalfBBoxHeight;
	/* radius of blur for hough images */
	float blur_radius;
	/* size of images at subsequent scales differ by coefficient */
	float ResizeCoef;
	/* bias of background cost, parameter of detection algorithm */
	float bg_bias;
	/* penalty for adding a hypothesis, parameter of detection algorithm */
	float hyp_penalty;
	/* maximum number of objects in an image */
	float MaxObjectsCount;
	/* resize images before detection by this coefficient */
	double koef;

	string forest_path;

	

	CGreedyDetection gd;
	vector <Box> boxes;
	

	void setDetector(int, int, int, int, int);

	void detect();
	void detect(Rect rect);

	

	void setRGBFrame(Mat frame);
	void setBWFrame(Mat frame);

private:
	int _left;
	int _right;
	int _line;
	Mat _RGBFrame;
	Mat _BWFrame;

	Rect enlargeRect(Rect littleRect, int coefWidth, int coefHeight);
	bool isInArea(Rect);
};

