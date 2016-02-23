#include <opencv2\opencv.hpp>
#include "KCF.h"
#include "../Box.h"
#include "../../Main.h"
#include "kcftracker.hpp"

using namespace cv;
using namespace std;

int RunKCF::_ID;

RunKCF::RunKCF()
{

}

RunKCF::~RunKCF()
{


}

void RunKCF::track()
{
	Box tmpBox;
	Rect result;
	vector<int> ind, inp;
	ind.clear();
	inp.clear();

	for (unsigned int p = 0; p < _person.size(); p++)
	{
		result = _person[p].update(_RGBframe);
		rectangle(_RGBframe, result, Scalar(0, 0, 255), 2);
		putText(_RGBframe, to_string(_person[p].ID), Point(result.x, result.y + result.height), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
		tmpBox.frameNO = _actualFrameNO;
		tmpBox.bbox = result;
		_trackBox.push_back(tmpBox);
		if (_person[p].lastBox != NULL)
		{
			if (isInArea(_person[p]))
			{
				if (compareBoxes(tmpBox, *_person[p].lastBox, 1))
				{
					_person[p].lost++;
					if (_person[p].lost > 5)
					{
						inp.push_back(p);
					}
				}
				else
				{
					_person[p].lost = 0;
					//_person[p].lastBox = new Box(_trackBox[_trackBox.size() - 1]);
				}
			}
			else
			{
				counting(_person[p].firstBox, _person[p].lastBox);
				inp.push_back(p);

			}
			
		}
		
		_person[p].lastBox = new Box(_trackBox[_trackBox.size() - 1]);
	}

	for (vector<int>::size_type p = inp.size() - 1; p != (vector<int>::size_type) - 1; p--)
	{
		//cout << "Mazem ososbu" << this->person[inp[p]].ID << endl;
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
						{

							if (ind[isd] == d)
								exists = true;
						}

						// ulozim indexy
						if (!exists)
						{
							//cout << _actualFrameNO << endl;
							ind.push_back(d);
						}
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

bool RunKCF::isInArea(KCFTracker person)
{
	switch (_countingType)
	{
	case VERTICAL:
		return ((person.lastBox->bbox.x + person.lastBox->bbox.width / 2 > _leftPoint)
			&&
			(person.lastBox->bbox.x + person.lastBox->bbox.width / 2 < _rightPoint));		
		break;
	case HORIZONTAL:
		break;
	}
	return false;
}


void RunKCF::counting(Box *first, Box *last)
{	
	switch (_countingType)
	{
	case VERTICAL:
		if (first->bbox.x + first->bbox.width / 2 > _middlePoint
			&&
			last->bbox.x + last->bbox.width / 2 < _middlePoint)
		{
			_leftCounter++;
			cout << _actualFrameNO << ", LEFT" << endl;
		}

		if (first->bbox.x + first->bbox.width / 2 < _middlePoint
			&&
			last->bbox.x + last->bbox.width / 2 > _middlePoint)
		{
			_rightCounter++;
			cout << _actualFrameNO << ", RIGHT" << endl;
		}
		break;
	case HORIZONTAL:
		// TO DO
		break;
	}
}

void RunKCF::setCounter(int type, int leftPoint, int midPoint, int rightPoint)
{
	_countingType = type;
	_leftPoint = leftPoint;
	_middlePoint = midPoint;
	_rightPoint = rightPoint;
}

void RunKCF::addBox(Box box)
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

void RunKCF::setFrames(Mat frame, int NO, int left, int right)
{
	_RGBframe = frame;
	_actualFrameNO = NO;
	_leftCounter = left;
	_rightCounter = right;
}

void RunKCF::initTracking()
{
	vector <int> ind;

	ind.clear();

	if (_detBox.size() > 0) // ak nieco detekovalo
	{

		// vytvorim newPersonTracking z ponechanych detBoxov
		for (unsigned int d = 0; d < _detBox.size(); d++)
		{
			//KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
			//KCFTracker tmp(false, true, true, false);
			KCFTracker * tmp = new KCFTracker(false, true, true, false);			
			tmp->init(_detBox[d].bbox, _RGBframe);
			tmp->ID = _ID++;
			tmp->lost = 0;
			tmp->firstBox = new Box(_detBox[d]);
			_person.push_back(*new KCFTracker(*tmp));
			delete tmp;			
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

bool RunKCF::compareBoxes(Box dBox, Box tBox, int treshold)
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

unsigned int RunKCF::getLeftCounter()
{
	return _leftCounter;
}

unsigned int RunKCF::getRightCounter()
{
	return _rightCounter;
}