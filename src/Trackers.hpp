/*
* Po��t�n� pr�chod� osob dve�mi s vyu�it�m stacion�rn� kamery
* Bakal�rska pr�ca, 2016
* �ubo� Tich�, xtichy23
* Trackers.hpp
*/

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
	/* Kon�truktor sledova�a. */
	Trackers(
		int t_typeOfTracker, int t_typeOfLine,
		int t_leftPoint, int t_middlePoint, int t_rightPoint);
		
	/* De�truktor sledova�a. */
	~Trackers();
	
	/* Rozhodne pod�a zadanej met�dy, ktor� met�da sledovania sa pou�ije. */
	void track(vector<Box> t_detectedBoxes, cv::Mat t_RGBframe, int t_numero);

	/* Z�ska hodnotu po��tania na �avej/spodnej strane. */
	int getLeftCounter();

	/* Z�ska hodnotu po��tania na pravej/vrchnej strane. */
	int getRightCounter();

	/* v�stupn� s�bor */
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

	/* vertik�lna orient�cia oblasti z�ujmu */
	const int VERTICAL = 1;

	/* horizont�lna orient�cia oblasti z�ujmu */
	const int HORIZONTAL = 2;

	const int  DETECTOR = 1;
	const int  TRACKER = 2;
	const int  COUNTER = 3;

	/* farebn� sn�mka */
	cv::Mat m_RGBframe;

	/* �iernobiela sn�mka */
	cv::Mat m_BWframe;

	/* poradov� ��slo sn�mky */
	int m_numero;

	/* bounding boxy detekcie */
	std::vector<Box> m_detBox;

	/* bounding boxy sledovania */
	std::vector<Box> m_trackBox;

	/* typ sledovania */
	int m_typeOfTracker;

	/* �iary oblasti z�ujmu */
	int m_line, m_leftPoint, m_middlePoint, m_rightPoint;

	/* hodnoty po��tania */
	int m_leftCounter, m_rightCounter;

	/* ID osoby */
	static int m_ID;

	/* sledova� TLD */
	std::vector <tld::TLD*> m_TLDperson;

	/* sledovanie korela�n�mi filtrami */
	std::vector <KCFTracker*> m_KCFperson;

	/* kompres�vne sledovanie */
	std::vector <CompressiveTracker*> m_CTperson;

	/* met�da kompres�vneho sledovania  */
	void trackCT();

	/* filtr�cia os�b kompres�vneho sledovania */
	void updatePersonsCT();

	/* met�da sledovania TLD */
	void trackTLD();

	/* filtr�cia os�b sledovania TLD */
	void updatePersonsTLD();

	/* met�da sledovania korela�n�mi filtrami */
	void trackKCF();

	/* filtr�cia os�b sledovania korela�n�mi filtrami */
	void updatePersonsKCF();

	/* nastav� sn�mky a aktu�lne poradov� ��slo sn�mky */
	void setFrames(cv::Mat t_RGBframe, int t_numero);

	/* prid� box pod�a typu medzi �al�ie boxy */
	void addBox(Box box);

	/* testuje, �i sa bounding box nach�dza v oblasti z�ujmu*/
	bool isInArea(Box *t_lastBox);

	/* zapo��tanie osoby a ur�enie smeru */
	void counting(Box *t_first, Box *t_last);

	/* porovn�vanie poz�ci� bounding boxov */
	bool compareBoxes(Box dBox, Box tBox, int treshold);
	
};



#endif