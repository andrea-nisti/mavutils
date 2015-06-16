#include "window.h"
#include <QPushButton>




window::window(QWidget *parent) :
    QWidget(parent)
{

setFixedSize(300,200);
button = new QPushButton ("new button",this);
button->setGeometry(100,100,80,30);
}

window::window(int w , int h, QWidget *parent) :
    QWidget(parent)
{

setFixedSize(w,h);
button = new QPushButton ("new button",this);
button->setGeometry(100,100,80,30);
}
