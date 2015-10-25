/*
 * StereoProcessor.cpp
 *
 *  Created on: Oct 20, 2015
 *      Author: nicolasrosa
 */

#ifndef STEREOPROCESSOR_H
#define STEREOPROCESSOR_H

/* Libraries */
#include "opencv2/opencv.hpp"
#include "3DReconstruction.h"

using namespace cv;
using namespace std;

/* Custom Classes */
class StereoCalib{
public:
    StereoCalib(); //Constructor

    string intrinsicsFileName;
    string extrinsicsFileName;
    string QmatrixFileName;

    Mat K,Q;
    double focalLength;
    double baseline;
    bool is320x240;
    bool is640x480;
    bool is1280x720;

    Mat M1,D1,M2,D2;
    Mat R,T,R1,P1,R2,P2;
    Rect roi1, roi2;
    bool isKcreated;
};

class StereoProcessor{
public:
    StereoProcessor(int inputNum); //Constructor
    int getInputNum();
    void readConfigFile();
    void stereoInit();
    void stereoCalib();
    void createKMatrix();
    void readQMatrix();
    void calculateQMatrix();

    Ptr<StereoBM> bm;
    StereoCalib calib;
    Reconstruction3D view3D;
    Size imageSize;
    Point2d imageCenter;
private:
    int inputNum;
};


#endif // STEREOPROCESSOR_H
