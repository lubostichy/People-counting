/*
 * Poèítání prùchodù osob dveømi s využitím stacionární kamery
 * Bakalárska práca, 2016
 * ¼uboš Tichý, xtichy23
 * Main.cpp
 */

#include <iostream>
#include <opencv2\opencv.hpp>
#include "Main.hpp"
#include "src\Box.h"
#include "src\Detectors.hpp"
#include "src\Trackers.hpp"


using namespace cv;
using namespace std;

/* 
 * MAIN
 */
int main(int argc, char** argv)
{
	
	

	if (argc == 2) 
	{
		loadConfig(argv);
		//countingThresh = stoi(argv[2]);
		//compareThresh = stoi(argv[3]);
		
	}
	else
	{
		cerr << "Specify path to config file." << endl;
		printHelp();
		return EXIT_FAILURE;
	}


	printConfig();

	cout << "Counting..." << endl;
	cout << "============================================================" << endl;

	doWork();

	cout << "============================================================" << endl;
	cout << "Counting is done." << endl;

	return EXIT_SUCCESS;
}

/* 
Vytlaèí pomocníka na obrazovku. 
*/
void printHelp()
{
	cout << "Run as: bp.exe [config file]" << endl;
}

/* 
Hlavný cyklus poèítania. 
*/
void doWork()
{
	int frameCounter = 0;
	Mat frame;
	Mat fgmask;
	Mat gray;
	VideoCapture cap;
	
	Detectors *detectors = new Detectors(
		typeOfDetector, typeOfLine,
		leftPoint, middlePoint, rightPoint,
		minWidthBox, maxWidthBox,
		minHeightBox, maxHeightBox,
		cascadeClassifier);

	Trackers *trackers = new Trackers(
		typeOfTracker, typeOfLine,
		leftPoint, middlePoint, rightPoint);


	BackgroundSubtractorMOG2 *pMOG2;
	
	unsigned int leftCounter = 0;
	unsigned int rightCounter = 0;
	

	cout << "Opening source... " << endl;
	cap.open(source);
	if (!cap.isOpened())
	{
		cerr << "Cannot open source." << '\n';		
	}
	cout << "Done." << endl;

	
	pMOG2 = new BackgroundSubtractorMOG2(0, 16.0, true);
	
	int size;
	Mat data;
	Mat plot_result;

	

	// hlavny cyklus 
	while (1)
	{
		
		frameCounter++;


		cap >> frame;

		if (frame.empty())
		{
			cout << "No more frames, assuming end of input." << endl;
			break;
		}


		
		pMOG2->operator()(frame, fgmask, -1);
		

		detectors->detect(frame, fgmask, frameCounter);
		
		trackers->track(detectors->getBoxes(), frame, frameCounter);
		

	
		switch (typeOfLine)
		{
		case VERTICAL:
			line(frame,
				Point(leftPoint, 0),
				Point(leftPoint, frame.size().height),
				Scalar(0, 128, 255), // oranzova
				2);
			line(frame,
				Point(middlePoint, 0),
				Point(middlePoint, frame.size().height),
				Scalar(255, 128, 0), // bledomodra
				2); 
			line(frame,
				Point(rightPoint, 0),
				Point(rightPoint, frame.size().height),
				Scalar(255, 0, 255), // fialova	
				2); 
			break;
		case HORIZONTAL:
			line(frame,
				Point(0, leftPoint),
				Point(frame.size().width, leftPoint),
				Scalar(0, 128, 255), // oranzova
				2);
			line(frame,
				Point(0, middlePoint),
				Point(frame.size().width, middlePoint),
				Scalar(255, 128, 0),
				2); // bledomodra
			line(frame,
				Point(0, rightPoint),
				Point(frame.size().width, rightPoint),
				Scalar(255, 0, 255),
				2); // fialova	
			break;
		}
			


		switch (typeOfLine)
		{
		case VERTICAL:
			putText(frame, 
				to_string(trackers->getLeftCounter()), 
				Point(20, 20), 
				cv::FONT_HERSHEY_SIMPLEX, 
				0.5, 
				Scalar(0, 255, 0), 
				2);
			putText(frame, 
				to_string(trackers->getRightCounter()),
				Point(frame.size().width - 20, 20), 
				cv::FONT_HERSHEY_SIMPLEX, 0.5, 
				Scalar(0, 255, 0), 
				2);
			break;
		case HORIZONTAL:
			putText(frame, to_string(trackers->getLeftCounter()), 
				Point(20, 20), 
				cv::FONT_HERSHEY_SIMPLEX, 
				0.5, 
				Scalar(0, 255, 0), 
				2);
			putText(frame, 
				to_string(trackers->getRightCounter()), 
				Point(20, frame.size().height - 20), 
				cv::FONT_HERSHEY_SIMPLEX, 
				0.5, 
				Scalar(0, 255, 0), 
				2);
			break;
		}
		
		/*
		string rawname = "doc";
		char filename[280];
		sprintf(filename, "C:/Users/Lenovo/Documents/Visual Studio 2013/Projects/bp/Release/outputs/%s/rgb/%d.png", rawname.c_str(), frameCounter);

		imwrite(filename, frame);

		
		sprintf(filename, "C:/Users/Lenovo/Documents/Visual Studio 2013/Projects/bp/Release/outputs/%s/bw/%d.png", rawname.c_str(), frameCounter);

		imwrite(filename, fgmask);
		
		
		imshow("People passing through a door counter", frame);
		waitKey(1);
		*/
		
		frame.release();

	}
	cout << frameCounter << endl;
}

/*
Naèíta konfiguraèný súbor,
*/
void loadConfig(char** argv)
{	
	//int leftPoint, rightPoint, middlePoint;
	string line;
	int result, size;

	
	cout << "Loading a config file..." << endl;


	ifstream file(argv[1]);
	

	if (!file.good()){
		cerr << "Wrong path to config." << '\n';		
		exit(1);
	}	

	// cesta k zdroju
	getline(file, line);
	getline(file, line);
	source = line;

	// typ detekcie
	getline(file, line);
	getline(file, line);
	getline(file, line);	
	if (!line.compare("MOVEMENT"))
	{
		typeOfDetector = MOVEMENT;
	}
	else if (!line.compare("MOVEMENT_CASCADE"))
	{
		typeOfDetector = MOVEMENT_CASCADE;
	}
	else if (!line.compare("CASCADE"))
	{
		typeOfDetector = CASCADE;
	}	
	else if (!line.compare("C4"))
	{
		typeOfDetector = C4;
	}
	else if (!line.compare("MOVEMENT_C4"))
	{
		typeOfDetector = MOVEMENT_C4;
	}
	else
	{
		cerr << "Wrong detector." << '\n';
		exit(1);
	}
	

	// typ sledovania
	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (!line.compare("TLD"))
	{
		typeOfTracker = TLD;
	}
	else if (!line.compare("CT"))
	{
		typeOfTracker = CT;
	}
	else if (!line.compare("KCF"))
	{
		typeOfTracker = KCF;
	}
	else
	{
		cerr << "Wrong tracker." << '\n';
		exit(1);
		
	}

	// left line
	getline(file, line);
	getline(file, line);
	getline(file, line);		
	if (sscanf(line.c_str(), "%d", &leftPoint) == EOF)
	{
		cerr << "Wrong left line." << '\n';
		exit(1);
	}
	//leftPoint = leftPoint;

	// middle line
	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (sscanf(line.c_str(), "%d", &middlePoint) == EOF)
	{
		cerr << "Wrong middle line." << '\n';
		exit(1);
	}
	//middlePoint = middlePoint;

	// right line
	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (sscanf(line.c_str(), "%d", &rightPoint) == EOF)
	{
		cerr << "Wrong right line." << '\n';
		exit(1);
	}
	//rightPoint = rightPoint;

	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (!line.compare("horizontal"))
	{
		typeOfLine = HORIZONTAL;
	}
	else if (!line.compare("vertical"))
	{
		typeOfLine = VERTICAL;
	}
	else
	{
		cerr << "Wrong type of line." << '\n';
		exit(1);
	}

	
	getline(file, line);
	getline(file, line);
	getline(file, line);
	/*
	ifstream file2(line);
	if (!file2.good()){
		throw std::invalid_argument("Wrong path to cascade classifier.");
	}
	*/
	cascadeClassifier = line;

	getline(file, line);
	getline(file, line);
	getline(file, line);	
	if (sscanf(line.c_str(), "%d", &minWidthBox) == EOF)
	{
		cerr << "Wrong minimal width of the box." << '\n';
		exit(1);
	}
	 
	getline(file, line);
	getline(file, line);
	getline(file, line);	
	if (sscanf(line.c_str(), "%d", &maxWidthBox) == EOF)
	{
		cerr << "Wrong maximal width of the box." << '\n';
		exit(1);
	}

	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (sscanf(line.c_str(), "%d", &minHeightBox) == EOF)
	{
		cerr << "Wrong minimal height of the box." << '\n';
		exit(1);
	}

	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (sscanf(line.c_str(), "%d", &maxHeightBox) == EOF)
	{
		cerr << "Wrong maximal height of the box." << '\n';
		exit(1);
	}
	
	file.close();

	cout << "Done." << endl;	
}


void printConfig()
{
	cout << "======================= CONFIG =======================" << endl;
	cout << "Source: \t" << source << endl;

	cout << "Detection: \t";
	switch (typeOfDetector) {
	case MOVEMENT:
		cout << "Only movement detection" << endl;
		break;
	case CASCADE:
		cout << "Only cascade - " << cascadeClassifier << endl;
		break;
	case MOVEMENT_CASCADE:
		cout << "Movement detection with cascade - " << cascadeClassifier << endl;
		break;	
	}

	cout << "Tracking: \t";
	switch (typeOfTracker) {
	case TLD:
		cout << "TLD tracker" << endl;
		break;
	case CT:
		cout << "Compressive tracking" << endl;
		break;
	case KCF:
		cout << "Kernelized correlation filters" << endl;
		break;
	}

	cout << "Width box: \t" << minWidthBox << " < " << "width box" << " < " << maxWidthBox << endl;
	cout << "Height box: \t" << minHeightBox << " < " << "height box" << " < " << maxHeightBox << endl;
	cout << "=====================================================" << endl;
}

/* 
string getFileName()
{
	string detector, tracker;
	switch(typeOfDetector) {
	case MOVEMENT:
		//cout << "Only movement detection" << endl;
		detector = "Only movement detection";
		break;
	case CASCADE:
		//cout << "Only cascade - " << cascadeClassifier << endl;
		detector = "Only cascade";
		break;
	case MOVEMENT_CASCADE:
		//cout << "Movement detection with cascade - " << cascadeClassifier << endl;
		detector = "Movement detection with cascade";
		break;
	
	case C4:
		detector = "C4";
		break;
	case MOVEMENT_C4:
		detector = "Movement C4";
		break;
	}

	//cout << "Tracking: \t";
	switch (typeOfTracker) {
	case TLD:
		//cout << "TLD tracker" << endl;
		tracker = "TLD";
		break;
	case CT:
		//cout << "Compressive tracking" << endl;
		tracker = "CT";
		break;
	case KCF:
		//cout << "Kernelized correlation filters" << endl;
		tracker = "KCF";
		break;
	}

	char filename[200];
	sprintf(filename, "menza_%s_%s_count%d_com%d.txt", detector.c_str(), tracker.c_str(), countingThresh, compareThresh);
	cout << filename << '\n';
	return filename;
}
*/