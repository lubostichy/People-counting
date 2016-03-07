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

	void setBS(int, int);
	void setFrames(cv::Mat, cv::Mat);
	void detect();
	void findBoundingBoxes();
private:
	Mat _BWframe;
	Mat _RGBframe;
	int _averageWidth;
	int _averageHeight;
	//int range;
	
	Rect enlargeRect(Rect littleRect, int coefWidth, int coefHeight);
	bool isGoodSize(Rect);

};