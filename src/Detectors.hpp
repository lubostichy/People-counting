#ifndef DETECTORS
#define DETECTORS

#include <opencv2\opencv.hpp>
//#include "Box.h"
#include "HoughTransform\DenseGreedyDetection\GreedyDetection.h"
#include "C4_detector\Pedestrian.h"

class Detectors
{
public:		
	Detectors(
		int t_typeOfDetector, int t_typeOfLine,
		int t_leftPoint, int t_middlePoint, int t_rightPoint,
		int t_minWidthBox, int t_maxWidthBox,
		int t_minHeightBox, int t_maxHeightBox,
		std::string t_cascadeClassifier);
	~Detectors();
	void detect(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero);
	std::vector<Box> getBoxes();
	
private:

	/* size of patch in a forest */
	int PatchSize = 16;
	/* only patches with probabilities to belong to background lower than this threshold can vote */
	double PatchBgThreshold = 0.7;
	/* minimal allowed probability of a patch to belong to object, if probability is lesser the vote is ignored */
	double ProbVoteThreshold = 0.00005;
	/* width of a bounding box at each scale corresponding to detection */
	int BBoxWidth;
	/* height of a bounding box at each scale corresponding to detection */
	int BBoxHeight;
	/* number of scales for multi - scale detection */
	int iNumberOfScales = 3;
	/* half of maximum width of object in the data used for training the forest */
	float HalfBBoxWidth;
	/* half of maximum height of object in the data used for training the forest */
	float HalfBBoxHeight;
	/* radius of blur for hough images */
	float blur_radius = 5;
	/* size of images at subsequent scales differ by coefficient */
	float ResizeCoef = 0.85;
	/* bias of background cost, parameter of detection algorithm */
	float bg_bias = 8.5;
	/* penalty for adding a hypothesis, parameter of detection algorithm */
	float hyp_penalty = 1500;
	/* maximum number of objects in an image */
	float MaxObjectsCount = 3;
	/* resize images before detection by this coefficient */
	double koef = 0.647;

	std::string forest_path = "pedestrian.dat";

	CGreedyDetection gd;

	DetectionScanner m_ds;

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

	const int C4 = 6;
	const int MOVEMENT_C4 = 7;

	const int VERTICAL = 1;
	const int HORIZONTAL = 2;

	const int  DETECTOR = 1;
	const int  TRACKER = 2;
	const int  COUNTER = 3;

	std::vector<Box> m_boxes;
	cv::CascadeClassifier m_cascadeClassifier;
	cv::Mat m_BWframe;
	cv::Mat m_RGBframe;
	int m_numero;
	int m_typeOfDetector;
	int m_line, m_left, m_middle, m_right;
	int m_minWidth, m_maxWidth;
	int m_minHeight, m_maxHeight;

	void detectMovement();
	void detectMovementCascade();
	void detectCascade();
	//void detectHT();
	//void detectMovementHT();
	void detectC4();
	void detectMovementC4();
	void setFrames(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero);
	void loadCascadeClassifier(std::string t_cascadeClassifier);
	void loadCascade(DetectionScanner& ds);
	
	std::vector<cv::Rect> getBoundRects();
	bool isInArea(cv::Rect);
	bool isGoodSize(cv::Rect);
	void decreaseRect(cv::Rect *t_bigRect, float t_coefWidth, float t_coefHeight);
	void enlargeRect(cv::Rect *t_littleRect, int t_coefWidth, int t_coefHeight);
	
	cv::Rect m_area;
	void setArea();
};


#endif