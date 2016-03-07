/*
 * StereoMorphology.h
 *
 *  Created on: Feb 29, 2016
 *      Author: nicolasrosa
 */

/* Libraries */
#include <opencv2/opencv.hpp>

#ifndef STEREOMORPHOLOGY_H
#define STEREOMORPHOLOGY_H

/* Threshold, Erosion, Dilation and Blur Constants */
#define THRESH_VALUE   100
#define EROSION_SIZE     5
#define DILATION_SIZE    5
#define BLUR_SIZE        3

using namespace cv;
using namespace std;

class StereoMorphology{
public:
    /* Constructor and Destructor */
    StereoMorphology();
    ~StereoMorphology();

    void imageProcessing(Mat src,Mat trackingView,bool isTrackingObjects,bool isVideoFile);
    void Disp_diff(Mat disp8U,Mat disp87_last,Mat disp8U_diff);
    //void cornerHarris_demo(int,void*);

    /* Dilation and Erosion Elements */
    Mat erosionElement;
    Mat dilationElement;

    /* Tracking Features Variables */
    int x,y;

    /* Parcial Results */
    Mat imgE ,imgEBGR;
    Mat imgED,imgEDBGR;
    Mat imgEDMedian;
    Mat imgEDMedianBGR;

    /* Results */
    Mat imgThreshold;
    Mat imgThresholdDraw;
    Mat trackingView;
};

#endif // STEREOMORPHOLOGY_H
