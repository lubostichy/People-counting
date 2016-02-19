#pragma once

#include "tld/TLD.h"
//#include "imacq/ImAcq.h"
#include "../Box.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class OpenTLD
{
	
public:
	
	OpenTLD();
	~OpenTLD();

	
	bool moreFrames;
	IplImage *img;

	
	bool showOutput;
	bool showTrajectory;
	int trajectoryLength;
	const char *printResults;
	const char *saveDir;
	double threshold;
	bool showForeground;
	bool showNotConfident;
	bool selectManually;
	int *initialBB;
	bool reinit;
	bool exportModelAfterRun;
	bool loadModel;
	const char *modelPath;
	const char *modelExportFile;
	int seed;
	tld::TLD *tld;

	void setDevice(std::string);
	void setPath(std::string);
	void setStartFrame(int);
	void setBBox(vector<int>);
	void track();
	void setFrames(Mat, int, int, int);	
	void addBox(Box box);
	void initTracking();	
	//bool findBox(tld::TLD person, Box detBox);
	void setCounter(int, int, int, int);
	int getLeftCounter();
	int getRightCounter();
private:	
	static int _ID;
	cv::Mat _frame;
	string device;
	string path;
	int startFrame;
	vector<int> bbox;
	int _countingType;
	int _leftPoint;
	int _middlePoint;
	int _rightPoint;
	int _actualFrameNO;
	unsigned int _leftCounter;
	unsigned int _rightCounter;
	vector <tld::TLD> _person;
	vector<Box> _detBox;
	vector<Box> _trackBox;
	
	tld::TLD* newPerson(Box);
	bool isInArea(Box*);
	void counting(Box*, Box*);
	bool compareBoxes(Box dbox, Box tbox, int);	
};

