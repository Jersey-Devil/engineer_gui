#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "robotconfiguration.h"

namespace Ui {
class SettingsDialog;
}

/*
 * This UI is Dialog style
 * Handles the robot speed configuration and others
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0, RobotConfiguration *c=NULL);
    ~SettingsDialog();

    //UI Slots
    //Called when user moved sliders
    //got value from 5000 to 32000, the speed of each joint
private slots:
    void on_platformForward_Slider_valueChanged(int value);

    void on_platformRotate_Slider_valueChanged(int value);

    void on_Neck_Slider_valueChanged(int value);

    void on_Elbow_Slider_valueChanged(int value);

    void on_Waist_Slider_valueChanged(int value);

    void on_Shoulder_Slider_valueChanged(int value);

    void on_buttonBox_accepted();

private:
    //configuration, which is got from Robot class
    //after the settings are set and button "OK" is pressed
    //settings are set to this pointer
    RobotConfiguration *conf;

    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
