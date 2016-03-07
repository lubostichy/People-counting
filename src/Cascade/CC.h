#include "opencv2\opencv.hpp"

#include "../Box.h"

using namespace cv;

class CC
{
public:
	CC();
	~CC();
	std::vector <Box> boxes;

	void setCC(struct Config);
	void detect();
	void setFrame(Mat);

private:
	CascadeClassifier _cascadeClassifier;
	Mat _RGBframe;
	Mat _BWframe;
	int _line, _left, _middle, _right;
	int _minWidth, _maxWidth;
	int _minHeight, _maxHeight;
	//int _averageWidth, _averageHeight;
	int _range;

	Rect decreaseRect(Rect, float, float);
	bool isInArea(Rect);
	bool isGoodSize(Rect);
};