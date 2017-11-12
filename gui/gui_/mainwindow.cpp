#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "robot.h"
#include <QKeyEvent>
#include <QStandardItem>
#include <robotPackets.h>
#include <robotcontroller.h>
#include <QProgressDialog>
#include <QThread>
#include <QString>
#include <QtDebug>
using namespace std;

/**
 * @brief MainWindow::MainWindow
 * @param parent
 * This is main window UI, shown after program is started
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //init form, used to control robot via windows
    form = new JointForm;

    //The object, using which the manipulations are made
    robot = new Robot();

    //settings, like speed, see settingsdialog.h for details
    settings = new SettingsDialog(this,robot->configuration);

    //connect robot's signal when position is changed
    //see updClient, where this signal is emitted
    connect(robot,SIGNAL(telemetryChanged(char*)),this,SLOT(setTelemetry(char*)));

    //needed to control hot keys
    qApp->installEventFilter(this);

    //dialog shown while connecting to robot
    //useless shit, I wanna remove it
//    dialog = new QProgressDialog("Connecting to robot...", "Cancel", 0, 0);
//    dialog->setWindowModality(Qt::WindowModal);

    //telemetry values init
    QTableWidget *widget = ui->telemetryView;
    for(int i = 0; i < 10; ++i){
        widget->setItem(i,0,new QTableWidgetItem());
        widget->setItem(i,1,new QTableWidgetItem());
    }

    //object to control robot via position and angles
    posController = robot->positionController;

    //object to control robot via speed values
    RobotController *c = robot->controller;

    //called when first telemetry packets got, so hide dialog and change button
    //text from "Connect" to "Disconnect"
    connect(robot->controller,SIGNAL(connectedToRobot()),this,SLOT(connectedToRobotUI()));

    //init the values of telemtry view
    QStringList *list  = new QStringList();
    *list<<"grippersR"           //1
        <<"shoulder"             //2
       <<      "elbow"           //3
      << "neck"               //4
     <<"grippersF"            //5
    <<"waist"                //6
    <<"platformRight"        //7
    <<"platformLeft"         //8
    <<"flippers"             //9
    <<"Light";               //10
    widget->setVerticalHeaderLabels(*list);

}


/**
 * @brief MainWindow::validateValues
 * method called when user pressed "Accept" button to
 * control robot via windows
 */
void MainWindow::validateValues(){
    form->elbow = validateValue(ui->elbowLineEdit->text());
    form->neck = validateValue(ui->neckLineEdit->text());
    form->shoulder = validateValue(ui->shoulderLineEdit->text());
    form->waist = validateValue(ui->waistLineEdit->text());
    form->platformF = validateValue(ui->platformForwardLineEdit->text());
    form->platformR = validateValue(ui->platformRLineEdit->text());

}

/**
 * @brief MainWindow::validateValue
 * @param value - value from input window
 * @return int value, which is speed
 */
int MainWindow::validateValue(QString value){
    if(value.isNull()|| value.isEmpty())
        return 0;
    return value.toInt();
}




//Destructor
MainWindow::~MainWindow()
{
//    delete dialog;
    delete form;
    delete ui;
    delete robot;
    delete settings;
}

//slot called to handle Light checkbox
//calls robot to turn light
void MainWindow::on_lightToggle_clicked()
{
    isLight = !isLight;
    //temporary diabled
//    robot->turnLight();
}

/**
 * @brief MainWindow::on_connectButton_clicked
 * Slot called to handle connect button click
 * starts connecting or disconnecting
 */
void MainWindow::on_connectButton_clicked()
{
    if (!robot->isConnected && !robot->isConnecting){
        robot->connectToEngineer();
        ui->connectButton->setText("Connecting...");
        //show dialog
        //it is hidden after udpclient emits signal
//        dialog->exec();
    }
    else{
        robot->disconnectFromEngineer();
        setEnabledAllControls(false);
        ui->connectButton->setText("Connect");
    }
}

/**
 * @brief MainWindow::on_gripper_valueChanged
 * @param value is dial's value:
 *  -1, which is down
 *   0, which is stop
 *   1, which is up
 */
void MainWindow::on_gripper_valueChanged(int value)
{
    robot->gripper(value);
}

/**
 * @brief MainWindow::on_flipper_valueChanged
 * @param value the same as gripper's slot
 */
void MainWindow::on_flipper_valueChanged(int value)
{

    robot->flippers(value);
}

//handles window inputs
//the main problem is AXIS and INPUT collision
//see servosila documents
void MainWindow::on_acceptForms_clicked()
{
    validateValues();
    if(form->platformF>0){
        robot->moveD(form->platformF);
        if(form->platformR>0)
            robot->moveR(form->platformR);
    }
    else{
        if(form->platformR>0)
            robot->moveR(form->platformR);
        else{
            robot->moveWaist(form->shoulder);
            if (form->waist>0)
                robot->turnWaist(form->waist);
        }
    }


    if(form->elbow>0)
        robot->turnElbowAndNeck(form->elbow);
    else
        robot->turnNeck(form->neck);
}



/**
 * @brief MainWindow::on_platformF_valueChanged
 * @param value - platform slider's value (from 0 to 100)
 * it is vertical
 */
void MainWindow::on_platformF_valueChanged(int value)
{
    if (value%10==0) {
        if (value==50)
            robot->controller->stopPlatformD();
        else
            robot->moveD(getRealSpeed(value, robot->configuration->platformForwardSpeed));
    }
}


//settings button slot
void MainWindow::on_settings_clicked()
{
    //show dialog, see settingsdialog.h
    settings->show();


}

/**
 * @brief MainWindow::on_platformR_valueChanged
 * @param value - platformR slider's value (from 0 to 100)
 * 0-left, 100-right
 * calls robot's moveR method
 */
void MainWindow::on_platformR_valueChanged(int value)
{
    if (value%10==0) {
        if (value==50)
            robot->controller->stopPlatformR();
        else
            robot->moveR(getRealSpeed(value, robot->configuration->platformRotateSpeed));
    }
}

//handles hot keys
bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    if (event->type()==QEvent::KeyPress || event->type()==QEvent::KeyRelease) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        qDebug() << "type: " << event->type() << "\n"
                 << "text: " << key->text() << "\n"
                 << "count: " << key->count() << "\n"
                 << "isRepeat: "<< key->isAutoRepeat() << "\n";

        if (key->isAutoRepeat()) {
            switch (key->key()) {
            case Qt::Key_W:
                ui->platformF->setValue(ui->platformF->maximum());
                break;
            case Qt::Key_S:
                ui->platformF->setValue(ui->platformF->minimum());
                break;
            case Qt::Key_A:
                ui->platformR->setValue(ui->platformR->minimum());
                break;
            case Qt::Key_D:
                ui->platformR->setValue(ui->platformR->maximum());
                break;
            default:
                break;
            }
            event->accept();
            return true;
        }
        //PANIC button handler
        if ((key->key()==Qt::Key_Space) || event->type() == QEvent::KeyRelease) {
            on_stopAll_clicked();
        } else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

//calls robot to stop
//sets UI elements to start
void MainWindow::on_stopAll_clicked()
{
    robot->stopAll();
    setSlidersToStart();
    setInputToZero();
}

/**
 * @brief MainWindow::setSlidersToStart
 * sets all sliders positions to start values
 */
void MainWindow::setSlidersToStart(){
    ui->waistUpDown->setValue(50);
    ui->waistLeftRight->setValue(50);
    ui->elbowSlider->setValue(50);
    ui->neckSlider->setValue(50);
    ui->gripper->setValue(0);
    ui->flipper->setValue(0);
    ui->platformF->setValue(50);
    ui->platformR->setValue(50);
}

//sets input values to zero
void MainWindow::setInputToZero(){
    ui->elbowLineEdit->setText("0");
    ui->neckLineEdit->setText("0");
    ui->shoulderLineEdit->setText("0");
    ui->waistLineEdit->setText("0");
    ui->platformForwardLineEdit->setText("0");
    ui->platformRLineEdit->setText("0");
}

/**
 *  THE NEXT SECTION HAS SIMILAR CODE
 *  THE DESCRIPTION IS ALSO SIMILAR
 *  these methods are slots, they handle user's slider moves
 *  is it is placed to zero, then movement is made
 *  START SECTION
 *  ======================================================================================
 */
void MainWindow::on_waistLeftRight_valueChanged(int value)
{
    if (value%10==0){
        if (value==50)
            robot->controller->stopWaist();
        else
            robot->turnWaist(getRealSpeed( value, robot->configuration->waistSpeed));
    }
}

//elbow and neck joints move
void MainWindow::on_elbowSlider_valueChanged(int value)
{
    if (value%10==0){
        if (value==50)
            robot->controller->stopElbowNeck();
        else
            robot->turnElbowAndNeck(getRealSpeed( value,robot->configuration->elbowSpeed));
    }
}

void MainWindow::on_neckSlider_valueChanged(int value)
{
    if (value%10==0){
        if (value==50)
            robot->controller->stopNeck();
        else
            robot->turnNeck(getRealSpeed( value, robot->configuration->neckSpeed));
    }
}

//also known as shoulder
void MainWindow::on_waistUpDown_valueChanged(int value)
{
    if(value%10==0){
        if (value==50)
            robot->controller->stopWaistUpDown();
        else
            robot->moveWaist(getRealSpeed(value,robot->configuration->shouldersSpeed));
    }
}
/**
 * END SECTION
 * ====================================================================================
 */


/**
 * @brief MainWindow::setTelemetry
 * this method is slot, called when UDPCLient got telemetry from robot
 * it converts data char* to Telemetry Packet
 *
 * @param data - char*, but is converted to TelemtryPacket
 * see robotPackets.h for its structure
 *
 * NOTE: it does not delete data pointer, because it is also used in
 * RobotPositionController class
 */
void MainWindow::setTelemetry(char *data){
    //qDebug(data);
    TelemetryPacket *packet = (TelemetryPacket*)data;
    QTableWidget *widget = ui->telemetryView;
    for(int i = 0; i < 10; ++i){
        QTableWidgetItem *itemSpeed = widget->item(i,0);
        QTableWidgetItem *itemPosition = widget->item(i,1);
        int robotSpeed = packet->M_DATA[i].SPEED;
        int robotPosition = packet->M_DATA[i].POSITION;
        itemSpeed->setText(QString::number(robotSpeed));
        itemPosition->setText(QString::number(robotPosition));
        ui->telemetryView->item(i,0)->setText(QString::number(packet->M_DATA[i].SPEED));
        ui->telemetryView->item(i,1)->setText(QString::number(packet->M_DATA[i].POSITION));
    }
    //delete packet;
}

/**
 * @brief MainWindow::connectedToRobotUI
 * slot to handle robot connection event
 */
void MainWindow::connectedToRobotUI(){
    ui->connectButton->setText("Disconnect");
    robot->isConnected = true;
    robot->isConnecting = false;
    setEnabledAllControls(true);
//    dialog->hide();
}

//called when robot connected with true value and with false when disconnected
void MainWindow::setEnabledAllControls(bool v){

    //disable sliders
    ui->waistUpDown->setEnabled(v);
    ui->elbowSlider->setEnabled(v);
    ui->waistLeftRight->setEnabled(v);
    ui->gripper->setEnabled(v);
    ui->flipper->setEnabled(v);
    ui->neckSlider->setEnabled(v);
    ui->platformF->setEnabled(v);
    ui->platformR->setEnabled(v);

    //disable windows
    ui->elbowLineEdit->setEnabled(v);
    ui->neckLineEdit->setEnabled(v);
    ui->shoulderLineEdit->setEnabled(v);
    ui->waistLineEdit->setEnabled(v);
    ui->platformForwardLineEdit->setEnabled(v);
    ui->platformRLineEdit->setEnabled(v);
}

/**
 * @brief MainWindow::getRealSpeed
 * Converts values from UI(sliders) to robot values
 * see Servosila Documentation
 *
 * @param speed - the value from slider (from 0 to 100)
 * @param maxSpeed - the speed from RobotConfiguration object
 * @return the robot kind speed (max range is from -32000 to 32000)
 */
int MainWindow::getRealSpeed(int speed, int maxSpeed){
    int realSpeed = 0;
    realSpeed = (speed-50)*(maxSpeed/50);
    return realSpeed;
}

void MainWindow::on_acceptButton_clicked()
{
    posController->startTimerTask(ui->elbowAngle->text().toInt());
}
