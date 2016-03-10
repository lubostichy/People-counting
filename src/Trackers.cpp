#include "Trackers.hpp"

using namespace cv;
using namespace std;

int Trackers::m_ID;

Trackers::Trackers(
	int t_typeOfTracker, int t_typeOfLine,
	int t_leftPoint, int t_middlePoint, int t_rightPoint)
{
	m_typeOfTracker = t_typeOfTracker;
	m_line = t_typeOfLine;
	m_leftPoint = t_leftPoint;
	m_middlePoint = t_middlePoint;
	m_rightPoint = t_rightPoint;
	m_leftCounter = 0;
	m_rightCounter = 0;
	m_ID = 0;
}

Trackers::~Trackers()
{

}

void Trackers::track(vector<Box> t_detectedBoxes, Mat t_RGBframe, int t_numero)
{
	setFrames(t_RGBframe, t_numero);
	//for (vector<Box>::iterator it = t_detectedBoxes.begin(); it != t_detectedBoxes.end(); ++it)
	for (unsigned int b = 0; b < t_detectedBoxes.size(); b++)
	{
		//addBox(t_detectedBoxes[b]);
		m_detBox.push_back(t_detectedBoxes[b]);
	}

	if (m_typeOfTracker == CT)
	{
		trackCT();
		updatePersonsCT();
	}
	if (m_typeOfTracker == TLD)
	{
		trackTLD();
		updatePersonsTLD();
	}
	if (m_typeOfTracker == KCF)
	{
		trackKCF();
		updatePersonsKCF();
	}

}

void Trackers::trackCT()
{
	Box tmpBox;
	vector<int> inp;
	vector<int> ind;
	
	inp.clear();
	ind.clear();

	

	for (unsigned int p = 0; p < m_CTperson.size(); p++)
	{
		//cout << m_CTperson[p]->ID << "/" << m_numero << endl;
		m_CTperson[p]->processFrame(m_BWframe, m_CTperson[p]->currBB);

		tmpBox.type = TRACKER;
		tmpBox.frameNO = m_numero;
		tmpBox.bbox = Rect(m_CTperson[p]->currBB);
		
		rectangle(m_RGBframe, tmpBox.bbox, Scalar(0, 0, 255), 2);
		putText(m_RGBframe, std::to_string(m_CTperson[p]->ID), Point(m_CTperson[p]->currBB.x + 10, m_CTperson[p]->currBB.y + m_CTperson[p]->currBB.height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
		m_trackBox.push_back(tmpBox);

		//if ((this->person[p].lastBox->bbox.x < (this->person[p].currBB.x + 3)) &&
		//	(this->person[p].lastBox->bbox.x >(this->person[p].currBB.x - 3)))
		if (m_CTperson[p]->lastBox != NULL)
		{

			if (isInArea(m_CTperson[p]->lastBox))
			{

				if (compareBoxes(*m_CTperson[p]->lastBox, tmpBox, 1))
				{
					m_CTperson[p]->stay++;
					if (m_CTperson[p]->stay > 5)
					{
						counting(m_CTperson[p]->firstBox, m_CTperson[p]->lastBox);
						inp.push_back(p);
					}
				}
				else
				{
					m_CTperson[p]->stay = 0;
				}
			}
			else
			{

				counting(m_CTperson[p]->firstBox, m_CTperson[p]->lastBox);
				inp.push_back(p);
			}
		}
		m_CTperson[p]->lastBox = new Box(m_trackBox[m_trackBox.size() - 1]);

		for (std::vector<int>::size_type p = inp.size() - 1; p != (std::vector<int>::size_type) - 1; p--)
		{
			cout << "Mazem osobu" << m_CTperson[inp[p]]->ID << endl;
			m_CTperson.erase(m_CTperson.begin() + inp[p]);
		}

		bool exists;
		if (m_detBox.size() > 0)
		{
			for (unsigned int d = 0; d < m_detBox.size(); d++)
			{
				for (unsigned int t = 0; t < m_trackBox.size(); t++)
				{
					if (m_detBox[d].frameNO == m_trackBox[t].frameNO)
					{
						if (compareBoxes(m_detBox[d], m_trackBox[t], 20))
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
								cout << d << endl;
								ind.push_back(d);
							}
						}
					}
				}
			}

			sort(ind.begin(), ind.end());

			// vymazem detBox podla ulozenych indexov, ostatne boxy sa trackuju
			for (std::vector<int>::size_type d = ind.size() - 1; d != (std::vector<int>::size_type) - 1; d--)
			{
				cout << '_' << endl;
				cout << "Mazem box: " << m_detBox[d].printBox() << endl;
				m_detBox.erase(m_detBox.begin() + ind[d]);
			}
			ind.clear();

			// vycistim zbytocne trackBoxy
			for (std::vector<int>::size_type t = m_trackBox.size() - 1; t != (std::vector<int>::size_type) - 1; t--)
			{
				if (m_trackBox[t].frameNO < m_numero)
					m_trackBox.erase(m_trackBox.begin() + t);
			}
		}

	}
}

void Trackers::updatePersonsCT()
{
	vector <int> ind;

	if (m_detBox.size() > 0)
	{
		for (unsigned int d = 0; d < m_detBox.size(); d++)
		{
			 
			m_CTperson.push_back(new CompressiveTracker());
			int n = m_CTperson.size() - 1;
			m_ID++;
			m_CTperson[n]->ID = m_ID;
			m_CTperson[n]->currBB = m_detBox[d].bbox;
			m_CTperson[n]->firstBox = new Box(m_detBox[d]);
			m_CTperson[n]->init(m_BWframe, m_CTperson[n]->currBB);
			cout << "Vytvaram osobu: " << m_CTperson[n]->ID << "s boxom: " << m_CTperson[n]->firstBox->printBox() << endl;
			ind.push_back(d);
		}
	}

	for (std::vector<int>::size_type d = ind.size() - 1; d != (std::vector<int>::size_type) - 1; d--)
	{
		//cout << "Mazem pouzity box: " << detBox[d].printBox() << endl;
		m_detBox.erase(m_detBox.begin() + ind[d]);
	}
}

void Trackers::trackTLD()
{
	Box tmpBox;
	vector<int> ind, inp;
	int numOfNewBox;
	
	for (unsigned int p = 0; p < m_TLDperson.size(); p++)
	{

		if (!m_TLDperson[p]->skipProcessingOnce)
		{

			m_TLDperson[p]->processImage(m_RGBframe);
		}
		else
		{
			m_TLDperson[p]->skipProcessingOnce = false;
		}

		// nasla sa detekcia
		if (m_TLDperson[p]->currBB != NULL)
		{
			rectangle(m_RGBframe, *m_TLDperson[p]->currBB, Scalar(0, 0, 255), 2);
			putText(m_RGBframe, to_string(m_TLDperson[p]->ID), Point(m_TLDperson[p]->currBB->x + 10, m_TLDperson[p]->currBB->y + m_TLDperson[p]->currBB->height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
			tmpBox.frameNO = m_numero;
			tmpBox.bbox = *m_TLDperson[p]->currBB;
			m_trackBox.push_back(tmpBox);
			if (m_TLDperson[p]->lastBox != NULL)
			{
				if (isInArea(m_TLDperson[p]->lastBox))
				{
					if (this->compareBoxes(tmpBox, *m_TLDperson[p]->lastBox, 1))
					{
						m_TLDperson[p]->stay++;
						if (m_TLDperson[p]->stay > 5)
						{
							counting(m_TLDperson[p]->firstBox, m_TLDperson[p]->lastBox);
							inp.push_back(p);
						}
					}
					else
					{
						m_TLDperson[p]->stay = 0;
					}
				}
				else
				{
					//_person[p]->lastBox = new Box(_trackBox[_trackBox.size() - 1]);
					counting(m_TLDperson[p]->firstBox, m_TLDperson[p]->lastBox);
					inp.push_back(p);
				}

				//this->person[p].lastBox->frameNO = trackBox[trackBox.size() - 1].frameNO;			
			}
			m_TLDperson[p]->lastBox = new Box(m_trackBox[m_trackBox.size() - 1]);
		}
		else
		{

			m_TLDperson[p]->lost++;
			//cout << this->person[p].lastBox->bbox.width << ' ' << this->person[p].lastBox->bbox.height << ' ' << this->person[p].lastBox->frameNO << endl;

			if (m_TLDperson[p]->lost > 20)
			{
				// ulozim index	
				if (m_TLDperson[p]->lastBox != NULL)
					counting(m_TLDperson[p]->firstBox, m_TLDperson[p]->lastBox);
				inp.push_back(p);

			}

		}
	}


	// vymazem ulozene
	for (vector<int>::size_type p = inp.size() - 1; p != (vector<int>::size_type) - 1; p--)
	{
		//cout << "Mazem ososbu" << _person[inp[p]]->ID << endl;
		m_TLDperson.erase(m_TLDperson.begin() + inp[p]);
	}
	inp.clear();

	bool exists;
	if (m_detBox.size() > 0)
	{
		for (unsigned int d = 0; d < m_detBox.size(); d++)
		{
			for (unsigned int t = 0; t < m_trackBox.size(); t++)
			{
				if (m_detBox[d].frameNO == m_trackBox[t].frameNO)
				{
					if (compareBoxes(m_detBox[d], m_trackBox[t], 40)) // 20
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
			m_detBox.erase(m_detBox.begin() + ind[d]);
		}
		ind.clear();
		// vycistim zbytocne trackBoxy
		for (vector<int>::size_type t = m_trackBox.size() - 1; t != (vector<int>::size_type) - 1; t--)
		{
			if (m_trackBox[t].frameNO < m_numero)
				m_trackBox.erase(m_trackBox.begin() + t);
		}
	}
}

void Trackers::updatePersonsTLD()
{
	vector <int> ind;
	int n;

	


	if (m_detBox.size() > 0) // ak nieco detekovalo
	{

		// vytvorim newPersonTracking z ponechanych detBoxov
		for (unsigned int d = 0; d < m_detBox.size(); d++)
		{
			//tld::TLD *tmp = new tld::TLD();
			//tld::TLD *tmp = newPerson(detBox[d]); // operator
			//_person.push_back(*new tld::TLD(*tmp));		// deep copy
			//delete tmp;

			m_TLDperson.push_back(new tld::TLD());
			//_person.emplace_back();

			n = m_TLDperson.size() - 1;
			m_ID++;
			m_TLDperson[n]->ID = m_ID;
			m_TLDperson[n]->firstBox = new Box(COUNTER, m_detBox[d].bbox);			
			m_TLDperson[n]->firstBox->setFrameNO(m_numero);

			Mat grey(m_RGBframe.size().height, m_RGBframe.size().width, CV_8UC1);
			cvtColor(m_RGBframe, grey, CV_BGR2GRAY);

			m_TLDperson[n]->detectorCascade->imgWidth = grey.cols;
			m_TLDperson[n]->detectorCascade->imgHeight = grey.rows;
			m_TLDperson[n]->detectorCascade->imgWidthStep = grey.step;
			m_TLDperson[n]->selectObject(grey, &m_detBox[d].bbox);

			m_TLDperson[n]->detectorCascade->varianceFilter->enabled = true;
			m_TLDperson[n]->detectorCascade->ensembleClassifier->enabled = true;
			m_TLDperson[n]->detectorCascade->nnClassifier->enabled = true;

			// classifier
			m_TLDperson[n]->detectorCascade->useShift = true;
			m_TLDperson[n]->detectorCascade->shift = 0.1;
			m_TLDperson[n]->detectorCascade->minScale = -10;
			m_TLDperson[n]->detectorCascade->maxScale = 10;
			m_TLDperson[n]->detectorCascade->minSize = 25;
			m_TLDperson[n]->detectorCascade->numTrees = 10;
			m_TLDperson[n]->detectorCascade->numFeatures = 13;

			m_TLDperson[n]->detectorCascade->nnClassifier->thetaTP = 0.65;
			m_TLDperson[n]->detectorCascade->nnClassifier->thetaFP = 0.5;

			m_TLDperson[n]->detectorCascade->numFeatures = 13;
			m_TLDperson[n]->detectorCascade->useShift = true;


			m_TLDperson[n]->skipProcessingOnce = false;

			ind.push_back(d);
			//cout << "Osoba ID " << person[person.size() - 1].ID << " bola vytvorena." << endl;
		}

		//vymazem pouzite detBoxy
		// vymazem detBox podla ulozenych indexov, ostatne boxy sa trackuju
		for (vector<int>::size_type d = ind.size() - 1; d != (vector<int>::size_type) - 1; d--)
		{
			//cout << "Mazem pouzity box: " << detBox[d].printBox() << endl;
			m_detBox.erase(m_detBox.begin() + ind[d]);
		}

		ind.clear();
	}
}

void Trackers::trackKCF()
{
	Box tmpBox;
	Rect result;
	vector<int> ind, inp;
	ind.clear();
	inp.clear();

	for (unsigned int p = 0; p < m_KCFperson.size(); p++)
	{
		result = m_KCFperson[p]->update(m_RGBframe);
		rectangle(m_RGBframe, result, Scalar(0, 0, 255), 2);
		putText(m_RGBframe, to_string(m_KCFperson[p]->ID), Point(result.x, result.y + result.height), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
		tmpBox.frameNO = m_numero;
		tmpBox.bbox = result;
		m_trackBox.push_back(tmpBox);
		if (m_KCFperson[p]->lastBox != NULL)
		{
			if (isInArea(m_KCFperson[p]->lastBox))
			{
				if (compareBoxes(tmpBox, *m_KCFperson[p]->lastBox, 1))
				{
					m_KCFperson[p]->stay++;
					if (m_KCFperson[p]->stay > 5)
					{
						counting(m_KCFperson[p]->firstBox, m_KCFperson[p]->lastBox);
						inp.push_back(p);
					}
				}
				else
				{
					m_KCFperson[p]->stay = 0;
					//_person[p].lastBox = new Box(_trackBox[_trackBox.size() - 1]);
				}
			}
			else
			{
				counting(m_KCFperson[p]->firstBox, m_KCFperson[p]->lastBox);
				inp.push_back(p);

			}

		}

		m_KCFperson[p]->lastBox = new Box(m_trackBox[m_trackBox.size() - 1]);
	}

	for (vector<int>::size_type p = inp.size() - 1; p != (vector<int>::size_type) - 1; p--)
	{
		//cout << "Mazem ososbu" << this->person[inp[p]].ID << endl;
		m_KCFperson.erase(m_KCFperson.begin() + inp[p]);
	}


	bool exists;
	if (m_detBox.size() > 0)
	{
		for (unsigned int d = 0; d < m_detBox.size(); d++)
		{
			for (unsigned int t = 0; t < m_trackBox.size(); t++)
			{
				if (m_detBox[d].frameNO == m_trackBox[t].frameNO)
				{
					if (compareBoxes(m_detBox[d], m_trackBox[t], 20))
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
			m_detBox.erase(m_detBox.begin() + ind[d]);
		}
		ind.clear();
		// vycistim zbytocne trackBoxy
		for (vector<int>::size_type t = m_trackBox.size() - 1; t != (vector<int>::size_type) - 1; t--)
		{
			if (m_trackBox[t].frameNO < m_numero)
				m_trackBox.erase(m_trackBox.begin() + t);
		}
	}
}

void Trackers::updatePersonsKCF()
{
	vector <int> ind;

	ind.clear();

	if (m_detBox.size() > 0) // ak nieco detekovalo
	{

		// vytvorim newPersonTracking z ponechanych detBoxov
		for (unsigned int d = 0; d < m_detBox.size(); d++)
		{
			//KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
			//KCFTracker tmp(false, true, true, false);
			//KCFTracker * tmp = new KCFTracker(false, true, true, false);
			m_KCFperson.push_back(new KCFTracker());
			int n = m_KCFperson.size() - 1;
			m_KCFperson[n]->init(m_detBox[d].bbox, m_RGBframe);
			m_KCFperson[n]->ID = m_ID++;
			m_KCFperson[n]->lost = 0;
			m_KCFperson[n]->firstBox = new Box(m_detBox[d]);
			
			//delete tmp;
			ind.push_back(d);
			//cout << "Osoba ID " << person[person.size() - 1].ID << " bola vytvorena." << endl;
		}

		//vymazem pouzite detBoxy
		// vymazem detBox podla ulozenych indexov, ostatne boxy sa trackuju
		for (vector<int>::size_type d = ind.size() - 1; d != (vector<int>::size_type) - 1; d--)
		{
			//cout << "Mazem pouzity box: " << detBox[d].printBox() << endl;
			m_detBox.erase(m_detBox.begin() + ind[d]);
		}


	}
}


void Trackers::addBox(Box box)
{
	if (box.type == DETECTOR)
	{
		m_detBox.push_back(box);
	}
	else if (box.type == TRACKER)
	{	
		m_trackBox.push_back(box);		
	}
}

void Trackers::setFrames(cv::Mat t_RGBframe, int t_numero)
{
	m_RGBframe = t_RGBframe;
	cvtColor(m_RGBframe, m_BWframe, CV_RGB2GRAY);
	m_numero = t_numero;
}

bool Trackers::isInArea(Box *t_lastBox)
{
	if (m_line == VERTICAL)
	{
		return ((t_lastBox->bbox.x + t_lastBox->bbox.width / 2 > m_leftPoint)
			&&
			(t_lastBox->bbox.x + t_lastBox->bbox.width / 2 < m_rightPoint));
	}
	else if (m_line == HORIZONTAL)
	{
		return ((t_lastBox->bbox.y + t_lastBox->bbox.height / 2 > m_leftPoint)
			&&
			(t_lastBox->bbox.y + t_lastBox->bbox.height / 2 < m_rightPoint));	
	}
	return false;
}



void Trackers::counting(Box *t_first, Box *t_last)
{
	if (m_line == VERTICAL)
	{
		if (t_first->bbox.x + t_first->bbox.width / 2 > m_middlePoint
			&&
			t_last->bbox.x + t_last->bbox.width / 2 < m_middlePoint)
		{
			m_leftCounter++;
			cout << m_numero << ",L" << endl;
		}

		if (t_first->bbox.x + t_first->bbox.width / 2 < m_middlePoint
			&&
			t_last->bbox.x + t_last->bbox.width / 2 > m_middlePoint)
		{
			m_rightCounter++;
			cout << m_numero << ",R" << endl;
		}
	}
	else if (m_line == HORIZONTAL)
	{
		if (t_first->bbox.y + t_first->bbox.height / 2 > m_middlePoint
			&&
			t_last->bbox.y + t_last->bbox.height / 2 < m_middlePoint)
		{
			m_leftCounter++;
			cout << m_numero << ",T" << endl;
		}

		if (t_first->bbox.y + t_first->bbox.height / 2 < m_middlePoint
			&&
			t_last->bbox.y + t_last->bbox.height / 2 > m_middlePoint)
		{
			m_rightCounter++;
			cout << m_numero << ",B" << endl;
		}		
	}

}

bool Trackers::compareBoxes(Box dBox, Box tBox, int treshold)
{
	int dCenter;
	int tCenter;

	dCenter = dBox.bbox.x + dBox.bbox.width / 2;
	tCenter = tBox.bbox.x + tBox.bbox.width / 2;

	//cout << "Porovnavam: " << dCenter << " vs. " << tCenter << endl;

	return (abs(dCenter - tCenter) <= treshold);
		
}

int Trackers::getLeftCounter()
{
	return m_leftCounter;
}

int Trackers::getRightCounter()
{
	return m_rightCounter;
}