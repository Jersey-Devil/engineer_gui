#ifndef MYQSLIDER_H
#define MYQSLIDER_H

#include <QObject>
#include <QSlider>

/*
 * class needed to handle specific behavior of sliders
 */
class MyQSlider : public QSlider
{
    Q_OBJECT
public:
    explicit MyQSlider(QObject *parent = 0);


signals:

public slots:
    void my_sliderMoved();

};

#endif // MYQSLIDER_H
