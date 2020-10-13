#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "processConfig.h"
#include "customlabel.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/mat.hpp>

using namespace cv;
using namespace std;


/* =====================================================
            CONSTRUCTORS / INIT / FINALIZE
   =====================================================*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->LabelImageCar, SIGNAL(myMousePressed()), this, SLOT(mousePressed()));
    connect(ui->LabelImageCar, SIGNAL(myMousePos()), this, SLOT(mouseCurrentPos()));
    connect(ui->LabelImageCar, SIGNAL(myMouseLeft()), this, SLOT(mouseLeft()));
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
   =====================================================*/

//TODO: colorier les contours avec OpenCV et pas QT
/* QPainter painter(&pixmap);
QPen Green((QColor(0,255,0)),1);
painter.setPen(Green);
painter.drawLine(50,50,250,250); */

void MainWindow::updatePictureAndTextLabel(void){
    // Getting image and drawing lines onto it
    Mat img = this->procConf->getBluredPicture();
    QPixmap pixmap = QPixmap::fromImage(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_BGR888));

    // Updating image and text labels
    ui->LabelImageCar->setFixedWidth(img.cols);
    ui->LabelImageCar->setFixedHeight(img.rows);
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
    }
}

void MainWindow::mouseCurrentPos(){

}


void MainWindow::mouseLeft(){

}


/* =====================================================
                    BUTTON METHODS
   =====================================================*/

//TODO: check errors on functions return
void MainWindow::on_ButtonPrevious_clicked()
{
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->procConf->previousImage();
    this->updatePictureAndTextLabel();
    this->updateProgressBar();
    this->updateButtonState();
    
}

void MainWindow::on_ButtonNext_clicked()
{
    if (ui->CheckboxSaveOnNext->isChecked()){
        this->on_ButtonSave_clicked();
    }

    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->procConf->nextImage();
    this->updatePictureAndTextLabel();
    this->updateProgressBar();
    this->updateButtonState();

    if (ui->CheckboxAlwaysTryAutoblur->isChecked()){
        this->on_ButtonAutoBlur_clicked();
    }
}

void MainWindow::on_ButtonBlur_clicked()
{
    this->procConf->blurImage(this->blurCorners);
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->updatePictureAndTextLabel();
}

void MainWindow::on_ButtonSave_clicked()
{
    if (this->procConf->isPictureStateChanged()){
        this->procConf->saveImage();
    }
}

void MainWindow::on_ButtonCancel_clicked()
{
    this->cornerBuffer.clear();
    this->blurCorners.clear();
    this->procConf->cancel();
    this->updatePictureAndTextLabel();
}

void MainWindow::on_ButtonAutoBlur_clicked()
{
    this->procConf->autoBlur();
    this->updatePictureAndTextLabel();
}
