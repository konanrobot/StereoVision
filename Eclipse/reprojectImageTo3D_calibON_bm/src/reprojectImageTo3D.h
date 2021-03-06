/*
 * reprojectImageTo3D.h
 *
 *  Created on: Jun 18, 2015
 *	Author: nicolasrosa
 */

#ifndef reproject_Image_To_3D_LIB_H_
#define reproject_Image_To_3D_LIB_H_

/* Libraries */
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#include "stdio.h"
#include "opencv2/opencv.hpp"
#include "trackObject.h"

using namespace cv;
using namespace std;

/* Calibration */
#define RESOLUTION_640x480
//#define RESOLUTION_1280x720
#define CALIBRATION_ON

/* Threshold, Erosion, Dilation and Blur Constants */
#define THRESH_VALUE 100
#define EROSION_SIZE 5  //SAR
#define DILATION_SIZE  5	//SAR
#define BLUR_SIZE 3

/* Custom Classes*/
class StereoProcessor{
public:
    StereoProcessor(int inputNum); //Constructor
    int getInputNum();

private:
    int inputNum;
    //float percentageOfDeletion;
};

StereoProcessor::StereoProcessor(int number){
    inputNum=number;
}

int StereoProcessor::getInputNum(){
    return inputNum;
}

class ConfigFile{
public:
	string intrinsicsFileName;
	string extrinsicsFileName;
	string QmatrixFileName;
public:
	void readConfigFile(ConfigFile *cfg);
};

void ConfigFile::readConfigFile(ConfigFile* cfg){
	FileStorage fs("config.yml", FileStorage::READ);
    if(!fs.isOpened()){
        printf("Failed to open config.yml file.\n");
        return;
    }
    fs["Intrinsics Path"] >> cfg->intrinsicsFileName;
    fs["Extrinsics Path"] >> cfg->extrinsicsFileName;
    fs["Q Matrix Path"]   >> cfg->QmatrixFileName;

    fs.release();

    cout << "Intrinsics Path: " << cfg->intrinsicsFileName << endl;
    cout << "Extrinsics Path: " << cfg->extrinsicsFileName << endl;
    cout << "Q Matrix Path: "   << cfg->QmatrixFileName    << endl;
    cout << "Config.yml Read Successfully." << endl << endl ;
}

/* Functions Scope */
void on_trackbar(int,void*);
void createTrackbars();

void printHelp();
void openStereoSource(int inputNum,VideoCapture* capL,VideoCapture* capR,Mat* imageL,Mat* imageR);
void stereoInit(StereoBM* bm);
void stereoCalib(Mat &M1,Mat &D1,Mat &M2,Mat &D2,Mat &R,Mat &T,ConfigFile* cfg);
void stereoSetparams(Rect* roi1,Rect* roi2,StereoBM* bm,int numRows,bool showStereoBMparams);
void readQMatrix(Mat &Q,double* focalLength, double* baseline,ConfigFile* cfg);
void calculateQMatrix(Mat &Q,Point2d imageCenter,double focalLength, double baseline);
void imageProcessing1(Mat img, Mat imgMedian, Mat imgMedianBGR);
void imageProcessing2(Mat src, Mat imgE, Mat imgED,Mat cameraFeedL,bool isTrackingObjects);

void resizeFrame(Mat* frame1,Mat* frame2);
void changeResolution(VideoCapture* cap_l,VideoCapture* cap_r);
void contrast_and_brightness(Mat &left,Mat &right,float alpha,float beta);

/* 3D Reconstruction Functions */
void eular2rot(double yaw,double pitch, double roll,Mat& dest);
void lookat(Point3d from, Point3d to, Mat& destR);
void projectImagefromXYZ(Mat &image, Mat &destimage, Mat &disp, Mat &destdisp, Mat &xyz, Mat &R, Mat &t, Mat &K, Mat &dist, bool isSub);
void fillOcclusion(Mat& src, int invalidvalue, bool isInv);

/* 3D Reconstruction Classes */
template <class T>
static void projectImagefromXYZ_(Mat& image, Mat& destimage, Mat& disp, Mat& destdisp, Mat& xyz, Mat& R, Mat& t, Mat& K, Mat& dist, Mat& mask, bool isSub);

template <class T>
static void fillOcclusionInv_(Mat& src, T invalidvalue);

template <class T>
static void fillOcclusion_(Mat& src, T invalidvalue);

/* Global Variables */
const string trackbarWindowName = "Stereo Param Setup";
const double focal_length = 752.093;
const double baseline = -2.61138;
bool isVideoFile=false,isImageFile=false,needCalibration=false,isTrackingObjects=true;
bool showInputImage=true,showXYZ=false,showStereoParams=false,showFPS=false,showDisparityMap=false,show3Dreconstruction=false,showDiffImage=false;

/* Trackbars Variables
 * Initial min and max BM Parameters values.These will be changed using trackbars
 */
int preFilterSize			 = 50;	const int preFilterSize_MAX		 	= 100;
int preFilterCap			 = 100;	const int preFilterCap_MAX		 	= 100;
int SADWindowSize			 = 25;	const int SADWindowSize_MAX		 	= 100;
int minDisparity			 = 47;	const int minDisparity_MAX		 	= 100;
int numberOfDisparities		 = 3;	const int numberOfDisparities_MAX 	= 16;
int textureThreshold		 = 30;	const int textureThreshold_MAX		= 100;
int uniquenessRatio			 = 0;	const int uniquenessRatio_MAX		= 100;
int speckleWindowSize		 = 0;	const int speckleWindowSize_MAX	 	= 100;
int speckleRange			 = 0;	const int speckleRange_MAX		 	= 100;
int disp12MaxDiff			 = 1;	const int disp12MaxDiff_MAX		 	= 1;

#endif /* reproject_Image_To_3D_LIB_H_ */
