
#include <iostream>
//#include "src\Cascade_BackgroundSubtraction\VJ_BS.h"
//#include "src\Cascade\CC.h"
//#include "src\HoughTransform\HT.h"
#include "Main.hpp"
#include <opencv2\opencv.hpp>
//#include "src\OpenTLD\OpenTLD.h"
#include "src\Box.h"
//#include "src\BackgroundSubtraction\BS.h"
//#include "src\CompressiveTracking\RunCT.h"
//#include "src\Kernelized_Correlation_Filters\KCF.h"
#include "src\Detectors.hpp"
#include "src\Trackers.hpp"


using namespace cv;
using namespace std;

/* 
 * MAIN
 */
int main(int argc, char** argv)
{
	
	/*
	try 
	{
		ret = runMain(argc, argv);
	}
	catch (std::exception & e)
	{
		cout << "\033[1;31mA std::exception was raised:\033[0m " << e.what() <<  '\n';
		ret = EXIT_FAILURE;
		throw;
	}
	catch (...)
	{
		cout << "\033[1;31mAn unknown exception was raised\033[0m " << endl;
		ret = EXIT_FAILURE; 
		throw;
	}
	*/

	if (argc == 2)
	{
		loadConfig(argv);
	}
	else
	{
		cerr << "Specify path to config file." << endl;
		printHelp();
		return EXIT_FAILURE;
	}


	//	printConfig();

	cout << "Counting..." << endl;
	cout << "============================================================" << endl;

	doWork();

	cout << "============================================================" << endl;
	cout << "Counting is done." << endl;
	//cout << "Press enter..." << endl;
	//getchar();
	return EXIT_SUCCESS;
}

void printHelp()
{
	cout << "Run as: bp.exe [config file]" << endl;
}

/*
int runMain(int argc, char** argv)
{

	// Spracovanie parametrov
	if (argc == 2)
	{
		loadConfig(argv);		
	}
	else 
	{
		cerr << "Specify path to config file." << endl;
		printHelp();
		return EXIT_FAILURE;
	}
	

//	printConfig();

	cout << "Counting..." << endl;
	cout << "============================================================" << endl;

	doWork();

	cout << "============================================================" << endl;
	cout << "Counting is done." << endl;
	//cout << "Press enter..." << endl;
	//getchar();
    return EXIT_SUCCESS;
}
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

	//HT ht;
	/*
	OpenTLD tld;	
	RunCT ct;
	RunKCF kcf;
	*/

	/*
	BS bs(typeOfLine,
		leftPoint, middlePoint, rightPoint,
		minWidthBox, maxWidthBox,
		minHeightBox,maxHeightBox);

	CC cc(typeOfLine,
		leftPoint, middlePoint, rightPoint,
		minWidthBox, maxWidthBox,
		minHeightBox, maxHeightBox);

	VJ_BS vj_bs;
	*/

	


	//vector <Box> boxes;
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
	/*
	// nastavenie detektora
	switch (typeOfDetector)
	{
	case MOVEMENT:
		//bs.setBS(config);
		break;
	case MOVEMENT_CASCADE:
		//vj_bs.setVJ_BS(config);
		break;
	case CASCADE:
		//cc.setCC(config.widthOfBox, config.heightOfBox, config.cascadeClassifier, config.typeOfLine, config.leftPoint, config.middlePoint, config.rightPoint);
		//cc.setCC(config);
		cc.loadClassifier(cascadeClassifier);
		//pMOG2 = new BackgroundSubtractorMOG2(0, 64, false);
		break;
	case HOUGH_TRANSFORM:		
		ht.setDetector(config);
		break;
	case MOVEMENT_HOUGH_TRANSFORM:
		//bs.setBS(config);
		ht.setDetector(config);
		break;
	}
	*/
	/*
	switch (typeOfTracker)
	{
	case TLD:
		tld.setCounter(config);
		break;
	case CT:
		ct.setCounter(config);
		break;
	case KCF:
		kcf.setCounter(config);
		break;
	}
	*/

	/* // pre prepisanie rovnakeho riadku v konzole
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(h, &bufferInfo);
	*/
	int size;
	Mat data;
	Mat plot_result;

	// hlavny cyklus 
	while (1)
	{
		//SetConsoleCursorPosition(h, bufferInfo.dwCursorPosition);
		frameCounter++;

		/*
		if (frameCounter % 100 == 0)
			cout << frameCounter << ". frame"<< endl;
		*/
		

		cap >> frame;

		if (frame.empty())
		{
			cout << "No more frames, assuming end of input." << endl;
			break;
		}

		//cvtColor(frame, gray, CV_RGB2GRAY);
		pMOG2->operator()(frame, fgmask, -1);

		//if (frameCounter < 900)
		//	continue;

		detectors->detect(frame, fgmask, frameCounter);
		//boxes = ;
		trackers->track(detectors->getBoxes(), frame, frameCounter);
		/*
		switch (typeOfDetector)
		{
		case MOVEMENT:
			bs.setFrames(fgmask, frame);
			bs.detect();
			boxes = bs.boxes;
			bs.boxes.clear();
			break;
		case MOVEMENT_CASCADE:			
			vj_bs.setFrames(fgmask, frame);
			vj_bs.detect();
			boxes = vj_bs.boxes;
			vj_bs.boxes.clear();
			break;
		case CASCADE:
			cc.setFrame(frame);
			cc.detect();
			boxes = cc.boxes;
			cc.boxes.clear();
			break;
		case HOUGH_TRANSFORM:
			ht.setRGBFrame(frame);
			ht.detect();
			boxes = ht.boxes;
			for (unsigned int i = 0; i < boxes.size(); i++)
			{
				rectangle(frame, boxes[i].bbox, Scalar(0, 255, 0), 2);
			}
			ht.boxes.clear();
			break;
		case MOVEMENT_HOUGH_TRANSFORM:
			bs.setFrames(fgmask, frame);
			bs.findBoundingBoxes();
			//bs.detect(); // detekuje pohyb
			ht.setRGBFrame(frame);

			// na kazdom boxe pohybu sa detekuje osoba
			for (unsigned int i = 0; i < bs.boundingBoxes.size(); i++)
			{
				ht.detect(bs.boundingBoxes[i]);
			}
			boxes = ht.boxes;
			// boxy detekcie osoby
			for (unsigned int i = 0; i < boxes.size(); i++)
			{
				rectangle(frame, boxes[i].bbox, Scalar(0, 255, 0), 2);
			}
			ht.gd.boxes.clear();
			bs.boundingBoxes.clear();

			break;
		}
		*/

		
		//if (boxes.size() != 0)
		
		/*
		switch (typeOfTracker)
		{
		case TLD:
			
			for (vector<int>::size_type i = boxes.size() - 1; i != (vector<int>::size_type) - 1; i--)
			{
				boxes[i].setFrameNO(frameCounter);
				tld.addBox(boxes[i]);
				boxes.pop_back();
			}

			tld.setFrames(frame, frameCounter, leftCounter, rightCounter);

			tld.track();
			tld.initTracking();
			leftCounter = tld.getLeftCounter();
			rightCounter = tld.getRightCounter();
			
			break;
		case CT:
			for (vector<int>::size_type i = boxes.size() - 1; i != (vector<int>::size_type) - 1; i--)
			{
				boxes[i].setFrameNO(frameCounter);
				ct.addBox(boxes[i]);
				boxes.pop_back();

			}
			ct.setFrames(frame, frameCounter, leftCounter, rightCounter);

			ct.track();
			ct.initTracking();
			leftCounter = ct.getLeftCounter();
			rightCounter = ct.getRightCounter();
			break;
		case KCF:
			for (vector<int>::size_type i = boxes.size() - 1; i != (vector<int>::size_type) - 1; i--)
			{
				boxes[i].setFrameNO(frameCounter);
				kcf.addBox(boxes[i]);
				boxes.pop_back();
			}
			kcf.setFrames(frame, frameCounter, leftCounter, rightCounter);
			kcf.track();
			kcf.initTracking();
			leftCounter = kcf.getLeftCounter();
			rightCounter = kcf.getRightCounter();
			break;
		}
		*/

	
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
			
		

		///// debugging
		
		

		/*
		sprintf(filename, "C:/Users/Lenovo/Documents/Visual Studio 2013/Projects/bp/Release/outputs/%s/bw/%d.png", rawname.c_str(), frameCounter);
		//cout << filename << endl;
		imwrite(filename, fgmask);
		*/
		/////

		switch (typeOfLine)
		{
		case VERTICAL:
			putText(frame, to_string(trackers->getLeftCounter()), Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
			putText(frame, to_string(trackers->getRightCounter()), Point(frame.size().width - 20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
			break;
		case HORIZONTAL:
			putText(frame, to_string(trackers->getLeftCounter()), Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
			putText(frame, to_string(trackers->getRightCounter()), Point(20, frame.size().height - 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
			break;
		}
		
		/*
		string rawname = "vratnica";
		char filename[280];
		sprintf(filename, "C:/Users/Lenovo/Documents/Visual Studio 2013/Projects/bp/Release/outputs/%s/rgb/%d.png", rawname.c_str(), frameCounter);

		imwrite(filename, frame);
		*/

		imshow("People passing through a door counter", frame);
		waitKey(1);
		frame.release();

	}
}


void loadConfig(char** argv)
{	
	//int leftPoint, rightPoint, middlePoint;
	string line;
	int result, size;

	/*
	config.ok = true;
	config.mode = 0;
	config.detector = 0;
	config.tracker = 0;
	config.source = "";
	*/
	cout << "Loading config file..." << endl;


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
	else if (!line.compare("HOUGH_TRANSFORM"))
	{
		typeOfDetector = HOUGH_TRANSFORM;
	}
	else if (!line.compare("MOVEMENT_HOUGH_TRANSFORM"))
	{
		typeOfDetector = MOVEMENT_HOUGH_TRANSFORM;
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
	case HOUGH_TRANSFORM:
		cout << "Hough transform" << endl;
		break;
	case MOVEMENT_HOUGH_TRANSFORM:
		cout << "Hough transform with movement detection" << endl;
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
