#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

/**
 * @brief A custom class to manage mouse events on a QLabel
 * 
 */
class CustomLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CustomLabel(QWidget *parent = nullptr);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *ev);

    int x,y;

signals:
    void myMousePressed();
    void myMousePos();
    void myMouseLeft();
};

#endif // CUSTOMLABEL_H
