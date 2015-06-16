#ifndef GUI_H
#define GUI_H

#include <QObject>
#include "Window.h"

class Gui : public QObject
{
    Q_OBJECT

    Window *win;
public:
    explicit Gui(QObject *parent = 0);

signals:

public slots:

};

#endif // GUI_H
