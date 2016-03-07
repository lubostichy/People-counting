
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


void printHelp();
struct Config getConfig(char**);
void printConfig(struct Config);
void doWork(struct Config);

const int VERTICAL = 1;
const int HORIZONTAL = 2;

/* typy detekcie */

/* len pohyb */
const int MOVEMENT = 1;

/* pohyb s detektorom */
const int MOVEMENT_CASCADE = 2;

/* detekcia */
const int CASCADE = 3;

/* detektor s vyuzitim Houghovej transformacie */
const int HOUGH_TRANSFORM = 4;

/* detektor s pohybom s vyuzitime Houghovej transformacie */
const int MOVEMENT_HOUGH_TRANSFORM = 5;


/* TLD tracker */
const int TLD = 1;

/* Compressive tracking */
const int CT = 2;

/* Kernelized correlation filters */
const int KCF = 3;


/* nastavenia systemu */
struct Config {
	bool ok;
	int mode;
	string source;
	int detector;
	int tracker;
	//Point point1;
	//Point point2;
	int typeOfLine;
	int leftPoint;
	int middlePoint;
	int rightPoint;
	string cascadeClassifier;
	int minWidthBox;
	int maxWidthBox;
	int widthOfBox;
	int minHeightBox;
	int maxHeightBox;
	int heightOfBox;
	// TO DO: + tresholdy
};
