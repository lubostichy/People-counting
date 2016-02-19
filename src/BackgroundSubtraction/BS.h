#include "opencv2\opencv.hpp"
#include "../Box.h"

using namespace cv;

class BS
{
public:
	BS();
	~BS();	

	std::vector <Box> boxes;
	

	void setBS(int, int);
	void setFrames(cv::Mat, cv::Mat);
	void detect();
	void count();
private:
	Mat BWframe;
	Mat RGBframe;
	int averageWidth;
	int averageHeight;
	int range;
	CascadeClassifier cascadeClassifier;
	
	bool isGoodSize(Rect);

};