#include "rendersettings.h"
#include "ui_rendersettings.h"
#include <QColorDialog>

const int INTENSITY_DIVIDE_FACTOR = 20;
const int AMBIENT_DIVIDE_FACTOR = 50;
const int SPECULAR_DIVIDE_FACTOR = 50;

RenderSettings::RenderSettings(QWidget *parent, RenderWidget *renderWidget) :
    QDialog(parent),
    ui(new Ui::RenderSettings),
    renderWidget(renderWidget)
{
    ui->setupUi(this);
}

void RenderSettings::setDefaultValues()
{
    ui->intensitySlider->setValue((int) (renderWidget->mScene.mainLight.intensity * INTENSITY_DIVIDE_FACTOR));
    ui->ambientSlider->setValue((int) (renderWidget->mScene.mainLight.ambientStrength * AMBIENT_DIVIDE_FACTOR));
    ui->specularSlider->setValue((int) (renderWidget->mScene.mainLight.specularStrength * SPECULAR_DIVIDE_FACTOR));
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

void RenderSettings::on_wireframeCheckBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        renderWidget->mRenderer.wireframeMode = true;
    } else {
        renderWidget->mRenderer.wireframeMode = false;
    }
    renderWidget->update();
}

void RenderSettings::on_intensitySlider_sliderMoved(int position)
{
    renderWidget->mScene.mainLight.intensity = (float) position / INTENSITY_DIVIDE_FACTOR;
    renderWidget->update();
}

void RenderSettings::on_ambientSlider_sliderMoved(int position)
{
    renderWidget->mScene.mainLight.ambientStrength = (float) position / AMBIENT_DIVIDE_FACTOR;
    renderWidget->update();
}

void RenderSettings::on_specularSlider_sliderMoved(int position)
{
    renderWidget->mScene.mainLight.specularStrength = (float) position / SPECULAR_DIVIDE_FACTOR;
    renderWidget->update();
}

void RenderSettings::on_lightColorButton_clicked()
{
    QColor initial;
    initial.setRedF(renderWidget->mScene.mainLight.color.x);
    initial.setGreenF(renderWidget->mScene.mainLight.color.y);
    initial.setBlueF(renderWidget->mScene.mainLight.color.z);

    QColor color = QColorDialog::getColor(initial, this, "Select Light Color");
    renderWidget->mScene.mainLight.color.x = (float) color.redF();
    renderWidget->mScene.mainLight.color.y = (float) color.greenF();
    renderWidget->mScene.mainLight.color.z = (float) color.blueF();
    renderWidget->update();
}

void RenderSettings::on_backgroundColorButton_clicked()
{
    QColor initial;
    initial.setRedF(renderWidget->mRenderer.mBackgroundColor.x);
    initial.setGreenF(renderWidget->mRenderer.mBackgroundColor.y);
    initial.setBlueF(renderWidget->mRenderer.mBackgroundColor.z);

    QColor color = QColorDialog::getColor(initial, this, "Select Background Color");
    renderWidget->mRenderer.mBackgroundColor.x = (float) color.redF();
    renderWidget->mRenderer.mBackgroundColor.y = (float) color.greenF();
    renderWidget->mRenderer.mBackgroundColor.z = (float) color.blueF();
    renderWidget->update();
}
