#include <opencv2\opencv.hpp>
#include "../Box.h"
#include "kcftracker.hpp"

using namespace cv;
using namespace std;



class RunKCF
{
public:
	RunKCF();
	~RunKCF();

	void track();
	void setFrames(Mat, int, int, int);
	void addBox(Box);
	void initTracking();
	void setCounter(struct Config);
	unsigned int getLeftCounter();
	unsigned int getRightCounter();
private:
	vector<KCFTracker> _person;
	static int _ID;
	Mat _RGBframe;
	int _actualFrameNO;
	int _countingType;
	int _leftPoint;
	int _middlePoint;
	int _rightPoint;
	vector<Box> _detBox;
	vector<Box> _trackBox;
	unsigned int _leftCounter;
	unsigned int _rightCounter;

	bool isInArea(KCFTracker);
	void counting(Box*, Box*);	
	bool compareBoxes(Box, Box, int);
	
};