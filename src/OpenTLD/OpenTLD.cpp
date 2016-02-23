#include "OpenTLD.h"
#include "tld/TLD.h"
#include "../../Main.h"
//#include "imacq/ImAcq.h"
//#include "Config.h"
#include <opencv2/opencv.hpp>
//#include "Trajectory.h"
#include "tld/TLDUtil.h"

//using namespace tld;
using namespace cv;
using namespace std;

int OpenTLD::_ID;



OpenTLD::OpenTLD()
{
	_ID = 0;


	//// new Main
	//tld = new tld::TLD();
	showOutput = 1;
	printResults = NULL;
	saveDir = ".";
	threshold = 0.5;
	showForeground = 0;

	showTrajectory = false;
	trajectoryLength = 0;

	selectManually = 0;

	initialBB = NULL;
	showNotConfident = true;

	reinit = 0;

	loadModel = false;

	exportModelAfterRun = false;
	modelExportFile = "model";
	seed = 0;

	modelPath = NULL;
	//imAcq = NULL;

	//// imacqalloc
	//imAcq = imAcqAlloc();

	/*
	//// config.init
	Config config;
	config.init(this->device, this->path, this->startFrame, this->bbox);

	//// config.configure
	config.configure(this);

	//// srand seed
	srand(this->seed);

	//// imacqinit
	imAcqInit(imAcq);
	*/
	//// gui init
	// ??

	//// main.dowork - samotne trackovanie

	// v main bude treba:
	// vstup int framu, bbox
	// vystup bbox, frame alebo Rects
}


OpenTLD::~OpenTLD()
{
	//delete tld;
	//imAcqFree(imAcq);
}

void OpenTLD::addBox(Box box)
{
	
	switch (box.type)
	{
	case DETECTOR:
		_detBox.push_back(box);
		break;
	case TRACKER:
		_trackBox.push_back(box);
		break;
	case COUNTER:
		break;
	}

}

void OpenTLD::initTracking()
{
	//cout << "# initTracking..." << endl;

	vector <int> ind;
	int n;

	ind.clear();
	

	if (_detBox.size() > 0) // ak nieco detekovalo
	{

		// vytvorim newPersonTracking z ponechanych detBoxov
		for (unsigned int d = 0; d < _detBox.size(); d++)
		{
			//tld::TLD *tmp = new tld::TLD();
			//tld::TLD *tmp = newPerson(detBox[d]); // operator
			//_person.push_back(*new tld::TLD(*tmp));		// deep copy
			//delete tmp;
			
			_person.push_back(new tld::TLD());
			//_person.emplace_back();
			
			n = _person.size() - 1;
			_ID++;
			_person[n]->ID = _ID;
			_person[n]->firstBox = new Box(COUNTER, _detBox[d].bbox);
			_person[n]->firstBox->setFrameNO(_actualFrameNO);

			Mat grey(img->height, img->width, CV_8UC1);
			cvtColor(_frame, grey, CV_BGR2GRAY);			

			_person[n]->detectorCascade->imgWidth = grey.cols;
			_person[n]->detectorCascade->imgHeight = grey.rows;
			_person[n]->detectorCascade->imgWidthStep = grey.step;
			_person[n]->selectObject(grey, &_detBox[d].bbox);

			_person[n]->detectorCascade->varianceFilter->enabled = true;
			_person[n]->detectorCascade->ensembleClassifier->enabled = true;
			_person[n]->detectorCascade->nnClassifier->enabled = true;

			// classifier
			_person[n]->detectorCascade->useShift = true;
			_person[n]->detectorCascade->shift = 0.1;
			_person[n]->detectorCascade->minScale = -10;
			_person[n]->detectorCascade->maxScale = 10;
			_person[n]->detectorCascade->minSize = 25;
			_person[n]->detectorCascade->numTrees = 10;
			_person[n]->detectorCascade->numFeatures = 13;

			_person[n]->detectorCascade->nnClassifier->thetaTP = 0.65;
			_person[n]->detectorCascade->nnClassifier->thetaFP = 0.5;

			_person[n]->detectorCascade->numFeatures = 13;
			_person[n]->detectorCascade->useShift = true;


			_person[n]->skipProcessingOnce = false;

			ind.push_back(d);			
			//cout << "Osoba ID " << person[person.size() - 1].ID << " bola vytvorena." << endl;
		}

		//vymazem pouzite detBoxy
		// vymazem detBox podla ulozenych indexov, ostatne boxy sa trackuju
		for (vector<int>::size_type d = ind.size() - 1; d != (vector<int>::size_type) - 1; d--)
		{
			//cout << "Mazem pouzity box: " << detBox[d].printBox() << endl;
			_detBox.erase(_detBox.begin() + ind[d]);
		}


	}


}



bool OpenTLD::compareBoxes(Box dBox, Box tBox, int treshold)
{
	int dCenter;
	int tCenter;

	dCenter = dBox.bbox.x + dBox.bbox.width / 2;
	tCenter = tBox.bbox.x + tBox.bbox.width / 2;

	//cout << "Porovnavam: " << dCenter << " vs. " << tCenter << endl;

	if ((abs(dCenter - tCenter)) <= treshold)
		return true;

	return false;
}


tld::TLD* OpenTLD::newPerson(Box bbox)
{
	tld::TLD *tmpPerson = new tld::TLD();

	_ID++;
	

	tmpPerson->ID = _ID;

	//cout << "Vytvaram osobu ID: " << tmpPerson->ID << ", box: " << bbox.printBox() << endl;

	tmpPerson->alternating = false;
	tmpPerson->learningEnabled = true;
	tmpPerson->trackerEnabled = true;


	// nad whilom bolo
	Mat grey(this->img->height, this->img->width, CV_8UC1);
	cvtColor(_frame, grey, CV_BGR2GRAY);

	tmpPerson->detectorCascade->imgWidth = grey.cols;
	tmpPerson->detectorCascade->imgHeight = grey.rows;
	tmpPerson->detectorCascade->imgWidthStep = grey.step;

	
	tmpPerson->selectObject(grey, &(bbox.bbox));

	tmpPerson->detectorCascade->varianceFilter->enabled = true;
	tmpPerson->detectorCascade->ensembleClassifier->enabled = true;
	tmpPerson->detectorCascade->nnClassifier->enabled = true;

	// classifier
	tmpPerson->detectorCascade->useShift = true;
	tmpPerson->detectorCascade->shift = 0.1;
	tmpPerson->detectorCascade->minScale = -10;
	tmpPerson->detectorCascade->maxScale = 10;
	tmpPerson->detectorCascade->minSize = 25;
	tmpPerson->detectorCascade->numTrees = 10;
	tmpPerson->detectorCascade->numFeatures = 13;

	tmpPerson->detectorCascade->nnClassifier->thetaTP = 0.65;
	tmpPerson->detectorCascade->nnClassifier->thetaFP = 0.5;

	tmpPerson->detectorCascade->numFeatures = 13;
	tmpPerson->detectorCascade->useShift = true;


	tmpPerson->skipProcessingOnce = false;

	tmpPerson->lost = 0;
	tmpPerson->firstBox = new Box(COUNTER, bbox.bbox);
	tmpPerson->lastBox = NULL;

	return tmpPerson;
}
/*
bool OpenTLD::findBox(tld::TLD person, Box detBox)
{
	int DISTANCE = 400;
	Mat image = this->frame(person.lastBox->bbox);
	cout << image.size().width << ' ' << image.size().height << endl; // 0 0
	resize(image, image, Size(64, 128));
	Mat img;
	cvtColor(image, img, CV_RGB2GRAY);

	Mat image2 = this->frame(detBox.bbox);
	resize(image2, image2, Size(64, 128));
	Mat img2;
	cvtColor(image2, img2, CV_RGB2GRAY);

	vector<float> features;
	vector<Point> locations;
	vector<float> features2;
	vector<Point> locations2;

	HOGDescriptor *hog = new HOGDescriptor();
	HOGDescriptor *hog2 = new HOGDescriptor();

	hog->compute(img, features, Size(32, 32), Size(0, 0), locations);
	cout << features.size() << endl;;

	hog2->compute(img2, features2, Size(32, 32), Size(0, 0), locations2);
	cout << features2.size() << endl;

	Mat Hogfeat;
	Hogfeat.create(features.size(), 1, CV_32FC1);
	for (int i = 0; i<features.size(); i++)
		Hogfeat.at<float>(i, 0) = features.at(i);

	Mat Hogfeat2;
	Hogfeat2.create(features2.size(), 1, CV_32FC1);
	for (int i = 0; i<features2.size(); i++)
		Hogfeat2.at<float>(i, 0) = features2.at(i);

	double distance = 0;
	for (int i = 0; i<Hogfeat.rows; i++)
	{
		distance += abs(Hogfeat.at<float>(i, 0) - Hogfeat2.at<float>(i, 0));
	}
	if (distance < DISTANCE)
	{

		cout << distance << endl;
		return true;
	}


	return false;
}
*/

void OpenTLD::setCounter(int type, int left, int middle, int right)
{
	_countingType = type;
	_leftPoint = left;
	_middlePoint = middle;
	_rightPoint = right;
}

void OpenTLD::counting(Box *first, Box *last)
{
	switch (_countingType)
	{
	case VERTICAL:
		if (first->bbox.x + first->bbox.width / 2 > _middlePoint
			&&
			last->bbox.x + last->bbox.width / 2 < _middlePoint)
		{
			_leftCounter++;
			cout << _actualFrameNO << ". LEFT" << endl;
		}

		if (first->bbox.x + first->bbox.width / 2 < _middlePoint
			&&
			last->bbox.x + last->bbox.width / 2 > _middlePoint)
		{
			_rightCounter++;
			cout << _actualFrameNO << ". RIGHT" << endl;
		}
		break;
	case HORIZONTAL:
		// TO DO
		break;
	}
}

void OpenTLD::track()
{
	//static int lost;
	//Box lastBox;
	Box tmpBox;
	vector<int> ind, inp;
	int numOfNewBox;

	ind.clear();
	inp.clear();

	
	for (unsigned int p = 0; p < _person.size(); p++)
	{		
		
		if (!_person[p]->skipProcessingOnce)
		{
			
			_person[p]->processImage(_frame);
		}
		else
		{
			_person[p]->skipProcessingOnce = false;
		}

		// nasla sa detekcia
		if (_person[p]->currBB != NULL)
		{			
			rectangle(_frame, *_person[p]->currBB, Scalar(0, 0, 255), 2);
			putText(_frame, to_string(_person[p]->ID), Point(_person[p]->currBB->x + 10,_person[p]->currBB->y + _person[p]->currBB->height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1);
			tmpBox.frameNO = _actualFrameNO;
			tmpBox.bbox = *_person[p]->currBB;
			_trackBox.push_back(tmpBox);
			if (_person[p]->lastBox != NULL) 
			{
				if (isInArea(_person[p]->lastBox))
				{
					if (this->compareBoxes(tmpBox, *_person[p]->lastBox, 1))
					{
						_person[p]->lost++;
						if (_person[p]->lost > 10)
						{
							counting(_person[p]->firstBox, _person[p]->lastBox);
							inp.push_back(p);
						}
					}
					else
					{
						_person[p]->lost = 0;
					}
				}
				else
				{
					//_person[p]->lastBox = new Box(_trackBox[_trackBox.size() - 1]);
					counting(_person[p]->firstBox, _person[p]->lastBox);
					inp.push_back(p);
				}
				
				//this->person[p].lastBox->frameNO = trackBox[trackBox.size() - 1].frameNO;			
			}
			_person[p]->lastBox = new Box(_trackBox[_trackBox.size() - 1]);
		}
		else
		{
			
			_person[p]->lost++;
			//cout << this->person[p].lastBox->bbox.width << ' ' << this->person[p].lastBox->bbox.height << ' ' << this->person[p].lastBox->frameNO << endl;
			
			if (_person[p]->lost > 10)
			{
				// ulozim index				
				inp.push_back(p);								

			}
			
		}
	}
	

	// vymazem ulozene
	for (vector<int>::size_type p = inp.size() - 1; p != (vector<int>::size_type) - 1; p--)
	{
		cout << "Mazem ososbu" << _person[inp[p]]->ID << endl;
		_person.erase(_person.begin() + inp[p]);
	}


	bool exists;
	if (_detBox.size() > 0)
	{
		for (unsigned int d = 0; d < _detBox.size(); d++)
		{
			for (unsigned int t = 0; t < _trackBox.size(); t++)
			{
				if (_detBox[d].frameNO == _trackBox[t].frameNO)
				{
					if (compareBoxes(_detBox[d], _trackBox[t], 20))
					{
						//cout << "Ukladam box pre vymazanie: " << detBox[d].printBox() << endl;

						exists = false;
						//ak je uz ulozeny neukladam
						for (unsigned int isd = 0; isd < ind.size(); isd++)	
							if (ind[isd] == d) 
								exists = true;

						// ulozim indexy
						if (!exists) 
							ind.push_back(d);
					}
				}
			}
		}

		// vymazem detBox podla ulozenych indexov, ostatne boxy sa trackuju
		for (vector<int>::size_type d = ind.size() - 1; d != (vector<int>::size_type) - 1; d--)
		{
			//cout << "Mazem box: " << this->detBox[d].printBox() << endl;
			_detBox.erase(_detBox.begin() + ind[d]);
		}

		// vycistim zbytocne trackBoxy
		for (vector<int>::size_type t = _trackBox.size() - 1; t != (vector<int>::size_type) - 1; t--)
		{
			if (_trackBox[t].frameNO < _actualFrameNO)
				_trackBox.erase(_trackBox.begin() + t);
		}
	}

}

bool OpenTLD::isInArea(Box *lastBox)
{

	switch (_countingType)
	{
	case VERTICAL:		
		return ((lastBox->bbox.x + lastBox->bbox.width / 2 > _leftPoint)
			&&
			(lastBox->bbox.x + lastBox->bbox.width / 2 < _rightPoint));
		break;
	case HORIZONTAL:
		break;
	}
	return false;
}

int OpenTLD::getLeftCounter()
{
	return _leftCounter;
}

int OpenTLD::getRightCounter()
{
	return _rightCounter;
}


void OpenTLD::setFrames(Mat frame, int actualFrameNO, int leftCounter, int rightCounter)
{
	this->img = new IplImage(frame);

	_frame = frame;
	_actualFrameNO = actualFrameNO;
	moreFrames = true;
	_leftCounter = leftCounter;
	_rightCounter = rightCounter;
}

void OpenTLD::setDevice(string device)
{
	this->device = device;
}

void OpenTLD::setPath(string path)
{
	this->path = path;
}

void OpenTLD::setStartFrame(int startFrame)
{
	this->startFrame = startFrame;
}

void OpenTLD::setBBox(vector<int> bbox)
{
	this->bbox = bbox;
}

