#include "Detectors.hpp"
#include "C4_detector\Pedestrian.h"

using namespace std;
using namespace cv;


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

	if ((t_typeOfDetector == MOVEMENT_HOUGH_TRANSFORM) || (t_typeOfDetector == HOUGH_TRANSFORM))
	{
		// TODO !!!!
		BBoxWidth = t_minWidthBox;
		BBoxHeight = t_minHeightBox;
		HalfBBoxWidth =  (t_minWidthBox + t_maxWidthBox) / 2;
		HalfBBoxHeight =  (t_minHeightBox + t_maxHeightBox) / 2;

		gd.SetForest(forest_path.c_str(),
			PatchSize, blur_radius,
			PatchBgThreshold, ProbVoteThreshold,
			BBoxWidth, BBoxHeight,
			HalfBBoxWidth, HalfBBoxHeight,
			iNumberOfScales, ResizeCoef);
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

Detectors::~Detectors()
{

}

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


void Detectors::loadCascade(DetectionScanner& ds)
{
	LoadCascade(ds);
	std::cout << "Detectors loaded." << std::endl;
	
	
}



void Detectors::setArea() {
	
	int x, y, width, height, body;
	
	x = m_left - m_maxWidth < 0 ? 0 : m_left - m_maxWidth;
	width = m_right + m_maxWidth > m_RGBframe.size().width ? m_RGBframe.size().width : m_right + m_maxWidth;
	y = 0;
	height = m_RGBframe.size().height;

	m_area = Rect(x, y, width, height);
}

void Detectors::detect(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero)
{
	setFrames(t_RGBFrame, t_BWFrame, t_numero);
	m_boxes.clear();
	setArea();
	
	if (m_typeOfDetector == MOVEMENT) detectMovement();
	if (m_typeOfDetector == MOVEMENT_CASCADE) detectMovementCascade();
	if (m_typeOfDetector == CASCADE) detectCascade();	
	//if (m_typeOfDetector == HOUGH_TRANSFORM) detectHT();
	//if (m_typeOfDetector == MOVEMENT_HOUGH_TRANSFORM) detectMovementHT();
	if (m_typeOfDetector == C4) detectC4();
	if (m_typeOfDetector == MOVEMENT_C4) detectMovementC4();

}

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
	//for (vector<vector <Point>>::iterator it = contours.begin(); it != contours.end(); i++t)
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

void Detectors::detectCascade()
{
	vector <Rect> people;



	// HAAR	
	/*
	cvtColor(_RGBframe, _BWframe, CV_BGR2GRAY);
	equalizeHist(_BWframe, _BWframe);
	_cascadeClassifier.detectMultiScale(_BWframe, people, 1.3, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(300, 300)); // HAAR

	for (unsigned int i = 0; i < people.size(); i++)
	{
	people[i] = decreaseRect(people[i], 0.5, 0.5);
	rectangle(_RGBframe, people[i], Scalar(0,255,0), 2);
	boxes.push_back(Box(DETECTOR, people[i]));
	}
	*/

	// HOG
	m_cascadeClassifier.detectMultiScale(m_RGBframe, people, 1.5, 3, 0); // HOG
	//Rect tmpPerson;
	for (unsigned int i = 0; i < people.size(); i++)
	//for (vector<Rect>::iterator it = people.begin(); it != people.end(); it++)
	{
		if (isInArea(people[i]))
		{
			//rectangle(_RGBframe, people[i], Scalar(255, 0, 0), 2); // blue
			decreaseRect(&people[i], 0.6, 0.9);
			//rectangle(_RGBframe, people[i], Scalar(0, 255, 0), 2); // green
			//cout << people[i].width << " / " << people[i].height << endl;
			if (isGoodSize(people[i]) && people[i].y < 150)
			{					
				rectangle(m_RGBframe, people[i], Scalar(0, 255, 0), 2); // green
				m_boxes.push_back(Box(DETECTOR, people[i], m_numero));
			}
		}
	}


	//people.clear();
}

void Detectors::detectMovementCascade()
{
	vector <Rect> people;

	vector <Rect> boundRects = getBoundRects();

	Rect largeRect, tmpPerson;
	Mat roi;

	for (vector<Rect>::iterator it = boundRects.begin(); it != boundRects.end(); it++)
	{
		//largeRect = enlargeRect(*it, 2.5, 2);
		//if (((largeRect).width > m_minWidth) && ((largeRect).height > m_minHeight))
		//{
		roi = m_RGBframe(*it);

		m_cascadeClassifier.detectMultiScale(roi, people, 1.5, 3, 0); //, Size(_minSize, _minSize), Size((_RGBFrame).size().width, (_RGBFrame).size().height));

		//for (unsigned int j = 0; j < people.size(); j++)
		for (vector<Rect>::iterator it2 = people.begin(); it2 != people.end(); it2++)
		{
			(*it2).x += (*it).x;
			(*it2).y += (*it).y;

			if (isInArea(*it2))
			{
				rectangle(m_RGBframe, *it2, Scalar(255, 0, 0), 2); // blue
				decreaseRect(&(*it2), 0.6, 0.9);
				
				rectangle(m_RGBframe, *it2, Scalar(255, 0, 0), 1); // blue
				if (isGoodSize(*it2))
				{
					rectangle(m_RGBframe, *it2, Scalar(0, 255, 0), 2); // green
					m_boxes.push_back(Box(DETECTOR, *it2, m_numero));
				}
			}

		}

		//}
	}
}

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
void Detectors::detectHT()
{

	Mat area = m_RGBframe(m_area);

	gd.Detect(area,
		koef, bg_bias, hyp_penalty,
		MaxObjectsCount);

	int roi_width = int(m_area.width * koef);
	int roi_height = int(m_area.height * koef);
	int roi_x = int(m_area.x * koef);
	int roi_y = int(m_area.y * koef);

	double koef_width = m_area.width / double(roi_width);
	double koef_height = m_area.height / double(roi_height);
	double koef_x = m_area.x / double(roi_x);
	double koef_y = m_area.y / double(roi_y);

	for (unsigned int d = 0; d < gd.boxes.size(); d++)
	{
		
		
		//gd.boxes[d].bbox.x = gd.boxes[d].bbox.x * koef_width + m_left - BBoxWidth;
		gd.boxes[d].bbox.x = m_area.x + gd.boxes[d].bbox.x * koef_x;
		gd.boxes[d].bbox.y = m_area.y + gd.boxes[d].bbox.y + koef_y;
		gd.boxes[d].bbox.width *= koef_width;
		gd.boxes[d].bbox.height *= koef_height;
		
		rectangle(m_RGBframe, gd.boxes[d].bbox, Scalar(255, 0, 0), 2);
		if (isInArea(gd.boxes[d].bbox))
		{
			rectangle(m_RGBframe, gd.boxes[d].bbox, Scalar(0, 255, 0), 2);
			m_boxes.push_back(Box(DETECTOR, gd.boxes[d].bbox, m_numero));
		}
	}
}
*/

/*
void Detectors::detectMovementHT()
{
	Rect rect;
	vector <Rect> boundRects = getBoundRects();
	
	for (vector<Rect>::iterator it = boundRects.begin(); it != boundRects.end(); it++)
	{
		rect = *it;
		Mat roi = m_RGBframe(rect);

		gd.Detect(roi,
			koef, bg_bias, hyp_penalty,
			MaxObjectsCount);

		int roi_width = int(rect.size().width * koef);
		int roi_height = int(rect.size().height * koef);
		int roi_x = int(rect.x * koef);
		int roi_y = int(rect.y * koef);
		double koef_width = rect.size().width / double(roi_width);
		double koef_height = rect.size().height / double(roi_height);
		double koef_x = rect.x / double(roi_x);
		double koef_y = rect.y / double(roi_y);



		for (int j = 0; j < gd.boxes.size(); j++)
		{
			gd.boxes[j].bbox.x = rect.x + gd.boxes[j].bbox.x * koef_x;
			gd.boxes[j].bbox.y = rect.y + gd.boxes[j].bbox.y * koef_y;
			gd.boxes[j].bbox.width *= koef_width;
			gd.boxes[j].bbox.height *= koef_height;

			if (isInArea(gd.boxes[j].bbox))
			{
				rectangle(m_RGBframe, gd.boxes[j].bbox, Scalar(0, 255, 0), 2);
				m_boxes.push_back(Box(DETECTOR, gd.boxes[j].bbox));
			}
		}
	}
}
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
				 rectangle(m_RGBframe, tmpRect, Scalar(0, 0, 255), 2);
				 m_boxes.push_back(Box(DETECTOR, tmpRect, m_numero));
			 }
		 }

	 }
	
}

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



vector<Box> Detectors::getBoxes()
{
	return m_boxes;
}

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

bool Detectors::isGoodSize(Rect t_rect) // 20, 20, 40, 70
{
	return (m_minWidth < t_rect.width && t_rect.width < m_maxWidth)
		&&
		(m_minHeight < t_rect.height && t_rect.height < m_maxHeight);
}

void Detectors::setFrames(cv::Mat t_RGBFrame, cv::Mat t_BWFrame, int t_numero)
{
	m_RGBframe = t_RGBFrame;
	m_BWframe = t_BWFrame;
	m_numero = t_numero;
}

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