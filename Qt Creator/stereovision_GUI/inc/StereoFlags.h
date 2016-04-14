/*
 * StereoFlags.h
 *
 *  Created on: Dec 3, 2015
 *      Author: nicolasrosa
 */

#ifndef STEREOFLAGS_H
#define STEREOFLAGS_H

/* Libraries */

class StereoFlags{
public:
    /* Constructor and Destructor */
    StereoFlags();
    ~StereoFlags();

    bool showInputImages;
    bool showXYZ;
    bool showStereoParam;
    bool showStereoParamValues;
    bool showFPS;
    bool showDisparityMap;
    bool show3Dreconstruction;
    bool showTrackingObjectView;
    bool showDiffImage;
    bool showWarningLines;
    bool showHistograms;
    bool showDispDepth;
    bool showLeftOnRightWindow;
};

#endif // STEREOFLAGS_H
