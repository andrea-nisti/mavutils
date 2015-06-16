#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class QPushButton;
class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = 0);
    explicit Window(int w, int h, QWidget *parent = 0);
    QPushButton *button;

signals:

public slots:

private:


};

#endif // Window_H
