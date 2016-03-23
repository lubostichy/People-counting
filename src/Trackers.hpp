#ifndef TRACKERS
#define TRACKERS

#include <opencv2\opencv.hpp>
#include <fstream>
#include "Box.h"
#include "OpenTLD\tld\TLD.h"
#include "Kernelized_Correlation_Filters\kcftracker.hpp"
#include "CompressiveTracking\CompressiveTracker.h"

class Trackers
{
public:
	Trackers(
		int t_typeOfTracker, int t_typeOfLine,
		int t_leftPoint, int t_middlePoint, int t_rightPoint);
		//std::string filename, int count, int com);
	~Trackers();
	void track(vector<Box> t_detectedBoxes, cv::Mat t_RGBframe, int t_numero);
	int getLeftCounter();
	int getRightCounter();

	std::string result; //= "test.txt";
	int countingThresh, compareThresh;
	std::ofstream fs;
private:

	/* TLD tracker */
	const int TLD = 1;

	/* Compressive tracking */
	const int CT = 2;

	/* Kernelized correlation filters */
	const int KCF = 3;

	const int VERTICAL = 1;
	const int HORIZONTAL = 2;
	const int  DETECTOR = 1;
	const int  TRACKER = 2;
	const int  COUNTER = 3;

	cv::Mat m_RGBframe;
	cv::Mat m_BWframe;
	int m_numero;
	std::vector<Box> m_detBox;
	std::vector<Box> m_trackBox;
	int m_typeOfTracker;
	int m_line, m_leftPoint, m_middlePoint, m_rightPoint;
	int m_leftCounter, m_rightCounter;
	static int m_ID;
	std::vector <tld::TLD*> m_TLDperson;
	std::vector <KCFTracker*> m_KCFperson;
	std::vector <CompressiveTracker*> m_CTperson;

	void trackCT();
	void updatePersonsCT();
	void trackTLD();
	void updatePersonsTLD();
	void trackKCF();
	void updatePersonsKCF();
	void setFrames(cv::Mat t_RGBframe, int t_numero);
	void addBox(Box box);
	bool isInArea(Box *t_lastBox);
	void counting(Box *t_first, Box *t_last);
	bool compareBoxes(Box dBox, Box tBox, int treshold);
	
};



#endif