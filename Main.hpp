/*
 * Poèítání prùchodù osob dveømi s využitím stacionární kamery
 * Bakalárska práca, 2016
 * ¼uboš Tichý, xtichy23
 * Main.hpp
 */

#ifndef MAIN
#define MAIN

#include <string>
#include <opencv2/opencv.hpp>



std::string source;

int typeOfDetector;
int typeOfTracker;

int typeOfLine;

int leftPoint;
int middlePoint;
int rightPoint;

std::string cascadeClassifier;

int minWidthBox;
int maxWidthBox;

int minHeightBox;
int maxHeightBox;

const int VERTICAL = 1;
const int HORIZONTAL = 2;


/* detekcia zalozena len pohybe */
const int MOVEMENT = 1;

/* detekcia zalozena na kaskade s vyuzitim pohybu */
const int MOVEMENT_CASCADE = 2;

/* detekcia s kaskadou */
const int CASCADE = 3;

/* detektor s vyuzitim Houghovej transformacie */
const int HOUGH_TRANSFORM = 4;

/* detektor s vyuzitim Houghovej transformacie a s vyuzitim pohybu */
//const int MOVEMENT_HOUGH_TRANSFORM = 5;

/*detektor metodou C4 */
const int C4 = 6;

const int MOVEMENT_C4 = 7;
/* TLD tracker */
const int TLD = 1;

/* Compressive tracking */
const int CT = 2;

/* Kernelized correlation filters */
const int KCF = 3;


/* vypise napovedu */
void printHelp();

/* nahra konfiguracne nastavenia */
void loadConfig(char**);

/* vypise nastavenia */
void printConfig();

/* samotny proces pocitania */
void doWork();


//std::string getFileName();

int countingThresh, compareThresh;


#endif