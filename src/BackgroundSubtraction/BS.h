#include "opencv2\opencv.hpp"
#include "../Box.h"

using namespace cv;

class BS
{
public:
	BS();
	~BS();	

	std::vector <Box> boxes;
	vector<Rect> boundingBoxes;

	void setBS(struct Config);
	void setFrames(cv::Mat, cv::Mat);
	void detect();
	void findBoundingBoxes();
private:
	Mat _BWframe;
	Mat _RGBframe;
	int _line, _left, _middle, _right;
	int _minWidth, _maxWidth;
	int _minHeight, _maxHeight;
	
	Rect enlargeRect(Rect littleRect, int coefWidth, int coefHeight);
	bool isGoodSize(Rect);

};