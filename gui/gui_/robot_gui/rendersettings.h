#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H
#include "renderwidget.h"
#include <QDialog>

namespace Ui {
class RenderSettings;
}

class RenderSettings : public QDialog
{
    Q_OBJECT

public:
    explicit RenderSettings(QWidget *parent, RenderWidget* renderWidget);
    void setDefaultValues();
    ~RenderSettings();

private slots:
    void on_wireframeCheckBox_stateChanged(int arg1);

    void on_intensitySlider_sliderMoved(int position);

    void on_ambientSlider_sliderMoved(int position);

    void on_specularSlider_sliderMoved(int position);

    void on_lightColorButton_clicked();

    void on_backgroundColorButton_clicked();

private:
    Ui::RenderSettings *ui;
    RenderWidget* renderWidget;
};

#endif // RENDERSETTINGS_H
