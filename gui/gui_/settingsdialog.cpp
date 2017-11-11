#include "settingsdialog.h"
#include "ui_settingsdialog.h"

/**
 * @brief SettingsDialog::SettingsDialog
 * Dialog shown when user clicked "Settings" button on MainWindow
 * @param parent - MainWindow
 * @param c - Robot's configuration to show
 */
SettingsDialog::SettingsDialog(QWidget *parent, RobotConfiguration *c) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    //save basic from Robot class configuration
    conf = c;

    ui->setupUi(this);

    //set input values
    ui->platformForward_Edit->setText(QString::number(conf->platformForwardSpeed));
    ui->platformRotate_Edit->setText(QString::number(conf->platformRotateSpeed));
    ui->Shoulder_Edit->setText(QString::number(conf->shouldersSpeed));
    ui->Neck_Edit->setText(QString::number(conf->neckSpeed));
    ui->Elbow_Edit->setText(QString::number(conf->elbowSpeed));
    ui->waist_Edit->setText(QString::number(conf->waistSpeed));

    //set sliders values
    ui->platformForward_Slider->setValue(conf->platformForwardSpeed);
    ui->platformRotate_Slider->setValue(conf->platformRotateSpeed);
    ui->Shoulder_Slider->setValue(conf->shouldersSpeed);
    ui->Neck_Slider->setValue(conf->neckSpeed);
    ui->Elbow_Slider->setValue(conf->elbowSpeed);
    ui->Waist_Slider->setValue(conf->waistSpeed);
}

SettingsDialog::~SettingsDialog()
{
    //config is not deleted because it is handled in Robot class
    delete ui;
}

/**
 * ALL METHODS BELOW DO THE SAME: GET VALUE FROM SLIDER AND SET IT TO INPUTS
 * START SECTION
 * ======================================================================
 */
void SettingsDialog::on_platformForward_Slider_valueChanged(int value)
{
    ui->platformForward_Edit->setText(QString::number(value));
}


void SettingsDialog::on_platformRotate_Slider_valueChanged(int value)
{
    ui->platformRotate_Edit->setText(QString::number(value));
}

void SettingsDialog::on_Neck_Slider_valueChanged(int value)
{
    ui->Neck_Edit->setText(QString::number(value));
}

void SettingsDialog::on_Elbow_Slider_valueChanged(int value)
{
    ui->Elbow_Edit->setText(QString::number(value));
}

void SettingsDialog::on_Waist_Slider_valueChanged(int value)
{
    ui->waist_Edit->setText(QString::number(value));
}

void SettingsDialog::on_Shoulder_Slider_valueChanged(int value)
{
    ui->Shoulder_Edit->setText(QString::number(value));
}

/**
 * END SECTION
 * =====================================================================
 */

/**
 * @brief SettingsDialog::on_buttonBox_accepted
 * Save values to configuration object
 */
void SettingsDialog::on_buttonBox_accepted()
{
    conf->platformForwardSpeed = ui->platformForward_Slider->value();
    conf->platformRotateSpeed = ui->platformRotate_Slider->value();
    conf->elbowSpeed = ui->Elbow_Slider->value();
    conf->shouldersSpeed = ui->Shoulder_Slider->value();
    conf->neckSpeed = ui->Neck_Slider->value();
    conf->waistSpeed = ui->Waist_Slider->value();
}
