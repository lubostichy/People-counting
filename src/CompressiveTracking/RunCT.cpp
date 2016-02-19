#include "RunCT.h"
#include "CompressiveTracker.h"
#include "../../Main.h"

using namespace std;
using namespace cv;

int RunCT::_ID;

RunCT::RunCT()
{
	_ID = 0;
}


RunCT::~RunCT()
{
	
}

void RunCT::addBox(Box box)
{

	switch (box.type)
	{
	case DETECTOR:
		this->_detBox.push_back(box);
		break;
	case TRACKER:
		this->_trackBox.push_back(box);
		break;
	case COUNTER:
		break;
	default:
		cout << "Ziadny typ boxu." << endl;
		break;
	}

}

bool RunCT::compareBoxes(Box dBox, Box tBox, int treshold)
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

void RunCT::setFrames(Mat frame, int counter, int left, int right)
{
	_frame = frame;
	cvtColor(frame, _BWframe, CV_RGB2GRAY);
	_actualFrameNO = counter;
	_leftCounter = left;
	_rightCounter = right;
}

void RunCT::initTracking()
{
	vector <int> ind;

	ind.clear();

	if (_detBox.size() > 0)
	{
		for (unsigned int d = 0; d < _detBox.size(); d++)
		{			
			_person.push_back(*new CompressiveTracker());
			int n = _person.size() - 1;
			_ID++;
			_person[n].ID = _ID;
			_person[n].currBB = _detBox[d].bbox;
			_person[n].firstBox = new Box(TRACKER, _detBox[d].bbox);			
			_person[n].init(_BWframe, _person[n].currBB);
			cout << "vytvorena: " << _person[n].ID << ", " << _person[n].firstBox->bbox.x << endl;
			ind.push_back(d);
		}
	}

	for (std::vector<int>::size_type d = ind.size() - 1; d != (std::vector<int>::size_type) - 1; d--)
	{
		//cout << "Mazem pouzity box: " << detBox[d].printBox() << endl;
		_detBox.erase(_detBox.begin() + ind[d]);
	}
}

/*
CompressiveTracker * RunCT::newPerson(Box bbox)
{
	CompressiveTracker *tmpPerson = new CompressiveTracker();
	_ID++;

	tmpPerson->ID = _ID;
	tmpPerson->currBB = bbox.bbox;
	tmpPerson->init(_BWframe, tmpPerson->currBB);

	tmpPerson->lost = 0;
	tmpPerson->firstBox = new Box(bbox);
	tmpPerson->lastBox = NULL;
	return tmpPerson;
}
*/

void RunCT::track()
{

	static int lost;
	Box tmpBox;
	vector<int> inp;
	vector<int> ind;

	inp.clear();
	ind.clear();
	
	for (unsigned int p = 0; p < _person.size(); p++)
	{
		
		
		_person[p].processFrame(_BWframe, _person[p].currBB);
		tmpBox.type = TRACKER;
		tmpBox.frameNO = _actualFrameNO;
		tmpBox.bbox = _person[p].currBB;
		
		
		
		rectangle(_frame, tmpBox.bbox, Scalar(0, 255, 0), 2);
		putText(_frame, std::to_string(_person[p].ID), Point(_person[p].currBB.x + 10, _person[p].currBB.y + _person[p].currBB.height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
		_trackBox.push_back(tmpBox);
		
		//if ((this->person[p].lastBox->bbox.x < (this->person[p].currBB.x + 3)) &&
		//	(this->person[p].lastBox->bbox.x >(this->person[p].currBB.x - 3)))
		if (_person[p].lastBox != NULL)
		{
			
			if (isInArea(_person[p]))
			{

				if (this->compareBoxes(*_person[p].lastBox, tmpBox, 1))
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
				}
			}
			else
			{				
				cout << "ratam: " << _person[p].ID << ", " << _person[p].firstBox->bbox.x << endl;
				counting(_person[p].firstBox, _person[p].lastBox);
				inp.push_back(p);
			}
		}
		_person[p].lastBox = new Box(_trackBox[_trackBox.size() - 1]);
	}

	for (std::vector<int>::size_type p = inp.size() - 1; p != (std::vector<int>::size_type) - 1; p--)
	{
		//cout << "Mazem ososbu" << _person[inp[p]].ID << endl;
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
					if (this->compareBoxes(_detBox[d], _trackBox[t], 20))
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
							ind.push_back(d);
					}
				}
			}
		}

		// vymazem detBox podla ulozenych indexov, ostatne boxy sa trackuju
		for (std::vector<int>::size_type d = ind.size() - 1; d != (std::vector<int>::size_type) - 1; d--)
		{
			//cout << "Mazem box: " << this->detBox[d].printBox() << endl;
			_detBox.erase(_detBox.begin() + ind[d]);
		}


		// vycistim zbytocne trackBoxy
		for (std::vector<int>::size_type t = _trackBox.size() - 1; t != (std::vector<int>::size_type) - 1; t--)
		{
			if (_trackBox[t].frameNO < _actualFrameNO)
				_trackBox.erase(_trackBox.begin() + t);
		}
	}


}

bool RunCT::isInArea(CompressiveTracker person)
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



void RunCT::counting(Box *first, Box *last)
{
	switch (_countingType)
	{
	case VERTICAL:
		if (first->bbox.x + first->bbox.width / 2 > _middlePoint
			&&
			last->bbox.x + last->bbox.width / 2 < _middlePoint)
		{
			_leftCounter++;
			//cout << _actualFrameNO << ", DOLAVA" << endl;
		}

		if (first->bbox.x + first->bbox.width / 2 < _middlePoint
			&&
			last->bbox.x + last->bbox.width / 2 > _middlePoint)
		{
			_rightCounter++;
			//cout << _actualFrameNO << ", DOPRAVA" << endl;
		}
		break;
	case HORIZONTAL:
		// TO DO
		break;
	}

}

void RunCT::setCounter(int type, int leftPoint, int midPoint, int rightPoint)
{
	_countingType = type;
	_leftPoint = leftPoint;
	_middlePoint = midPoint;
	_rightPoint = rightPoint;
}


unsigned int RunCT::getLeftCounter()
{
	return _leftCounter;
}

unsigned int RunCT::getRightCounter()
{
	return _rightCounter;
}


