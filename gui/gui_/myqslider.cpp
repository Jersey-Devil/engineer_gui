#include "myqslider.h"
#include <QSlider>
#include <QDebug>
MyQSlider::MyQSlider(QObject *parent)
{
    //connect to add custom action
    connect(this, SIGNAL(sliderReleased()),this, SLOT(my_sliderMoved()));
}

/**
 * @brief MyQSlider::my_sliderMoved
 * called to handle slider's movement and set value to %10 value
 */
void MyQSlider::my_sliderMoved(){
    int position = value();
    if (position%10!=0){
        if (position%10>5)
            setValue((position/10+1)*10);
        else setValue((position/10)*10);

    }
}

