/*
 * mainwindow.h
 *
 *  Created on: Oct 1, 2015
 *      Author: nicolasrosa
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/* Custom Libraries */
#include "inc/main.h"
#include "inc/MainWindow.h"
#include "inc/StereoProcessor.h"
#include "inc/StereoSetParamsWindow.h"

using namespace cv;
using namespace std;

namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    /* Constructor and Destructor */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /* App Configuration */
    void uiConfiguration();
    void timerConfiguration();
    void setupUi_Custom();

    void stereoVisionProcessInit();

    void printHelp();
    void openStereoSource(int inputNum);
    void deleteStereoObj();

    //TODO: Arrumar escopo da funçao
    void pcl_function();

    /* App TextBox */
    void uiText1();

    /* App Windows */
    void updateDisplayWindows();
    void putImage(const Mat& src,int windowID);

    QImage Mat2QImage(const Mat& mat);

    enum{LEFT_WINDOW,RIGHT_WINDOW};

private:
    Ui::MainWindow  *ui;
    StereoProcessor *stereo;
    SetStereoParams *stereoParamsSetupWindow;

    QImage qimageL,qimageR;
    QTimer* tmrTimer;

    bool closeEventOccured;
    bool isTrackingObjects;

signals:

public slots:
    void stereoVisionProcess_UpdateGUI();

private slots:
    void on_btnPauseOrResume_clicked();
    void on_btnShowDisparityMap_clicked();
    void on_btnShowStereoParamSetup_clicked();
    void on_btnShow3DReconstruction_clicked();
    void on_btnShowInputImages_clicked();
    void on_btnShowTrackingObjectView_clicked();
    void on_btnShowDiffImage_clicked();
    void on_btnShowWarningLines_clicked();
    void on_toggleBtnShowHist_clicked(bool checked);
    void on_toggleBtnShowXYZ_toggled(bool checked);
    void on_toggleBtnShowDispDepth_toggled(bool checked);
    void on_methodSelector_activated(int index);
    void on_inputSelector_activated(int index);
    void on_toggleBtnShowLeftImage_toggled(bool checked);

    void on_toggleBtnShowOverlay_toggled(bool checked);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

