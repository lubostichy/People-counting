/*
* Po��t�n� pr�chod� osob dve�mi s vyu�it�m stacion�rn� kamery
* Bakal�rska pr�ca, 2016
* �ubo� Tich�, xtichy23
* Box.h
*/

#include <opencv2/opencv.hpp>
#include <string>

#ifndef BOX
#define BOX

class Box
{
public:

	/* pr�znaky */
	const int DETECTOR = 1;
	const int TRACKER = 2;
	const int COUNTER = 3;
	
	/* kon�truktory bounding boxu */
	Box();	
	Box(int type, cv::Rect box);
	Box(int type, cv::Rect box, int);

	/* copy kon�truktor */
	Box(const Box& pBox);

	/* assignemt operator */
	void operator=(const Box& pBox);

	/* de�truktor bounding boxu */
	~Box();

	/* nastav� poradov� ��slo */
	void setFrameNO(int NO);

	/* typ bounding boxu */
	int type;

	/* poradov� ��slo bounding boxu */
	int frameNO;

	/* �trukt�ra boxu */
	cv::Rect bbox;

	/* vytla�� inform�cie o bounding boxe na obrazovku */
	std::string printBox();

};

#endif

