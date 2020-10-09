#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "processConfig.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>

//using namespace std;
using namespace cv;

// Display OpenCV Image: https://amin-ahmadi.com/2015/12/16/how-to-display-a-mat-image-in-qt/


/* =====================================================
            CONSTRUCTORS / INIT / FINALIZE
   =====================================================*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    // Displaying the first image
    this->procConf->firstImage();
    this->updateButtonState();
    this->updatePictureLabel();

    if (ui->CheckboxAlwaysTryAutoblur->isChecked()){
        this->on_ButtonAutoBlur_clicked();
    }
}

void MainWindow::finalizeProcess(void){
    this->procConf->finalize();
}


/* =====================================================
                        METHODS
   =====================================================*/

void MainWindow::updatePictureLabel(void){
    Mat img = this->procConf->getBluredPicture();
    ui->ImageLabel->setPixmap(QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888)));
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

/* =====================================================
                    BUTTON METHODS
   =====================================================*/

//TODO: check errors
void MainWindow::on_ButtonPrevious_clicked()
{
    this->procConf->previousImage();
    this->updatePictureLabel();
    this->updateProgressBar();
    this->updateButtonState();
}

void MainWindow::on_ButtonNext_clicked()
{
    if (ui->CheckboxSaveOnNext->isChecked()){
        this->on_ButtonSave_clicked();
    }

    this->procConf->nextImage();
    this->updatePictureLabel();
    this->updateProgressBar();
    this->updateButtonState();

    if (ui->CheckboxAlwaysTryAutoblur->isChecked()){
        this->on_ButtonAutoBlur_clicked();
    }
}

void MainWindow::on_ButtonBlur_clicked()
{
    //this->procConf->blurImage(std::vector<std::vector<cv::Point> >);
    this->updatePictureLabel();
}

void MainWindow::on_ButtonSave_clicked()
{
    if (this->procConf->isPictureStateChanged()){
        this->procConf->saveImage();
    }
}

void MainWindow::on_ButtonCancel_clicked()
{
    this->procConf->cancel();
    this->updatePictureLabel();
}

void MainWindow::on_ButtonAutoBlur_clicked()
{
    this->procConf->autoBlur();
    this->updatePictureLabel();
}
