/* Project: reprojectImageTo3D - BlockMatching Algorithm
 * mainwindow.cpp
 *
 *  Created on: June, 2015
 *      Author: nicolasrosa
 *  Credits to:
 * Anonymous Author: Credits: http://opencv.jp/opencv2-x-samples/point-cloud-rendering
 * Kyle Hounslow - https://www.youtube.com/watch?v=bSeFrPrqZ2A
 * helicopters*.png - Icons made by Freepik from www.flaticon.com - Link: http://www.flaticon.com/free-icon/drone_90894#term=drone&page=1&position=11
 * spydrone*.png - Icon made by Aha-Soft from Noun Project
 */

/* Libraries */
#include <QtCore>
#include <opencv2/imgproc/imgproc.hpp>

/* Custom Libraries */
#include "reprojectImageTo3D.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QShortcut"

using namespace cv;
using namespace std;

/* Constructor */
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    setupUi_Custom();

    this->stereo = new StereoProcessor(1);
    StereoVisionProcessInit();

    tmrTimer = new QTimer(this);
    connect(tmrTimer,SIGNAL(timeout()),this,SLOT(StereoVisionProcess_UpdateGUI()));
    tmrTimer->start(20);

    closeEventOccured = false;
}

MainWindow::~MainWindow(){
    delete stereo;
    delete ui;

    tmrTimer->stop();
    closeEventOccured = true;
}

//static MainWindow& getUi()
//{
//    static MainWindow myUi;
//    return myUi;
//}

void MainWindow::setupUi_Custom(){
    setWindowIcon(QIcon(":/icons/icon/spydrone_black.png"));

    /* Allows the following pushButtons to toggle - A non-checkable button never emits the toggled(bool) signal. */
    ui->toggleBtnShowHist->setCheckable(true);
    ui->toggleBtnShowXYZ->setCheckable(true);
    ui->toggleBtnShowDispDepth->setCheckable(true);

    ui->toggleBtnShowHist->hide();
    ui->toggleBtnShowXYZ->hide();
    ui->toggleBtnShowDispDepth->hide();

    ui->statusBar->showMessage("Running...");

    /* */
    //QShortcut *shortcut = new QShortcut(QKeySequence(""), parent);
    //QObject::connect(shortcut, SIGNAL(activated()), receiver, SLOT(yourSlotHere()));
}

void MainWindow::StereoVisionProcessInit(){
    cerr << "Arrumar a Matrix K, os valores das últimas colunas estão errados." << endl;
    cerr << "Arrumar a função StereoProcessor::calculateQMatrix()." << endl;
    cerr << "Arrumar o Constructor da classe StereoDisparityMap para Alocação de Memória das variáveis: disp_16S,disp_8U,disp_BGR" << endl;
    cerr << "Arrumar a declaração dos Destrutores de todas as classes" << endl;
    cerr << "Arrumar a inicialização e separar as variáveis 'Stereocfg' para os métodos BM e SGBM" << endl;

    printHelp();

    /* (1) Open Image Source */
    openStereoSource(stereo->getInputNum());
    stereo->readConfigFile();
    stereo->readStereoBMConfigFile();
    stereo->readStereoSGBMConfigFile();

    /* (2) Camera Setting */

    /* Getting Input Resolution*/
    if(stereo->isVideoFile){
        stereo->calib.imageSize.width = stereo->capL.get(CV_CAP_PROP_FRAME_WIDTH);
        stereo->calib.imageSize.height = stereo->capL.get(CV_CAP_PROP_FRAME_HEIGHT);
    }else{
        stereo->calib.imageSize.width = stereo->imageL[0].cols;
        stereo->calib.imageSize.height = stereo->imageL[0].rows;
    }

    /* Setting Desired Resolution */
    stereo->calib.imageSizeDesired.width = 640;
    stereo->calib.imageSizeDesired.height = 480;

    /* Checking Invalid Input Size */
    if(stereo->calib.imageSize.width==0 && stereo->calib.imageSize.height==0){
        cerr << "Number of Cols and Number of Rows equal to ZERO!" << endl;
    }else{
        /* Console Output */
        cout << "Input Resolution(Width,Height): (" << stereo->calib.imageSize.width << "," << stereo->calib.imageSize.height << ")" << endl;
        cout << "Desired Resolution(Width,Height): (" << stereo->calib.imageSizeDesired.width << "," << stereo->calib.imageSizeDesired.height << ")" << endl << endl;

        /* GUI */
        ui->textBoxOutput->appendPlainText(QString("Input Resolution(Width,Height): (")+QString::number(stereo->calib.imageSize.width)+QString(",")+QString::number(stereo->calib.imageSize.height)+QString(")"));
        ui->textBoxOutput->appendPlainText(QString("Desired Resolution(Width,Height): (")+QString::number(stereo->calib.imageSizeDesired.width)+QString(",")+QString::number(stereo->calib.imageSizeDesired.height)+QString(")"));
    }

    /* Resizing the Input Resolution to the Desired Resolution*/
    if(stereo->isImageFile && (stereo->calib.imageSize.width != stereo->calib.imageSizeDesired.width)){
        stereo->utils.resizeFrames(&stereo->imageL[0],&stereo->imageR[0]);
    }

    /* (3) Stereo Initialization */

    /* Initializing Stereo Matching Methods */
    stereo->stereoBM_Init();
    stereo->stereoSGBM_Init();

    /* Setting Stereo Parameters */
    stereo->setStereoBM_Params();
    stereo->setStereoSGBM_Params();

    /* (4) Stereo Calibration */
    if(stereo->calib.needCalibration){
        /* Read Calibration Files */
        stereo->calib.readCalibrationFiles();

        /* Read/Calculate the Q Matrix */
        stereo->calib.readQMatrix(); //true=640x480 false=others

        //Point2d imageCenter = Point2d((imageL[0].cols-1.0)/2.0,(imageL[0].rows-1.0)/2.0);
        //stereo->calib.calculateQMatrix();

        /* Calculate the K Matrix */
        ////        // Checking Intrinsic Matrix
        ////        if(stereo->calib.isKcreated){
        ////           cout << "The Intrinsic Matrix is already Created." << endl << endl;
        ////        }else{
        //            //createKMatrix();
        // //       }
        stereo->calib.createKMatrix();

    }else{
        cout << "Calibration: OFF" << endl << endl;
        cerr << "Warning: Couldn't generate 3D Reconstruction. Please, check Q,K Matrix." << endl;

        //stereo->readQMatrix(); //true=640x480 false=others
        //stereo->createKMatrix();
    }

    /* (5) Point Cloud Initialization */
    stereo->view3D.PointCloudInit(stereo->calib.baseline,true);
}

void MainWindow::StereoVisionProcess_UpdateGUI(){
    /* (6) Rendering Loop */
    stereo->utils.startClock();

    if(stereo->isVideoFile){
        /* (7) Frames Capture */
        stereo->captureFrames();

        /* (8) Camera Retification */
        if(stereo->calib.needCalibration){
            stereo->applyRectification();
        }
    }

    /* (9) Disparities Calculation */
    if(stereo->flags.showDisparityMap || stereo->flags.show3Dreconstruction || stereo->flags.showTrackingObjectView || stereo->flags.showDiffImage || stereo->flags.showWarningLines){
        stereo->calculateDisparities();
    }

    /* (10) Projecting 3D point cloud to image */
    if(stereo->flags.show3Dreconstruction){
        stereo->calculate3DReconstruction();
    }

    /* (11) Image Processing */
    if(stereo->flags.showTrackingObjectView || stereo->flags.showDiffImage || stereo->flags.showWarningLines){
        if(stereo->isVideoFile){
            stereo->imageProcessing(stereo->disp.disp_8U,stereo->disp.disp_8U_eroded,stereo->disp.disp_8U_eroded_dilated,stereo->imageL[0],true);
        }

        if(stereo->isImageFile){
            ui->textBoxOutput->appendPlainText(QString("This feature is not supported, because of the Input file type (Image File).1"));
        }
    }

    if(stereo->flags.showHistograms){
        stereo->utils.calculateHist(stereo->disp.disp_8U,"Disparity Map Histogram");
        stereo->utils.calculateHist(stereo->imageL[0],"Left Image Histogram");
    }else{
        destroyWindow("Disparity Map Histogram");
        destroyWindow("Left Image Histogram");
    }

    /* (12) Movement Difference between Frames */
    if(stereo->flags.showDiffImage || stereo->flags.showWarningLines){
        if(stereo->isVideoFile){
            if(stereo->diff.StartDiff){
                stereo->diff.createDiffImage(stereo->imageL_grey[0],stereo->imageL_grey[1]);

                if(stereo->diff.diffImage.data){
                    stereo->diff.createResAND(stereo->diff.diffImage,stereo->imgThreshold);
                    stereo->diff.convertToBGR();
                    stereo->imageL[0].copyTo(stereo->diff.imageL);
                    stereo->diff.addRedLines();
                }

                stereo->saveLastFrames();
            }else{
                stereo->saveLastFrames();
                stereo->diff.StartDiff=1;
            }
        }

        if(stereo->isImageFile){
            ui->textBoxOutput->appendPlainText(QString("This feature is not supported, because of the Input file type (Image File).2"));
        }
    }

    /* (13) GUI Output */
    if(stereo->flags.showInputImages){
        this->putImageL(stereo->imageL[0]);
        this->putImageR(stereo->imageR[0]);

        ui->toggleBtnShowHist->hide();
        ui->toggleBtnShowXYZ->hide();
        ui->toggleBtnShowDispDepth->hide();
    }

    if(stereo->flags.showDisparityMap){
        this->putImageL(stereo->disp.disp_8U);
        this->putImageR(stereo->disp.disp_BGR);

        ui->toggleBtnShowHist->show();
        ui->toggleBtnShowXYZ->hide();
        ui->toggleBtnShowDispDepth->show();
    }

    if(stereo->flags.show3Dreconstruction){
        this->putImageL(stereo->view3D.disp3D_8U);
        this->putImageR(stereo->view3D.disp3D_BGR);

        ui->toggleBtnShowHist->hide();
        ui->toggleBtnShowXYZ->show();
        ui->toggleBtnShowDispDepth->hide();
    }

    if(stereo->flags.showTrackingObjectView){
        this->putImageL(stereo->trackingView);
        this->putImageR(stereo->imgThresholdDraw);

        ui->toggleBtnShowHist->hide();
        ui->toggleBtnShowXYZ->hide();
        ui->toggleBtnShowDispDepth->hide();
    }

    if(stereo->flags.showDiffImage && stereo->diff.StartDiff){
        this->putImageL(stereo->diff.diffImage);
        this->putImageR(stereo->diff.res_AND);

        ui->toggleBtnShowHist->hide();
        ui->toggleBtnShowXYZ->hide();
        ui->toggleBtnShowDispDepth->hide();
    }

    if(stereo->flags.showWarningLines && stereo->diff.StartDiff){
        this->putImageL(stereo->diff.res_ADD);
        this->putImageR(stereo->diff.res_AND_BGR);

        ui->toggleBtnShowHist->hide();
        ui->toggleBtnShowXYZ->hide();
        ui->toggleBtnShowDispDepth->hide();
    }

    /* (14) Video Loop - If the last frame is reached, reset the capture and the frameCounter */
    stereo->videoLooper();

    /* (15) Performance Measurement - FPS */
    stereo->utils.stopClock();
    //stereo->utils.showFPS();
    ui->lcdNumber->display(this->stereo->utils.getFPS());

    /* (16) Shortcuts */

    waitKey(1); // It will display the window infinitely until any keypress (it is suitable for image display)
    if(closeEventOccured){
        cout << "----------------------------- END ------------------------------------" << endl;
        return;
    }
}

void MainWindow::printHelp(){
    /* Console Output */
    cout << "-----------------Help Menu-----------------\n"
         << "Run command line: ./reprojectImageTo3D\n"
         << "Keys:\n"
         << "'`' -\tShow Help\n"
         << "'1' -\tShow L/R Windows\t\t'4' -\tShow XYZ\n"
         << "'2' -\tShow Disparity Map\t\t'5' -\tShow FPS\n"
         << "'3' -\tShow 3D Reconstruction\t'6' -\tShow Stereo Parameters\n"
         << "\n3D Viewer Navigation:\n"
         << "x-axis:\t'Key_Right'/'Key_Left' -> +x,-x\n"
         << "y-axis:\t'Key_Up'/'Key_Down' -> +y,-y\n"
         << "z-axis:\t'Key_Plus'/'Key_Minus' -> +z,-z\n"
         << "-------------------------------------------\n"
         << "\n\n";

    /* GUI */
    ui->textBoxOutput->appendPlainText
            (QString("-----------------Help Menu-----------------\n")+
             QString("Run command line: ./reprojectImageTo3D\n")+
             QString("Keys:\n")+
             QString("'`' -\tShow Help\n")+
             QString("'1' -\tShow L/R Windows\t\t'4' -\tShow XYZ\n")+
             QString("'2' -\tShow Disparity Map\t\t'5' -\tShow FPS\n")+
             QString("'3' -\tShow 3D Reconstruction\t'6' -\tShow Stereo Parameters\n")+
             QString("\n3D Viewer Navigation:\n")+
             QString("x-axis:\t'Key_Right'/'Key_Left' -> +x,-x\n")+
             QString("y-axis:\t'Key_Up'/'Key_Down' -> +y,-y\n")+
             QString("z-axis:\t'Key_Plus'/'Key_Minus' -> +z,-z\n")+
             QString("-------------------------------------------\n")+
             QString("\n\n"));
}

void MainWindow::openStereoSource(int inputNum){
    /* Create an object that decodes the input Video stream. */
    cout << "Enter Video Number(1,2,3,4,5,6,7,8,9): " << endl;
    cout << "Input File: " << inputNum << endl;

    //	scanf("%d",&inputNum);
    ui->textBoxOutput->appendPlainText(QString("Enter Video Number(1,2,3,4,5,6,7,8,9): "));
    ui->textBoxOutput->appendPlainText(QString("Input File: ")+QString::number(inputNum));

    switch(inputNum){
    case 1:
        this->stereo->imageL_filename = "../../workspace/data/video10_l.avi";
        this->stereo->imageR_filename = "../../workspace/data/video10_r.avi";
        stereo->calib.needCalibration=true;
        break;
    case 2:
        this->stereo->imageL_filename = "../../workspace/data/video12_l.avi";
        this->stereo->imageR_filename = "../../workspace/data/video12_r.avi";
        stereo->calib.needCalibration=true;
        break;
    case 3:
        this->stereo->imageL_filename = "../../workspace/data/dataset/Piano-perfect/im0.png";
        this->stereo->imageR_filename = "../../workspace/data/dataset/Piano-perfect/im1.png";
        stereo->calib.needCalibration=true;
        break;
    case 4:
        this->stereo->imageL_filename = "../../workspace/data/20004.avi";
        this->stereo->imageR_filename = "../../workspace/data/30004.avi";
        stereo->calib.needCalibration=true;
        break;
    case 5:
        this->stereo->imageL_filename = "../../workspace/data/teddy_l.png";
        this->stereo->imageR_filename = "../../workspace/data/teddy_r.png";
        stereo->calib.needCalibration=true;
        break;
    case 6:
        this->stereo->imageL_filename = "../../workspace/data/left/video15.avi";
        this->stereo->imageR_filename = "../../workspace/data/right/video15.avi";
        stereo->calib.needCalibration=true;
        break;
    case 7:
        this->stereo->imageL_filename = "../../workspace/data/20011.avi";
        this->stereo->imageR_filename = "../../workspace/data/30011.avi";
        stereo->calib.needCalibration=false;
        break;
    case 8:
        this->stereo->imageL_filename = "../data/left/left2.png";
        this->stereo->imageR_filename = "../data/right/right2.png";
        stereo->calib.needCalibration=false;
        break;
    case 9:
        this->stereo->imageL_filename = "../data/left/left3.png";
        this->stereo->imageR_filename = "../data/right/right3.png";
        stereo->calib.needCalibration=false;
        break;
    }

    /* Identify the type of the input file. */
    if(this->stereo->imageL_filename.substr(this->stereo->imageL_filename.find_last_of(".") + 1) == "avi"){
        this->stereo->isVideoFile=true;

        stereo->capL.open(this->stereo->imageL_filename);
        stereo->capR.open(this->stereo->imageR_filename);

        if(!stereo->capL.isOpened() || !stereo->capR.isOpened()){		// Check if it succeeded
            cerr <<  "Could not open or find the input videos!" << endl ;
            ui->textBoxOutput->appendPlainText(QString( "Could not open or find the input videos!"));
            //return -1;
        }

        /* Console Output */
        cout << "It's a Video file" << endl;
        cout << "Input 1 Resolution: " << stereo->capR.get(CV_CAP_PROP_FRAME_WIDTH) << "x" << stereo->capR.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
        cout << "Input 2 Resolution: " << stereo->capL.get(CV_CAP_PROP_FRAME_WIDTH) << "x" << stereo->capL.get(CV_CAP_PROP_FRAME_HEIGHT) << endl << endl;

        /* GUI */
        ui->textBoxOutput->appendPlainText(QString("It's a Video file"));
        ui->textBoxOutput->appendPlainText(QString("Input 1 Resolution: ") + QString::number(stereo->capL.get(CV_CAP_PROP_FRAME_WIDTH)) + QString("x") + QString::number(stereo->capL.get(CV_CAP_PROP_FRAME_HEIGHT)));
        ui->textBoxOutput->appendPlainText(QString("Input 2 Resolution: ") + QString::number(stereo->capR.get(CV_CAP_PROP_FRAME_WIDTH)) + QString("x") + QString::number(stereo->capR.get(CV_CAP_PROP_FRAME_HEIGHT)));
    }else{
        cout << "It is not a Video file" << endl;
        ui->textBoxOutput->appendPlainText(QString( "It is not a Video file"));
        if(this->stereo->imageL_filename.substr(this->stereo->imageL_filename.find_last_of(".") + 1) == "jpg" || this->stereo->imageL_filename.substr(this->stereo->imageL_filename.find_last_of(".") + 1) == "png"){
            cout << "It's a Image file" << endl;
            ui->textBoxOutput->appendPlainText(QString( "It's a Image file"));
            this->stereo->isImageFile=true;

            stereo->imageL[0] = imread(this->stereo->imageL_filename, CV_LOAD_IMAGE_COLOR);
            stereo->imageR[0] = imread(this->stereo->imageR_filename, CV_LOAD_IMAGE_COLOR);

            if(!stereo->imageL[0].data || !stereo->imageR[0].data){      // Check if it succeeded
                ui->textBoxOutput->appendPlainText(QString("Could not open or find the input images!"));
                return;
            }
        }else{
            cout << "It is not a Image file" << endl;
            ui->textBoxOutput->appendPlainText(QString( "It is not a Image file"));
        }
    }
}

void MainWindow::on_btnShowInputImages_clicked(){
    this->stereo->flags.showInputImages = true;
    this->stereo->flags.showDisparityMap = false;
    this->stereo->flags.show3Dreconstruction = false;
    this->stereo->flags.showTrackingObjectView = false;
    this->stereo->flags.showDiffImage = false;
    this->stereo->flags.showWarningLines = false;
}

void MainWindow::on_btnShowDisparityMap_clicked(){
    this->stereo->flags.showInputImages = false;
    this->stereo->flags.showDisparityMap = true;
    this->stereo->flags.show3Dreconstruction = false;
    this->stereo->flags.showTrackingObjectView = false;
    this->stereo->flags.showDiffImage = false;
    this->stereo->flags.showWarningLines = false;
}

void MainWindow::on_btnShow3DReconstruction_clicked(){
    this->stereo->flags.showInputImages = false;
    this->stereo->flags.showDisparityMap = false;
    this->stereo->flags.show3Dreconstruction = true;
    this->stereo->flags.showTrackingObjectView = false;
    this->stereo->flags.showDiffImage = false;
    this->stereo->flags.showWarningLines = false;
}

void MainWindow::on_btnShowTrackingObjectView_clicked(){
    this->stereo->flags.showInputImages = false;
    this->stereo->flags.showDisparityMap = false;
    this->stereo->flags.show3Dreconstruction = false;
    this->stereo->flags.showTrackingObjectView = true;
    this->stereo->flags.showDiffImage = false;
    this->stereo->flags.showWarningLines = false;
}

void MainWindow::on_btnShowDiffImage_clicked(){
    this->stereo->flags.showInputImages = false;
    this->stereo->flags.showDisparityMap = false;
    this->stereo->flags.show3Dreconstruction = false;
    this->stereo->flags.showTrackingObjectView = false;
    this->stereo->flags.showDiffImage = true;
    this->stereo->flags.showWarningLines = false;
}

void MainWindow::on_btnShowWarningLines_clicked(){
    this->stereo->flags.showInputImages = false;
    this->stereo->flags.showDisparityMap = false;
    this->stereo->flags.show3Dreconstruction = false;
    this->stereo->flags.showTrackingObjectView = false;
    this->stereo->flags.showDiffImage = false;
    this->stereo->flags.showWarningLines = true;
}

void MainWindow::on_btnPauseOrResume_clicked(){
    if(tmrTimer->isActive() == true){
        tmrTimer->stop();

        /* Console Output */
        //cout << "Paused!" << endl;

        /* GUI */
        ui->btnPauseOrResume->setText("Resume");
        ui->statusBar->showMessage("Halted!");

    }else{
        tmrTimer->start(20);

        /* Console Output */
        //cout << "Resumed!" << endl;

        /* GUI */
        ui->btnPauseOrResume->setText("Pause");
        ui->statusBar->showMessage("Running...");
    }
}

void MainWindow::on_btnShowStereoParamSetup_clicked(){
    /* Creates  stereoParamsSetupWindow Object */
    stereoParamsSetupWindow = new SetStereoParams(this, stereo);

    cout << "[Stereo Param Setup] Stereo Parameters Configuration Loaded!" << endl;

    if(stereo->flags.methodBM){
        this->stereoParamsSetupWindow->loadStereoParamsUi(stereo->BMcfg.preFilterSize,
                                                          stereo->BMcfg.preFilterCap,
                                                          stereo->BMcfg.SADWindowSize,
                                                          stereo->BMcfg.minDisparity,
                                                          stereo->BMcfg.numberOfDisparities,
                                                          stereo->BMcfg.textureThreshold,
                                                          stereo->BMcfg.uniquenessRatio,
                                                          stereo->BMcfg.speckleWindowSize,
                                                          stereo->BMcfg.speckleRange,
                                                          stereo->BMcfg.disp12MaxDiff);
        // Debug
        //stereo->BMcfg.showConfigValues();
    }

    if(stereo->flags.methodSGBM){
        this->stereoParamsSetupWindow->loadStereoParamsUi(stereo->SGBMcfg.preFilterSize,
                                                          stereo->SGBMcfg.preFilterCap,
                                                          stereo->SGBMcfg.SADWindowSize,
                                                          stereo->SGBMcfg.minDisparity,
                                                          stereo->SGBMcfg.numberOfDisparities,
                                                          stereo->SGBMcfg.textureThreshold,
                                                          stereo->SGBMcfg.uniquenessRatio,
                                                          stereo->SGBMcfg.speckleWindowSize,
                                                          stereo->SGBMcfg.speckleRange,
                                                          stereo->SGBMcfg.disp12MaxDiff);
        // Debug
        //stereo->SGBMcfg.showConfigValues();
    }

    stereoParamsSetupWindow->show();
}

void MainWindow::on_comboBox_activated(int index){
    switch(index){
    case 0:
        cout << "Chose Method: BM" <<endl;
        stereo->flags.methodBM = true;
        stereo->flags.methodSGBM = false;
        break;
    case 1:
        cout << "Chose Method: SGBM" <<endl;
        stereo->flags.methodBM = false;
        stereo->flags.methodSGBM = true;
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes){
        event->ignore();
    } else {
        event->accept();
        this->~MainWindow();
    }
}

QImage MainWindow::Mat2QImage(const Mat& mat){
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1){
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }

    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3){
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else{
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::putImageL(const Mat& src){
    qimageL = Mat2QImage(src);
    ui->lblOriginalLeft->setPixmap(QPixmap::fromImage(qimageL));
}

void MainWindow::putImageR(const Mat& src){
    qimageR = Mat2QImage(src);
    ui->lblOriginalRight->setPixmap(QPixmap::fromImage(qimageR));
}

void MainWindow::on_toggleBtnShowHist_clicked(bool checked){
    if(checked){
        cout << "Show Hist 1: On" << endl;
        stereo->flags.showHistograms = true;
    }else{
        cout << "Show Hist 2: Off" << endl;
        stereo->flags.showHistograms = false;
    }
}

void MainWindow::on_toggleBtnShowXYZ_toggled(bool checked){
    if(checked){
        cout << "ShowXYZ: On" << endl;
        stereo->flags.showXYZ = true;
    }else{
        cout << "ShowXYZ: Off" << endl;
        stereo->flags.showXYZ = false;
    }
}

void MainWindow::on_toggleBtnShowDispDepth_toggled(bool checked){
    if(checked){
        cout << "Show Disparity/Depth: On" << endl;
        stereo->flags.showDispDepth = true;
    }else{
        cout << "Show Disparity/Depth: Off" << endl;
        stereo->flags.showDispDepth = false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e){
    int x_clickedPos;
    int y_clickedPos;

    int x_min = ui->lblOriginalLeft->geometry().x();
    int x_max = x_min + ui->lblOriginalLeft->geometry().width();

    int y_min = ui->lblOriginalLeft->geometry().y();
    int y_max = y_min + ui->lblOriginalLeft->geometry().height();

    x_clickedPos = e->pos().x();
    y_clickedPos = e->pos().y();

    if(x_clickedPos >= x_min && x_clickedPos <= x_max){
        if(y_clickedPos >= y_min && y_clickedPos <= y_max){

            stereo->x = e->pos().x()-x_min;
            stereo->y = e->pos().y()-y_min;

            /* Debug - Mouse Clicked Coordinates (x,y) */
            //cout << "x: " << stereo->x << endl;
            //cout << "y: " << stereo->y << endl;
        }

    }
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_Left:
        stereo->view3D.viewpoint.x-= stereo->view3D.step;
        break;
    case Qt::Key_Right:
        stereo->view3D.viewpoint.x+= stereo->view3D.step;
        break;
    case Qt::Key_Up:
        stereo->view3D.viewpoint.y+= stereo->view3D.step;
        break;
    case Qt::Key_Down:
        stereo->view3D.viewpoint.y-= stereo->view3D.step;
        break;
    case Qt::Key_Plus:
        stereo->view3D.viewpoint.z+= stereo->view3D.step;
        break;
    case Qt::Key_Minus:
        stereo->view3D.viewpoint.z-= stereo->view3D.step;
        break;
    }
}
