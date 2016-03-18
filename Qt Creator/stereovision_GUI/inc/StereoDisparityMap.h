/*
 * StereoDisparityMap.h
 *
 *  Created on: Dec 3, 2015
 *      Author: nicolasrosa
 */

#ifndef STEREODISPARITYMAP_H
#define STEREODISPARITYMAP_H

/* Libraries */
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class StereoDisparityMap{
public:
    /* Constructor and Destructor */
    StereoDisparityMap();
    ~StereoDisparityMap();

    /* Raw Disparities Maps */
    Mat disp_16S;
    Mat disp_8U;
    Mat disp_BGR;

    Mat disp_8U_last;
    Mat disp_8U_diff;

    /* Image Processing Results */
    //Mat disp_8U_eroded;
    //Mat disp_8U_eroded_dilated;
};

#endif // STEREODISPARITYMAP_H