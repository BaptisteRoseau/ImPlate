#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "processConfig.h"
#include "customlabel.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QCoreApplication>
#include <QShortcut>
#include <QImage>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

/* =====================================================
            CONSTRUCTORS / INIT / FINALIZE
   ===================================================== */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup UI and mouse tracking routines
    ui->setupUi(this);
    connect(ui->LabelImageCar, SIGNAL(myMousePressed()), this, SLOT(mousePressed()));
    connect(ui->LabelImageCar, SIGNAL(myMousePos()), this, SLOT(mouseCurrentPos()));
    connect(ui->LabelImageCar, SIGNAL(myMouseLeft()), this, SLOT(mouseLeft()));

    // Setup shortcut
    new QShortcut(QKeySequence(Qt::Key_A),            this, SLOT(on_ButtonPrevious_clicked()));
    new QShortcut(QKeySequence(Qt::Key_Z),            this, SLOT(on_ButtonNext_clicked()));
    new QShortcut(QKeySequence(Qt::Key_E),            this, SLOT(on_ButtonBlur_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(on_ButtonSave_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this, SLOT(on_ButtonCancel_clicked()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_E), this, SLOT(on_ButtonAutoBlur_clicked()));

    // Initialize corners vectors
    blurCorners  = vector<vector<Point> >();
    cornerBuffer = vector<Point>();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete procConf;
}

void MainWindow::initProcess(char *argv[]){
    this->procConf = new ProcessConfig(argv);
    this->procConf->init();
    this->initProgressBar();
    ui->LabelImageCar->setScaledContents(true);

    // Displaying the first image
    this->procConf->firstImage();
    this->updateButtonState();
    this->updatePictureAndTextLabel();

    if (ui->CheckboxAlwaysTryAutoblur->isChecked()){
        this->on_ButtonAutoBlur_clicked();
    }
}

void MainWindow::finalizeProcess(void){
    this->procConf->finalize();
}


/* =====================================================
                        METHODS
   ===================================================== */

void drawLinesFromCorner(Mat &img, vector<Point> corners){
    if (corners.empty()) return;

    size_t i;
    for (i = 0; i < corners.size() - 1; i++){
        line(img, corners[i], corners[i+1], Scalar(0,255,0), 1);
    }
    if (corners.size() == 4){
        line(img, corners[3], corners[0], Scalar(0,255,0), 1);
    }
}

void MainWindow::drawLines(Mat &img){
    drawLinesFromCorner(img, this->cornerBuffer);
    for (auto &corners: blurCorners){
        drawLinesFromCorner(img, corners);
    }
}

void MainWindow::updatePictureAndTextLabel(void){
    // Getting image and drawing lines onto it
    Mat img = this->procConf->getblurredPicture().clone();
    this->drawLines(img);
    QPixmap pixmap = QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_BGR888));

    // Updating image and text labels
    ui->LabelImageCar->resize(pixmap.size());
    ui->LabelImageCar->setPixmap(pixmap);
    ui->LabelTextFileName->setText(QString::fromStdString(this->procConf->getFilepath()));
}

void MainWindow::initProgressBar(void){
    ui->progressBar->setMinimum(1);
    ui->progressBar->setMaximum(this->procConf->maximumPictureIdx());
    ui->progressBar->setValue(this->procConf->currentPictureIdx());
}

void MainWindow::updateProgressBar(void){
    ui->progressBar->setValue(this->procConf->currentPictureIdx());
}

void MainWindow::updateButtonState(void){
    if (this->procConf->isFirstImage()){
        ui->ButtonPrevious->setEnabled(false);
    } else {
        ui->ButtonPrevious->setEnabled(true);
    }

    if (this->procConf->isLastImage()){
        ui->ButtonNext->setEnabled(false);
    } else {
        ui->ButtonNext->setEnabled(true);
    }
}

void MainWindow::mousePressed(){
    // Adding point to corner buffer
    this->cornerBuffer.push_back(Point(ui->LabelImageCar->x, ui->LabelImageCar->y));

    // Adding last 4 corners to 4 corners list
    if (this->cornerBuffer.size() >= 4){
        this->blurCorners.push_back(this->cornerBuffer);
        this->cornerBuffer.clear();
    }

    // Bluring if CheckboxBlurEveryFour is checked
    if (ui->CheckboxBlurEveryFour->isChecked() && !this->blurCorners.empty()){
        this->on_ButtonBlur_clicked();
    } else {
        this->updatePictureAndTextLabel();
    }
}

void MainWindow::mouseCurrentPos(){
    // Not used but better leave it here
}


void MainWindow::mouseLeft(){
    // Not used but better leave it here
}


/* =====================================================
                    BUTTON METHODS
   ===================================================== */

void MainWindow::on_ButtonPrevious_clicked(){
    // Loading previous picture and updating main window
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->procConf->clearCornersVector();
    this->procConf->previousImage();
    this->updatePictureAndTextLabel();
    this->updateProgressBar();
    this->updateButtonState();

}

void MainWindow::on_ButtonNext_clicked(){
    // Saving picture before getting to next one
    if (ui->CheckboxSaveOnNext->isChecked()){
        this->on_ButtonSave_clicked();
    }

    // Loading next picture and updating main window
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->procConf->clearCornersVector();
    this->procConf->nextImage();
    this->updatePictureAndTextLabel();
    this->updateProgressBar();
    this->updateButtonState();

    // Trying to autoblur next picture
    if (ui->CheckboxAlwaysTryAutoblur->isChecked()){
        this->on_ButtonAutoBlur_clicked();
    }
}

void MainWindow::on_ButtonBlur_clicked(){
    // Try bluring only if there is at least a 4 clicked point vector
    if (this->blurCorners.empty()){ return; }

    // User feedback
    ui->LabelProgramStatus->setText("Bluring, please wait...");
    QCoreApplication::processEvents();

    // Saving plate location if only 1 area is selected for bluring
    if (this->blurCorners.size() == 1){
        this->procConf->setCornersVector(this->blurCorners.back());
    }

    // Bluring picture and cleaning buffers
    this->procConf->blurImage(this->blurCorners);
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->updatePictureAndTextLabel();

    // Clearing user feedback
    ui->LabelProgramStatus->setText("");
}

void MainWindow::on_ButtonSave_clicked(){
    // Saving picture only if something changed (ex: blurred)
    if (this->procConf->isPictureStateChanged()){
        // User feedback
        ui->LabelProgramStatus->setText("Saving...");
        QCoreApplication::processEvents();
        
        this->procConf->saveImage();
        
        // Clearing user feedback
        ui->LabelProgramStatus->setText("");
    }

}

void MainWindow::on_ButtonCancel_clicked(){
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->procConf->cancel();
    this->updatePictureAndTextLabel();
}

void MainWindow::on_ButtonAutoBlur_clicked(){
    this->procConf->autoBlur();
    this->updatePictureAndTextLabel();
}
