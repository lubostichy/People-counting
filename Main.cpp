
#include <iostream>
#include "src\Cascade_BackgroundSubtraction\VJ_BS.h"
#include "src\Cascade\CC.h"
#include "src\HoughTransform\HT.h"
#include "Main.h"
#include <opencv2\opencv.hpp>
#include "src\OpenTLD\OpenTLD.h"
#include "src\Box.h"
#include "src\BackgroundSubtraction\BS.h"
#include "src\CompressiveTracking\RunCT.h"
#include "src\Kernelized_Correlation_Filters\KCF.h"


using namespace cv;
using namespace std;

void printHelp()
{
	cout << "Run as: bp.exe [config file]" << endl;
}


/* 
 * MAIN 
 */
int main(int argc, char** argv)
{
	
	struct Config config;
	

	// Spracovanie parametrov
	if (argc == 2)
	{
		config = getConfig(argv);
		if (!config.ok)
		{
			cerr << "Error: Wrong config file." << endl;
			return EXIT_FAILURE;
		}
	}
	else if (argc == 1)
	{
		cerr << "Error: Specify path to config file." << endl;
		return EXIT_FAILURE;
	}
	else
	{
		cerr << "Error: Wrong params." << endl;
		printHelp();
		return EXIT_FAILURE;
	}

	printConfig(config);

	cout << "Counting..." << endl;
	cout << "============================================================" << endl;

	doWork(config);

	cout << "============================================================" << endl;
	cout << "Counting is done." << endl;
	//cout << "Press enter..." << endl;
	//getchar();
    return EXIT_SUCCESS;
}


void doWork(struct Config config)
{
	int frameCounter = 0;
	Mat frame;
	Mat fgmask;
	Mat gray;
	VideoCapture cap;

	VJ_BS vj_bs;
	HT ht;
	OpenTLD tld;
	
	RunCT ct;
	BS bs;
	CC cc;
	RunKCF kcf;


	vector <Box> boxes;
	BackgroundSubtractorMOG2 *pMOG2;
	string output, input;
	unsigned int leftCounter = 0;
	unsigned int rightCounter = 0;
	

	cout << "Opening source... " << endl;
	cap.open(config.source);

	if (!cap.isOpened())
	{
		cerr << "Cannot open source." << endl;
		exit(1);
	}
	cout << "Done." << endl;

	
	pMOG2 = new BackgroundSubtractorMOG2(0, 16.0, true);
	// nastavenie detektora
	switch (config.detector)
	{
	case MOVEMENT:
		bs.setBS(config.widthOfBox, config.heightOfBox);
		break;
	case MOVEMENT_CASCADE:
		vj_bs.setVJ_BS(config);
		break;
	case CASCADE:
		//cc.setCC(config.widthOfBox, config.heightOfBox, config.cascadeClassifier, config.typeOfLine, config.leftPoint, config.middlePoint, config.rightPoint);
		cc.setCC(config);
		//pMOG2 = new BackgroundSubtractorMOG2(0, 64, false);
		break;
	case HOUGH_TRANSFORM:		
		ht.setDetector(config.widthOfBox, config.heightOfBox, config.typeOfLine, config.leftPoint, config.rightPoint);
		break;
	case MOVEMENT_HOUGH_TRANSFORM:
		bs.setBS(config.widthOfBox, config.heightOfBox);
		ht.setDetector(config.widthOfBox, config.heightOfBox, config.typeOfLine, config.leftPoint, config.rightPoint);
		break;
	}

	switch (config.tracker)
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


		switch (config.detector)
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

		
		//if (boxes.size() != 0)
		
		
		switch (config.tracker)
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
		
		
	
		switch (config.typeOfLine)
		{
		case VERTICAL:
			line(frame,
				Point(config.leftPoint, 0),
				Point(config.leftPoint, frame.size().height),
				Scalar(0, 128, 255), // oranzova
				2);
			line(frame,
				Point(config.middlePoint, 0),
				Point(config.middlePoint, frame.size().height),
				Scalar(255, 128, 0),
				2); // bledomodra
			line(frame,
				Point(config.rightPoint, 0),
				Point(config.rightPoint, frame.size().height),
				Scalar(255, 0, 255),
				2); // fialova	
			break;
		case HORIZONTAL:
			line(frame,
				Point(0, config.leftPoint),
				Point(frame.size().width, config.leftPoint),
				Scalar(0, 128, 255), // oranzova
				2);
			line(frame,
				Point(0, config.middlePoint),
				Point(frame.size().width, config.middlePoint),
				Scalar(255, 128, 0),
				2); // bledomodra
			line(frame,
				Point(0 ,config.rightPoint),
				Point(frame.size().width, config.rightPoint),
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

		putText(frame, to_string(leftCounter), Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
		putText(frame, to_string(rightCounter), Point(frame.size().width - 20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
			
		/*
		string rawname = "menza";
		char filename[280];
		sprintf(filename, "C:/Users/Lenovo/Documents/Visual Studio 2013/Projects/bp/Release/outputs/%s/rgb/%d.png", rawname.c_str(), frameCounter);

		imwrite(filename, frame);
		*/

		imshow("People passing through a door counter", frame);
		waitKey(1);
		frame.release();

	}
}


struct Config getConfig(char** argv)
{
	struct Config config;
	int leftPoint, rightPoint, middlePoint;

	config.ok = true;
	config.mode = 0;
	config.detector = 0;
	config.tracker = 0;
	config.source = "";

	cout << "Loading config file..." << endl;

	ifstream file(argv[1]);
	string line;
	int result, size;

	if (!file.good()){
		cerr << "Error: Wrong path to config." << endl;
		config.ok = false;
		return config;
	}	

	// cesta k zdroju
	getline(file, line);
	getline(file, line);
	config.source = line;

	// typ detekcie
	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (!line.compare("MOVEMENT"))
	{
		config.detector = MOVEMENT;
	}
	else if (!line.compare("MOVEMENT_CASCADE"))
	{
		config.detector = MOVEMENT_CASCADE;
	}
	else if (!line.compare("CASCADE"))
	{
		config.detector = CASCADE;
	}
	else if (!line.compare("HOUGH_TRANSFORM"))
	{
		config.detector = HOUGH_TRANSFORM;
	}
	else if (!line.compare("MOVEMENT_HOUGH_TRANSFORM"))
	{
		config.detector = MOVEMENT_HOUGH_TRANSFORM;
	}
	else
	{
		cerr << "Error: Wrong detector." << endl;
		config.ok = false;
		return config;
	}

	// typ sledovania
	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (!line.compare("TLD"))
	{
		config.tracker = TLD;
	}
	else if (!line.compare("CT"))
	{
		config.tracker = CT;
	}
	else if (!line.compare("KCF"))
	{
		config.tracker = KCF;
	}
	else
	{
		cerr << "Error: Wrong tracker." << endl;
		config.ok = false;
		return config;
	}

	// left line
	getline(file, line);
	getline(file, line);
	getline(file, line);
		
	result = sscanf(line.c_str(), "%d", &leftPoint);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}
	config.leftPoint = leftPoint;

	// middle line
	getline(file, line);
	getline(file, line);
	getline(file, line);

	result = sscanf(line.c_str(), "%d", &middlePoint);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}
	config.middlePoint = middlePoint;

	// left line
	getline(file, line);
	getline(file, line);
	getline(file, line);

	result = sscanf(line.c_str(), "%d", &rightPoint);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}
	config.rightPoint = rightPoint;

	getline(file, line);
	getline(file, line);
	getline(file, line);
	if (!line.compare("horizontal"))
	{
		config.typeOfLine = HORIZONTAL;
	}
	else if (!line.compare("vertical"))
	{
		config.typeOfLine = VERTICAL;
	}
	else
	{
		config.ok = false;
		return config;
	}

	
	getline(file, line);
	getline(file, line);
	getline(file, line);
	config.cascadeClassifier = line;

	getline(file, line);
	getline(file, line);
	getline(file, line);
	
	
	result = sscanf(line.c_str(), "%d", &config.minWidthBox);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}
	 
	getline(file, line);
	getline(file, line);
	getline(file, line);


	result = sscanf(line.c_str(), "%d", &config.maxWidthBox);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}

	getline(file, line);
	getline(file, line);
	getline(file, line);

	
	result = sscanf(line.c_str(), "%d", &config.minHeightBox);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}

	getline(file, line);
	getline(file, line);
	getline(file, line);


	result = sscanf(line.c_str(), "%d", &config.maxHeightBox);
	if (result == EOF)
	{
		config.ok = false;
		return config;
	}
	
	// TO DO: tresholdy

	file.close();

	cout << "Done." << endl;

	return config;
}

void printConfig(struct Config config)
{
	cout << "======================= CONFIG =======================" << endl;
	cout << "Source: \t" << config.source << endl;

	cout << "Detection: \t";
	switch (config.detector) {
	case MOVEMENT:
		cout << "Only movement detection" << endl;
		break;
	case CASCADE:
		cout << "Only cascade - " << config.cascadeClassifier << endl;
		break;
	case MOVEMENT_CASCADE:
		cout << "Movement detection with cascade - " << config.cascadeClassifier << endl;
		break;
	case HOUGH_TRANSFORM:
		cout << "Hough transform" << endl;
		break;
	case MOVEMENT_HOUGH_TRANSFORM:
		cout << "Hough transform with movement detection" << endl;
		break;
	}

	cout << "Tracking: \t";
	switch (config.tracker) {
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

	cout << "Width box: \t" << config.minWidthBox << " < " << "width box" << " < " << config.maxWidthBox << endl;
	cout << "Height box: \t" << config.minHeightBox << " < " << "height box" << " < " << config.maxHeightBox << endl;
	cout << "=====================================================" << endl;
}