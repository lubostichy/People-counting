#include "../Box.h"
#include "opencv2\opencv.hpp"
#include "CompressiveTracker.h"

class RunCT
{
public:
	
	RunCT();
	~RunCT();


	void addBox(Box);
	void track();
	void initTracking();
	void setFrames(cv::Mat, int, int, int);
	void setCounter(int, int, int, int);
	unsigned int getLeftCounter();	
	unsigned int getRightCounter();
private:

	static int _ID;
	std::vector<Box> _detBox;
	std::vector<Box> _trackBox;
	Mat _frame;
	Mat _BWframe;
	int _actualFrameNO;
	unsigned int _leftCounter;
	unsigned int _rightCounter;
	int _countingType;
	int _leftPoint;
	int _middlePoint;
	int _rightPoint;
	std::vector <CompressiveTracker> _person;

	bool isInArea(CompressiveTracker);
	void counting(Box*, Box*);
	bool compareBoxes(Box, Box, int);
	CompressiveTracker * newPerson(Box);
};

