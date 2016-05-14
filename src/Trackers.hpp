/*
* Poèítání prùchodù osob dveømi s využitím stacionární kamery
* Bakalárska práca, 2016
* ¼uboš Tichý, xtichy23
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
	/* Konštruktor sledovaèa. */
	Trackers(
		int t_typeOfTracker, int t_typeOfLine,
		int t_leftPoint, int t_middlePoint, int t_rightPoint);
		
	/* Deštruktor sledovaèa. */
	~Trackers();
	
	/* Rozhodne pod¾a zadanej metódy, ktorá metóda sledovania sa použije. */
	void track(vector<Box> t_detectedBoxes, cv::Mat t_RGBframe, int t_numero);

	/* Získa hodnotu poèítania na ¾avej/spodnej strane. */
	int getLeftCounter();

	/* Získa hodnotu poèítania na pravej/vrchnej strane. */
	int getRightCounter();

	/* výstupný súbor */
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

	/* vertikálna orientácia oblasti záujmu */
	const int VERTICAL = 1;

	/* horizontálna orientácia oblasti záujmu */
	const int HORIZONTAL = 2;

	const int  DETECTOR = 1;
	const int  TRACKER = 2;
	const int  COUNTER = 3;

	/* farebná snímka */
	cv::Mat m_RGBframe;

	/* èiernobiela snímka */
	cv::Mat m_BWframe;

	/* poradové èíslo snímky */
	int m_numero;

	/* bounding boxy detekcie */
	std::vector<Box> m_detBox;

	/* bounding boxy sledovania */
	std::vector<Box> m_trackBox;

	/* typ sledovania */
	int m_typeOfTracker;

	/* èiary oblasti záujmu */
	int m_line, m_leftPoint, m_middlePoint, m_rightPoint;

	/* hodnoty poèítania */
	int m_leftCounter, m_rightCounter;

	/* ID osoby */
	static int m_ID;

	/* sledovaè TLD */
	std::vector <tld::TLD*> m_TLDperson;

	/* sledovanie korelaènými filtrami */
	std::vector <KCFTracker*> m_KCFperson;

	/* kompresívne sledovanie */
	std::vector <CompressiveTracker*> m_CTperson;

	/* metóda kompresívneho sledovania  */
	void trackCT();

	/* filtrácia osôb kompresívneho sledovania */
	void updatePersonsCT();

	/* metóda sledovania TLD */
	void trackTLD();

	/* filtrácia osôb sledovania TLD */
	void updatePersonsTLD();

	/* metóda sledovania korelaènými filtrami */
	void trackKCF();

	/* filtrácia osôb sledovania korelaènými filtrami */
	void updatePersonsKCF();

	/* nastaví snímky a aktuálne poradové èíslo snímky */
	void setFrames(cv::Mat t_RGBframe, int t_numero);

	/* pridá box pod¾a typu medzi ïalšie boxy */
	void addBox(Box box);

	/* testuje, èi sa bounding box nachádza v oblasti záujmu*/
	bool isInArea(Box *t_lastBox);

	/* zapoèítanie osoby a urèenie smeru */
	void counting(Box *t_first, Box *t_last);

	/* porovnávanie pozícií bounding boxov */
	bool compareBoxes(Box dBox, Box tBox, int treshold);
	
};



#endif