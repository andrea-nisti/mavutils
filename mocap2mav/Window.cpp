#include "Window.h"
#include <QPushButton>
#include <QPoint>
#include <QSize>

Window::Window(QWidget *parent) :
    QWidget(parent)
{

setFixedSize(500,500);
button = new QPushButton ("new button",this);
button->setGeometry(100,100,150,150);

}

Window::Window(int w , int h, QWidget *parent) :
    QWidget(parent)
{

setFixedSize(w,h);
button = new QPushButton ("new button",this);
QPoint *topL = new QPoint(w/2 - 40, h/2 - 15);
QSize  *size = new QSize(80,30);
QRect  *r = new QRect(*topL,*size);
button->setGeometry(*r);
}
