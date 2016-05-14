/*
* Poèítání prùchodù osob dveømi s využitím stacionární kamery
* Bakalárska práca, 2016
* ¼uboš Tichý, xtichy23
* Box.h
*/

#include <opencv2/opencv.hpp>
#include <string>

#ifndef BOX
#define BOX

class Box
{
public:

	/* príznaky */
	const int DETECTOR = 1;
	const int TRACKER = 2;
	const int COUNTER = 3;
	
	/* konštruktory bounding boxu */
	Box();	
	Box(int type, cv::Rect box);
	Box(int type, cv::Rect box, int);

	/* copy konštruktor */
	Box(const Box& pBox);

	/* assignemt operator */
	void operator=(const Box& pBox);

	/* deštruktor bounding boxu */
	~Box();

	/* nastaví poradové èíslo */
	void setFrameNO(int NO);

	/* typ bounding boxu */
	int type;

	/* poradové èíslo bounding boxu */
	int frameNO;

	/* štruktúra boxu */
	cv::Rect bbox;

	/* vytlaèí informácie o bounding boxe na obrazovku */
	std::string printBox();

};

#endif

