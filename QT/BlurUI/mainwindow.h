#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processConfig.h"

#include <QMainWindow>
#include <QCheckBox>

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


private:
    Ui::MainWindow *ui;
    QCheckBox *cbAlwaysTryAutoBlur = new QCheckBox("cbAlwaysTryAutoBlur", this);
    QCheckBox *cbSaveOnNext        = new QCheckBox("cbSaveOnNext", this);
    ProcessConfig *procConf        = NULL;

    void updatePictureLabel(void);
    void initProgressBar(void);
    void updateProgressBar(void);
    void updateButtonState(void);
};
#endif // MAINWINDOW_H
