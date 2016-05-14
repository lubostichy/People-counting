/*
* Poèítání prùchodù osob dveømi s využitím stacionární kamery
* Bakalárska práca, 2016
* ¼uboš Tichý, xtichy23
* Detectors.cpp
*/

#include "Detectors.hpp"
#include "C4_detector\Pedestrian.h"

using namespace std;
using namespace cv;

/*
Konštruktor detektorov.
*/
Detectors::Detectors(
	int t_typeOfDetector, int t_typeOfLine,
	int t_leftPoint, int t_middlePoint, int t_rightPoint,
	int t_minWidthBox, int t_maxWidthBox,
	int t_minHeightBox, int t_maxHeightBox,
	string t_cascadeClassifier)
{


	if ((t_typeOfDetector == CASCADE) || (t_typeOfDetector == MOVEMENT_CASCADE))
	{
		loadCascadeClassifier(t_cascadeClassifier);
	}


	if ((t_typeOfDetector == C4) || (t_typeOfDetector == MOVEMENT_C4))
	{
		m_ds = DetectionScanner(108, 36, 9, 4, 256, 0.8);
		loadCascade(m_ds);
	}
	
	m_typeOfDetector = t_typeOfDetector;
	m_line = t_typeOfLine;
	m_left = t_leftPoint;
	m_middle = t_middlePoint;
	m_right = t_rightPoint;
	m_minWidth = t_minWidthBox;
	m_maxWidth = t_maxWidthBox;
	m_minHeight = t_minHeightBox;
	m_maxHeight = t_maxHeightBox;

	
}

/*
Deštruktor detektorov.
*/
Detectors::~Detectors()
{

}

/*
Naèíta kaskády pre HOG.
*/
void Detectors::loadCascadeClassifier(std::string t_cascadeClassifier)
{
	if (m_cascadeClassifier.empty())
	{
		if (!m_cascadeClassifier.load(t_cascadeClassifier))
		{
			cerr << "Can not load a cascade classifier." << '\n';
			exit(1);
		}
	}
}

/*
Naèíta kaskády pre metódu C4.
*/
void Detectors::loadCascade(DetectionScanner& ds)
{
	LoadCascade(ds);
	std::cout << "Detectors loaded." << std::endl;
	
	
}

/*
Rozhodne pod¾a zadanej metódy, ktorá metóda sledovania sa použije.
*/
void Detectors::detect(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero)
{
	setFrames(t_RGBFrame, t_BWFrame, t_numero);
	m_boxes.clear();
	
	
	if (m_typeOfDetector == MOVEMENT) detectMovement();
	if (m_typeOfDetector == MOVEMENT_CASCADE) detectMovementCascade();
	if (m_typeOfDetector == CASCADE) detectCascade();		
	if (m_typeOfDetector == C4) detectC4();
	if (m_typeOfDetector == MOVEMENT_C4) detectMovementC4();

}

/*
Metóda detekcie pohybom.
*/
void Detectors::detectMovement()
{
	vector<vector <Point>> contours; // kontury


	int erosionSize = 1;
	cv::Mat elementErosion = cv::getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));

	int dilationSize = 7;
	Mat elementDilation = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilationSize + 1, 2 * dilationSize + 1),
		Point(dilationSize, dilationSize));

	threshold(m_BWframe, m_BWframe, 250, 255, 0);

	erode(m_BWframe, m_BWframe, elementErosion);
	dilate(m_BWframe, m_BWframe, elementDilation);

		
	findContours(m_BWframe.clone(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<Point> approx;



	vector<Rect> boundRect(contours.size());
	vector<vector<Point> > contours_poly(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++)	
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));	
		
		if (!isGoodSize(boundRect[i]))
		{
			decreaseRect(&boundRect[i], 0.7, 0.7);
		}
		if (isInArea(boundRect[i]) && (isGoodSize(boundRect[i])))
		{
			rectangle(m_RGBframe, boundRect[i], Scalar(0, 255, 0), 2);
			m_boxes.push_back(Box(DETECTOR, boundRect[i], m_numero));
		}
		
	}
}

/*
HOG detekcia.
*/
void Detectors::detectCascade()
{
	vector <Rect> people;



	m_cascadeClassifier.detectMultiScale(m_RGBframe, people, 1.5, 3, 0); 
	
	for (unsigned int i = 0; i < people.size(); i++)	
	{
		if (isInArea(people[i]))
		{			
			decreaseRect(&people[i], 0.6, 0.9);
			if (isGoodSize(people[i]) && people[i].y < 150)
			{					
				rectangle(m_RGBframe, people[i], Scalar(0, 255, 0), 2); // green
				m_boxes.push_back(Box(DETECTOR, people[i], m_numero));
			}
		}
	}


}

/*
HOG s detekciou v mieste pohybu.
*/
void Detectors::detectMovementCascade()
{
	vector <Rect> people;

	vector <Rect> boundRects = getBoundRects();

	Rect largeRect, tmpPerson;
	Mat roi;

	for (vector<Rect>::iterator it = boundRects.begin(); it != boundRects.end(); it++)
	{
		
		roi = m_RGBframe(*it);

		m_cascadeClassifier.detectMultiScale(roi, people, 1.5, 3, 0); //, Size(_minSize, _minSize), Size((_RGBFrame).size().width, (_RGBFrame).size().height));

		
		for (vector<Rect>::iterator it2 = people.begin(); it2 != people.end(); it2++)
		{
			(*it2).x += (*it).x;
			(*it2).y += (*it).y;

			if (isInArea(*it2))
			{
				//rectangle(m_RGBframe, *it2, Scalar(255, 0, 0), 2); // blue
				decreaseRect(&(*it2), 0.6, 0.9);
				
				//rectangle(m_RGBframe, *it2, Scalar(255, 0, 0), 1); // blue
				if (isGoodSize(*it2))
				{
					rectangle(m_RGBframe, *it2, Scalar(0, 255, 0), 2); // green
					m_boxes.push_back(Box(DETECTOR, *it2, m_numero));
				}
			}

		}

		
	}
}

/*
Získa miesta pohybu.
*/
vector<Rect> Detectors::getBoundRects()
{
	vector<vector <Point>> contours;

	int erosionSize = 1;
	cv::Mat elementErosion = cv::getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
		cv::Point(erosionSize, erosionSize));

	int dilationSize = 7;
	Mat elementDilation = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * dilationSize + 1, 2 * dilationSize + 1),
		Point(dilationSize, dilationSize));

	threshold(m_BWframe, m_BWframe, 250, 255, 0);

	erode(m_BWframe, m_BWframe, elementErosion);
	dilate(m_BWframe, m_BWframe, elementDilation);

	findContours(m_BWframe, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Rect> goodRects;

	for (unsigned int i = 0; i < contours.size(); i++)	
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		
		if ((boundRect[i].width > m_minWidth) && (boundRect[i].height > m_minHeight))
		{
			enlargeRect(&boundRect[i], 2, 2);
			goodRects.push_back(boundRect[i]);
			//rectangle(m_RGBframe, boundRect[i], Scalar(0, 0, 255));
		}
	}

	return goodRects;
}


/*
Metóda detekcie C4.
*/
void Detectors::detectC4()
{
	 vector<CRect> results = DetectHuman(m_RGBframe, m_ds);
	 for (unsigned int i = 0; i < results.size(); i++) {
		 Rect tmpRect = Rect(
			 results[i].left, 
			 results[i].top, 
			 results[i].right - results[i].left, 
			 results[i].bottom - results[i].top);
		 //rectangle(m_RGBframe, tmpRect, Scalar(255, 0, 0), 2);
		 if (isInArea(tmpRect))  {			 
			 decreaseRect(&tmpRect, 0.7, 0.7);
			 if (isGoodSize(tmpRect)) {				
				 rectangle(m_RGBframe, tmpRect, Scalar(0, 255, 0), 2);
				 m_boxes.push_back(Box(DETECTOR, tmpRect, m_numero));
			 }
		 }

	 }
	
}

/*
Metóda detekcie C4 v mieste pohybu.
*/
void Detectors::detectMovementC4()
{
	vector <Rect> boundRects = getBoundRects();

	for (vector<Rect>::iterator it = boundRects.begin(); it != boundRects.end(); it++)
	{
		Mat roi = m_RGBframe(*it);
		vector<CRect> results = DetectHuman(roi, m_ds);
		for (unsigned int i = 0; i < results.size(); i++) {
			Rect tmpRect = Rect(
				results[i].left + (*it).x, 
				results[i].top + (*it).y, 
				results[i].right - results[i].left, 
				results[i].bottom - results[i].top);
			//rectangle(m_RGBframe, tmpRect, Scalar(255, 0, 0), 2);
			if (isInArea(tmpRect))  {
				decreaseRect(&tmpRect, 0.8, 0.8);
				if (isGoodSize(tmpRect)) {
					rectangle(m_RGBframe, tmpRect, Scalar(0, 0, 255), 2);
					m_boxes.push_back(Box(DETECTOR, tmpRect, m_numero));
				}
			}

		}
	}
}


/*
Získa bounding boxy detekcie.
*/
vector<Box> Detectors::getBoxes()
{
	return m_boxes;
}

/*
Testuje, èi sa bounding box nachádza v oblasti záujmu.
*/
bool Detectors::isInArea(Rect t_rect)
{
	if (m_line == VERTICAL)
	{
		return ((t_rect.x + t_rect.width / 2 >= m_left)
			&&
			(t_rect.x + t_rect.width / 2 <= m_right));
	}
		
	if (m_line == HORIZONTAL)
	{
		return ((t_rect.y + t_rect.height / 2 >= m_left)
			&&
			(t_rect.y + t_rect.height / 2 <= m_right));
	}
	return false;
}

/*
Testuje, èi má bounding box adekvátnu ve¾kos.
*/
bool Detectors::isGoodSize(Rect t_rect) // 20, 20, 40, 70
{
	return (m_minWidth < t_rect.width && t_rect.width < m_maxWidth)
		&&
		(m_minHeight < t_rect.height && t_rect.height < m_maxHeight);
}

/*
Nastaví snímky a aktuálne poradové èíslo.
*/
void Detectors::setFrames(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero)
{
	m_RGBframe = t_RGBFrame;
	m_BWframe = t_BWFrame;
	m_numero = t_numero;
}

/*
Zmenší ve¾kos bounding boxu o zadaný koeficient.
*/
void Detectors::decreaseRect(Rect *t_bigRect, float t_coefWidth, float t_coefHeight)
{
	Rect originalRect(*t_bigRect);
	//sirka
	t_bigRect->width = t_bigRect->width * t_coefWidth;

	// vyska
	t_bigRect->height = t_bigRect->height * t_coefHeight;

	t_bigRect->x = t_bigRect->x + (originalRect.width - t_bigRect->width) / 2;

	t_bigRect->y = t_bigRect->y + (originalRect.height - t_bigRect->height) / 2;

	
}

/*
Zväèší ve¾kos bounding boxu o zadaný koeficient.
*/
void Detectors::enlargeRect(Rect *t_littleRect, int t_coefWidth, int t_coefHeight)
{
	Rect originalRect(*t_littleRect);
	// sirka
	t_littleRect->width = ((t_littleRect->x + t_littleRect->width * t_coefWidth) < m_RGBframe.size().width) ? t_littleRect->width * t_coefWidth : m_RGBframe.size().width - t_littleRect->x;

	// vyska
	t_littleRect->height = ((t_littleRect->y + t_littleRect->height * t_coefHeight) < m_RGBframe.size().height) ? t_littleRect->height * t_coefHeight : m_RGBframe.size().height - t_littleRect->y;

	t_littleRect->x = ((t_littleRect->x - ((t_littleRect->width - originalRect.width) / 2)) > 0) ? t_littleRect->x - ((t_littleRect->width - originalRect.width) / 2) : 0;
	t_littleRect->y = ((t_littleRect->y - ((t_littleRect->height - originalRect.height) / 2)) > 0) ? t_littleRect->y - ((t_littleRect->height - originalRect.height) / 2) : 0;

	
}