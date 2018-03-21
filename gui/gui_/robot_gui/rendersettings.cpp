#include "rendersettings.h"
#include "ui_rendersettings.h"

RenderSettings::RenderSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);
}

RenderSettings::~RenderSettings()
{
    delete ui;
}
