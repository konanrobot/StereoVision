/*
 * StereoFlags.h
 *
 *  Created on: Dec 3, 2015
 *      Author: nicolasrosa
 */

#ifndef STEREOFLAGS_H
#define STEREOFLAGS_H

class StereoFlags{
public:
    StereoFlags(); //Constructor

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
};

#endif // STEREOFLAGS_H
