#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processConfig.h"
#include "customlabel.h"

#include <QMainWindow>
#include <QCheckBox>
#include <QGraphicsScene>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initProcess(char *argv[]);
    void finalizeProcess(void);


private slots:
    void on_ButtonPrevious_clicked();
    void on_ButtonNext_clicked();
    void on_ButtonBlur_clicked();
    void on_ButtonSave_clicked();
    void on_ButtonCancel_clicked();
    void on_ButtonAutoBlur_clicked();

    void mousePressed(void);
    void mouseCurrentPos(void);
    void mouseLeft(void);

private:
    Ui::MainWindow *ui;
    QCheckBox *cbAlwaysTryAutoBlur = new QCheckBox("cbAlwaysTryAutoBlur", this);
    QCheckBox *cbSaveOnNext        = new QCheckBox("cbSaveOnNext", this);
    ProcessConfig *procConf        = NULL;
    std::vector<std::vector<cv::Point> > blurCorners;
    std::vector<cv::Point> cornerBuffer;

    void updatePictureAndTextLabel(void);
    void initProgressBar();
    void updateProgressBar();
    void updateButtonState();
    
};
#endif // MAINWINDOW_H
