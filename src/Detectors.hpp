/*
 * Po��t�n� pr�chod� osob dve�mi s vyu�it�m stacion�rn� kamery
 * Bakal�rska pr�ca, 2016
 * �ubo� Tich�, xtichy23
 * Detectors.hpp
 */

#ifndef DETECTORS
#define DETECTORS

#include <opencv2\opencv.hpp>
#include "Box.h"
#include "C4_detector\Pedestrian.h"

class Detectors
{
public:		
	/* Kon�truktor detektorov */
	Detectors(
		int t_typeOfDetector, int t_typeOfLine,
		int t_leftPoint, int t_middlePoint, int t_rightPoint,
		int t_minWidthBox, int t_maxWidthBox,
		int t_minHeightBox, int t_maxHeightBox,
		std::string t_cascadeClassifier);
	/* De�truktor detektorov. */
	~Detectors();

	/* Met�da ur�� pou�itie zvolenej detekcie. */
	void detect(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero);

	/* Z�ska bounding boxy detekcie. */
	std::vector<Box> getBoxes();
	
private:

	
	

	/* detekcia zalozena len pohybe */
	const int MOVEMENT = 1;

	/* detekcia zalozena na kaskade s vyuzitim pohybu */
	const int MOVEMENT_CASCADE = 2;

	/* detekcia s kaskadou */
	const int CASCADE = 3;

	/* detektor s vyuzitim Houghovej transformacie */
	const int HOUGH_TRANSFORM = 4;

	/* detektor s vyuzitim Houghovej transformacie a s vyuzitim pohybu */
	const int MOVEMENT_HOUGH_TRANSFORM = 5;

	/* detektor met�dy C4 */
	const int C4 = 6;

	/* detektor met�dy C4 v mieste pohybu  */
	const int MOVEMENT_C4 = 7;

	/* vertik�lna orient�cia oblasti z�ujmu */
	const int VERTICAL = 1;

	/* horizont�lna orient�cia oblasti z�ujmu */
	const int HORIZONTAL = 2;
	
	const int  DETECTOR = 1;
	const int  TRACKER = 2;
	const int  COUNTER = 3;

	/* bounding boxy detekcie */
	std::vector<Box> m_boxes;

	/* kask�dy */
	cv::CascadeClassifier m_cascadeClassifier;

	/* �iernobiela sn�mka */
	cv::Mat m_BWframe;

	/* farebn� sn�mka */
	cv::Mat m_RGBframe;

	/* poradov� ��slo sn�mky */
	int m_numero;

	/* typ detekcie */
	int m_typeOfDetector;

	/* �iary oblasti z�umu */
	int m_line, m_left, m_middle, m_right;

	/* minim�lna a maxim�lna ��rka osoby */
	int m_minWidth, m_maxWidth;

	/* minim�lna a maxim�lna v��ka osoby */
	int m_minHeight, m_maxHeight;

	/* Met�da detekcie pohybu. */
	void detectMovement();

	/* HOG detekcia v mieste pohybu. */
	void detectMovementCascade();

	/* HOG detekcia. */
	void detectCascade();

	/* Detektor C4 */
	DetectionScanner m_ds;

	/* MMet�da detekcie C4. */
	void detectC4();

	/* Met�da detekcie C4 v mieste pohybu. */
	void detectMovementC4();

	/* Nastav� sn�mky a aktu�lne poradov� ��slo sn�mky. */
	void setFrames(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero);
	
	/* Na��ta kask�dy pre HOG. */	
	void loadCascadeClassifier(std::string t_cascadeClassifier);

	/* Na��ta kask�dy pre met�du C4. */
	void loadCascade(DetectionScanner& ds);
	
	/*  Z�ska miesta pohybu. */
	std::vector<cv::Rect> getBoundRects();
	
	/* Testuje, �i sa bounding box nach�dza v oblasti z�ujmu. */
	bool isInArea(cv::Rect);

	/* Testuje, �i m� bounding box detekcie adekv�tnu ve�kos�. */
	bool isGoodSize(cv::Rect);

	/* Zmen�� ve�kos� bounding boxu o dan� koeficient. */
	void decreaseRect(cv::Rect *t_bigRect, float t_coefWidth, float t_coefHeight);

	/* Zv��� ve�kos� bounding boxu o dan� koeficient. */
	void enlargeRect(cv::Rect *t_littleRect, int t_coefWidth, int t_coefHeight);
	
};


#endif