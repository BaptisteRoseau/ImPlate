#include "customlabel.h"

CustomLabel::CustomLabel(QWidget *parent) : QLabel(parent){
}


void CustomLabel::mouseMoveEvent(QMouseEvent *ev){
    this->x = ev->pos().x();
    this->y = ev->pos().y();
    emit myMousePos();
}

void CustomLabel::mousePressEvent(QMouseEvent *ev){
    (void) ev;
    emit myMousePressed();
}

void CustomLabel::leaveEvent(QEvent *ev){
    (void) ev;
    emit myMouseLeft();
}

void myMousePressed(){

}

void myMousePos(){

}

void myMouseLeft(){

}
