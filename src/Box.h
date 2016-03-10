#include <opencv2/opencv.hpp>
#include <string>

#ifndef BOX
#define BOX

class Box
{
public:

	const int DETECTOR = 1;
	const int TRACKER = 2;
	const int COUNTER = 3;
	
	Box();
	Box(int type, cv::Rect box);
	Box(int type, cv::Rect box, int);
	Box(const Box& pBox);
	void operator=(const Box& pBox);
	~Box();
	void setFrameNO(int NO);
	int type;
	int frameNO;
	cv::Rect bbox;
	std::string printBox();

};

#endif

