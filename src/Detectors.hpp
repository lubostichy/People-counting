/*
 * Poèítání prùchodù osob dveømi s vyuitím stacionární kamery
 * Bakalárska práca, 2016
 * ¼uboš Tichı, xtichy23
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
	/* Konštruktor detektorov */
	Detectors(
		int t_typeOfDetector, int t_typeOfLine,
		int t_leftPoint, int t_middlePoint, int t_rightPoint,
		int t_minWidthBox, int t_maxWidthBox,
		int t_minHeightBox, int t_maxHeightBox,
		std::string t_cascadeClassifier);
	/* Deštruktor detektorov. */
	~Detectors();

	/* Metóda urèí pouitie zvolenej detekcie. */
	void detect(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero);

	/* Získa bounding boxy detekcie. */
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

	/* detektor metódy C4 */
	const int C4 = 6;

	/* detektor metódy C4 v mieste pohybu  */
	const int MOVEMENT_C4 = 7;

	/* vertikálna orientácia oblasti záujmu */
	const int VERTICAL = 1;

	/* horizontálna orientácia oblasti záujmu */
	const int HORIZONTAL = 2;
	
	const int  DETECTOR = 1;
	const int  TRACKER = 2;
	const int  COUNTER = 3;

	/* bounding boxy detekcie */
	std::vector<Box> m_boxes;

	/* kaskády */
	cv::CascadeClassifier m_cascadeClassifier;

	/* èiernobiela snímka */
	cv::Mat m_BWframe;

	/* farebná snímka */
	cv::Mat m_RGBframe;

	/* poradové èíslo snímky */
	int m_numero;

	/* typ detekcie */
	int m_typeOfDetector;

	/* èiary oblasti záumu */
	int m_line, m_left, m_middle, m_right;

	/* minimálna a maximálna šírka osoby */
	int m_minWidth, m_maxWidth;

	/* minimálna a maximálna vıška osoby */
	int m_minHeight, m_maxHeight;

	/* Metóda detekcie pohybu. */
	void detectMovement();

	/* HOG detekcia v mieste pohybu. */
	void detectMovementCascade();

	/* HOG detekcia. */
	void detectCascade();

	/* Detektor C4 */
	DetectionScanner m_ds;

	/* MMetóda detekcie C4. */
	void detectC4();

	/* Metóda detekcie C4 v mieste pohybu. */
	void detectMovementC4();

	/* Nastaví snímky a aktuálne poradové èíslo snímky. */
	void setFrames(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero);
	
	/* Naèíta kaskády pre HOG. */	
	void loadCascadeClassifier(std::string t_cascadeClassifier);

	/* Naèíta kaskády pre metódu C4. */
	void loadCascade(DetectionScanner& ds);
	
	/*  Získa miesta pohybu. */
	std::vector<cv::Rect> getBoundRects();
	
	/* Testuje, èi sa bounding box nachádza v oblasti záujmu. */
	bool isInArea(cv::Rect);

	/* Testuje, èi má bounding box detekcie adekvátnu ve¾kos. */
	bool isGoodSize(cv::Rect);

	/* Zmenší ve¾kos bounding boxu o danı koeficient. */
	void decreaseRect(cv::Rect *t_bigRect, float t_coefWidth, float t_coefHeight);

	/* Zväèší ve¾kos bounding boxu o danı koeficient. */
	void enlargeRect(cv::Rect *t_littleRect, int t_coefWidth, int t_coefHeight);
	
};


#endif