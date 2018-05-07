#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "robot.h"
#include "robotsi.h"
#include <QKeyEvent>
#include <QStandardItem>
#include "robotpackets.h"
#include "robotcontroller.h"
#include <cmath>
#include <QThread>
#include <QString>
#include <QtDebug>
#include "model.h"
//using namespace std;

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
    renderWidget = ui->renderWidget;
//    RobotPositionController *c = new RobotPositionController(nullptr);


    //The object, using which the manipulations are made
//    robot = new Robot();
    robot = new RobotSI();
    connect(robot, SIGNAL(videoFrameSended(char*, int)), this, SLOT(handleVideoFrame(char*, int)));
    //settings, like speed, see settingsdialog.h for details
    settings = new SettingsDialog(this,robot->configuration);
    renderSettings = new RenderSettings(this, renderWidget);
    //connect robot's signal when position is changed
    //see udpClient, where this signal is emitted
    connect(robot,SIGNAL(telemetryChanged(char*)),this,SLOT(setTelemetry(char*)));

    //needed to control hot keys
    qApp->installEventFilter(this);

    //dialog shown while connecting to robot
    //useless shit, I wanna remove it
//    dialog = new QProgressDialog("Connecting to robot...", "Cancel", 0, 0);
//    dialog->setWindowModality(Qt::WindowModal);


    //object to control robot via position and angles
    controller = robot->controller;

    //object to control robot via speed values
//    RobotController *c = robot->controller;

    //called when first telemetry packets got, so hide dialog and change button
    //text from "Connect" to "Disconnect"
    connect(robot->controller,SIGNAL(connectedToRobot()),this,SLOT(connectedToRobotUI()));
    connect(controller, SIGNAL(connectionDrop()),this, SLOT(robotDisconnect()));
    //init the values of telemtry view

    QStringList lst;
    lst << "Gripper(fingers)" << "Gripper(rotation)" << "Elbow" << "Neck" << "Shoulder" << "Waist" << "Right drive"
        << "Left drive" << "Flippers";
    QStringList list;
    list << "ID" << "State" << "Op. mode" << "Position" << "Angle" << "Speed" << "Speed(m/s)" << "Amps";
    QTableWidget *md = ui->motorData;
    md->setColumnCount(8);
    md->setShowGrid(true);
    md->setRowCount(9);
    md->setVerticalHeaderLabels(lst);
    md->setHorizontalHeaderLabels(list);


    setEnabledAllControls(false);

}


/**
 * @brief MainWindow::validateValues
 * method called when user pressed "Accept" button to
 * control robot via windows
 */
void MainWindow::validateValues(){
    form->elbow = validateValue(ui->elbowLineEdit->text(), robot->configuration->elbowSpeed);
//    ui->elbowLineEdit->setText(QString::number(form->elbow));
    form->neck = validateValue(ui->neckLineEdit->text(), robot->configuration->neckSpeed);
//    ui->neckLineEdit->setText(QString::number(form->neck));
    form->shoulder = validateValue(ui->shoulderLineEdit->text(), robot->configuration->shouldersSpeed);
//    ui->shoulderLineEdit->setText(QString::number(form->shoulder));
    form->waist = validateValue(ui->waistLineEdit->text(), robot->configuration->waistSpeed);
//    ui->waistLineEdit->setText(QString::number(form->waist));
    form->platformF = validateValue(ui->platformForwardLineEdit->text(), robot->configuration->platformForwardSpeed);
//    ui->platformForwardLineEdit->setText(QString::number(form->platformF));
    form->platformR = validateValue(ui->platformRLineEdit->text(), robot->configuration->platformRotateSpeed);
//    ui->platformRLineEdit->setText(QString::number(form->platformR));
}

/**
 * @brief MainWindow::validateValue
 * @param value - value from input window
 * @return int value, which is speed
 */
int MainWindow::validateValue(QString value, int max){
    qDebug() << value << " value";
    if(value.isNull() || value.isEmpty())
        return 0;
    int v = value.toInt();
    if (v > max) return max;
    if (v < -max) return -max;
    return v;
}




//Destructor
MainWindow::~MainWindow()
{
    disconnect(robot->controller, &RobotPositionController::jointsUpdated, renderWidget, &RenderWidget::jointsUpdated);
//    delete dialog;
    if (robot->isConnected || robot->isConnecting){
        robot->disconnectFromEngineer();
    }
    delete form;
    delete ui;
    delete robot;
    delete settings;
}

void MainWindow::loadModel()
{
//    QString filename = "/home/avispa/Workspace/Engineer_Gui/3d_model/robot_test1.dae";
    QString filename = "/home/avispa/untitled1.dae";

//        Model m;
//        m.loadFromFilename(filename.toStdString());

        connect(robot->controller, &RobotPositionController::jointsUpdated, renderWidget, &RenderWidget::jointsUpdated);
        renderWidget->mScene.mainModel.loadFromFilename(filename.toStdString());
        renderWidget->mRenderer.cleanUp();
        renderWidget->mScene.mainCamera.reset();
        renderWidget->initializeGL();
        renderWidget->resizeGL(renderWidget->width(), renderWidget->height());
        renderWidget->update();
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

inline int getSliderSpeed(int speed, int max) {
    if (max > 32767) max = 32767;
    return 50 * speed / max + 50;
}

//handles window inputs
//the main problem is AXIS and INPUT collision
//see servosila documents
void MainWindow::on_acceptForms_clicked()
{
    validateValues();
    ui->neckSlider->setValue(getSliderSpeed(form->neck, robot->configuration->neckSpeed));
    ui->neckLineEdit->setText(QString::number(form->neck));
//    if (form->neck != 0) robot->turnNeck(form->neck);
//    else robot->controller->stopNeck();
    ui->elbowSlider->setValue(getSliderSpeed(form->elbow, robot->configuration->elbowSpeed));
    ui->elbowLineEdit->setText(QString::number(form->elbow));
//    if (form->elbow != 0) robot->turnElbowAndNeck(form->elbow);
//    else robot->controller->stopElbowNeck();
    ui->waistUpDown->setValue(getSliderSpeed(form->shoulder, robot->configuration->shouldersSpeed));
    ui->shoulderLineEdit->setText(QString::number(form->shoulder));
//    if (form->shoulder != 0) robot->moveWaist(form->shoulder);
//    else robot->controller->stopWaistUpDown();
    ui->waistLeftRight->setValue(getSliderSpeed(form->waist, robot->configuration->waistSpeed));
    ui->waistLineEdit->setText(QString::number(form->waist));
//    if (form->waist != 0) robot->turnWaist(form->waist);
//    else robot->controller->stopWaist();
    ui->platformF->setValue(getSliderSpeed(form->platformF, robot->configuration->platformForwardSpeed));
    ui->platformForwardLineEdit->setText(QString::number(form->platformF));
//    if (form->platformF != 0) robot->moveD(form->platformF);
//    else robot->controller->stopPlatformD();
    ui->platformR->setValue(getSliderSpeed(form->platformR, robot->configuration->platformRotateSpeed));
    ui->platformRLineEdit->setText(QString::number(form->platformR));
//    if (form->platformR != 0) robot->moveR(form->platformR);
//    else robot->controller->stopPlatformR();

    /*if(form->platformF>0){
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
        robot->turnNeck(form->neck);*/
}



/**
 * @brief MainWindow::on_platformF_valueChanged
 * @param value - platform slider's value (from 0 to 100)
 * it is vertical
 */
void MainWindow::on_platformF_valueChanged(int value)
{
    ui->platformForwardLineEdit->setText(QString::number(getRealSpeed(value, robot->configuration->platformForwardSpeed)));
//    if (value%10==0) {
    if (true) {
        if (value==50)
            robot->controller->stopPlatformD();
        else //min speed 5000 both forward and rotate
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
    ui->platformRLineEdit->setText(QString::number(getRealSpeed(value, robot->configuration->platformRotateSpeed)));
//    if (value%10==0) {
    if (true) {
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
//        qDebug() << "type: " << event->type() << "\n"
//                 << "text: " << key->text() << "\n"
//                 << "count: " << key->count() << "\n"
//                 << "isRepeat: "<< key->isAutoRepeat() << "\n";

        if (event->type()==QEvent::KeyPress) {
            switch (key->key()) {
            case Qt::Key_W: //forward
                ui->platformF->setValue(ui->platformF->maximum());
                break;
            case Qt::Key_S: //reverse
                ui->platformF->setValue(ui->platformF->minimum());
                break;
            case Qt::Key_A: //rotate left
                ui->platformR->setValue(ui->platformR->minimum());
                break;
            case Qt::Key_D: //rotate right
                ui->platformR->setValue(ui->platformR->maximum());
                break;
            case Qt::Key_Q: //waist rotate left
                ui->waistLeftRight->setValue(ui->waistLeftRight->minimum());
                break;
            case Qt::Key_E: //waist rotate right
                ui->waistLeftRight->setValue(ui->waistLeftRight->maximum());
                break;
            case Qt::Key_F: //flippers up
                ui->flipper->setValue(ui->flipper->maximum());
                break;
            case Qt::Key_R: //flippers down
                ui->flipper->setValue(ui->flipper->minimum());
                break;
            case Qt::Key_G: //grippers close
                ui->gripper->setValue(ui->gripper->minimum());
                break;
            case Qt::Key_T: //grippers open
                ui->gripper->setValue(ui->gripper->maximum());
                break;
            case Qt::Key_U: //elbow up
                ui->elbowSlider->setValue(ui->elbowSlider->maximum());
                break;
            case Qt::Key_J: //elbow down
                ui->elbowSlider->setValue(ui->elbowSlider->minimum());
                break;
            case Qt::Key_H: //shoulder down
                ui->waistUpDown->setValue(ui->waistUpDown->minimum());
                break;
            case Qt::Key_Y: //shoulder up
                ui->waistUpDown->setValue(ui->waistUpDown->maximum());
                break;
            case Qt::Key_K: //neck down
                ui->neckSlider->setValue(ui->neckSlider->minimum());
                break;
            case Qt::Key_I: //neck up
                ui->neckSlider->setValue(ui->neckSlider->maximum());
                break;
            case Qt::Key_Space:
                robot->stopAll();
                setSlidersToStart();
                event->accept();
                return true;
                break;
            default:
                break;
            }
//            event->accept();
            return QObject::eventFilter(obj, event);
//            return true;
        }
        //PANIC button handler
        if (event->type() == QEvent::KeyRelease) {
            switch (key->key()) {
            case Qt::Key_W:
            case Qt::Key_S:
                ui->platformF->setValue(ui->platformF->maximum() / 2);
                break;
            case Qt::Key_A:
            case Qt::Key_D:
                ui->platformR->setValue(ui->platformR->maximum() / 2);
                break;
            case Qt::Key_Q:
            case Qt::Key_E:
                ui->waistLeftRight->setValue(ui->waistLeftRight->maximum() / 2);
                break;
            case Qt::Key_F:
            case Qt::Key_R:
                ui->flipper->setValue(0);
                break;
            case Qt::Key_G:
            case Qt::Key_T:
                ui->gripper->setValue(0);
                break;
            case Qt::Key_U:
            case Qt::Key_J:
                ui->elbowSlider->setValue(ui->elbowSlider->maximum() / 2);
                break;
            case Qt::Key_H:
            case Qt::Key_Y:
                ui->waistUpDown->setValue(ui->waistUpDown->maximum() / 2);
                break;
            case Qt::Key_I:
            case Qt::Key_K:
                ui->neckSlider->setValue(ui->neckSlider->maximum() / 2);
            default:
                break;
            }
        } else {
            return QObject::eventFilter(obj, event);
        }
        return QObject::eventFilter(obj, event);
    } else {
        return QObject::eventFilter(obj, event);
    }
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
    ui->waistLineEdit->setText(QString::number(getRealSpeed(value, robot->configuration->waistSpeed)));
//    if (value%10==0){
    qDebug() << "onWaistChanged value " << value;
    if (true) {
        if (value==50)
            robot->controller->stopWaist();
        else
            robot->turnWaist(getRealSpeed( value, robot->configuration->waistSpeed));
    }
}

//elbow and neck joints move
void MainWindow::on_elbowSlider_valueChanged(int value)
{
    ui->elbowLineEdit->setText(QString::number(getRealSpeed(value, robot->configuration->elbowSpeed)));
//    if (value%10==0){
    if (true) {
        if (value==50)
            robot->controller->stopElbowNeck();
        else
            robot->turnElbowAndNeck(getRealSpeed( value,robot->configuration->elbowSpeed));
    }
}

void MainWindow::on_neckSlider_valueChanged(int value)
{
//    if (value%10==0){
    ui->neckLineEdit->setText(QString::number(getRealSpeed(value, robot->configuration->neckSpeed)));
    if (true) {
        if (value==50)
            robot->controller->stopNeck();
        else
            robot->turnNeck(getRealSpeed( value, robot->configuration->neckSpeed));
    }
}

//also known as shoulder
void MainWindow::on_waistUpDown_valueChanged(int value)
{
    ui->shoulderLineEdit->setText(QString::number(getRealSpeed(value, robot->configuration->shouldersSpeed)));
//    if(value%10==0){
    if (true) {
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
void MainWindow::setTelemetry(char* buffer){
    TelemetryPacket *packet = (TelemetryPacket*) buffer;
    QTableWidget *widget = ui->motorData;
    for (size_t i = 0; i < 9; ++i) {
        widget->setItem(i,0, new QTableWidgetItem(QString::number(packet->M_DATA[i].DEVICE_ID)));
        widget->setItem(i,1, new QTableWidgetItem(QString::number(packet->M_DATA[i].DEVICE_STATE)));
        widget->setItem(i,2, new QTableWidgetItem(QString::number(packet->M_DATA[i].OPERATION_MODE)));
        widget->setItem(i,3, new QTableWidgetItem(QString::number(packet->M_DATA[i].POSITION)));
        widget->setItem(i,4, new QTableWidgetItem(getAngleById(packet->M_DATA[i].DEVICE_ID, packet->M_DATA[i].POSITION))); //angle
        widget->setItem(i,5, new QTableWidgetItem(QString::number(packet->M_DATA[i].SPEED)));
        widget->setItem(i,6, new QTableWidgetItem(getSpeedMsById(packet->M_DATA[i].DEVICE_ID, packet->M_DATA[i].SPEED))); //speed in m/s
        widget->setItem(i,7, new QTableWidgetItem(QString::number(packet->M_DATA[i].AMPS)));
//        widget->setItem(i,6, new QTableWidgetItem(QString::number(packet->M_DATA[i].STATUS_BITS)));
//        widget->setItem(i,7, new QTableWidgetItem(QString::number(packet->M_DATA[i].POSITION_COMMAND)));
//        widget->setItem(i,8, new QTableWidgetItem(QString::number(packet->M_DATA[i].SPEED_COMMAND)));
//        widget->setItem(i,9, new QTableWidgetItem(QString::number(packet->M_DATA[i].AMPS_COMMAND)));
//        widget->setItem(i,10, new QTableWidgetItem(QString::number(packet->M_DATA[i].FAULT_DETECTED)));
//        widget->setItem(i,11, new QTableWidgetItem(QString::number(packet->M_DATA[i].FAULTS_COUNTER)));
    }
}

QString MainWindow::getAngleById(int id, int position) {
    switch (id) {
    case 2: //gripper fingers
    case 3: //gripper rotation
        return QString::fromUtf8("-", 1);
    case 4: //elbow
        return QString::number(-216.9 + ((double)(position - 23500) / (63000 - 23500)) * (216.9));
    case 5: //neck
        return QString::number(188.8 - ((double)(position - 7900) / (42000 - 7900)) * 188.8);
    case 6: //shoulder
        return QString::number(-16.4 + ((double)(position - 40500) / (64400 - 40500)) * (114.2 + 16.4));
    case 7: //waist
        return QString::number(-214.2 + ((double)(position - 1000) / (65000 - 1000)) * (137.3 + 214.2));
    case 8: //right drive
    case 9: //left drive
        return QString::fromUtf8("-", 1);
    case 10: //flippers
        return QString::number(230 - ((double)(position - 1070) / (64300 - 1070)) * (230 + 93));
    default:
        return QString::fromUtf8("-", 1);
    }
}

QString MainWindow::getSpeedMsById(int id, int speed) {
    switch (id) {
    case 8:
    case 9:
        return QString::number(RobotSI::motorSpeedToMsForward(speed));
    default:
        return QString::fromUtf8("-", 1);
    }
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
    ui->elbowAngle->setEnabled(v);
    ui->neckAngle->setEnabled(v);
    ui->shoulderAngle->setEnabled(v);
    ui->waistAngle->setEnabled(v);
    ui->flippersAngle->setEnabled(v);

    ui->acceptButton->setEnabled(v);
    ui->stopAll->setEnabled(v);
    ui->stop_all_position_Button->setEnabled(v);
    ui->acceptForms->setEnabled(v);
    ui->lightToggle->setEnabled(v);
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
    if (maxSpeed > 32767) maxSpeed = 32767;
    int realSpeed = 0;
    realSpeed = (speed-50)*(maxSpeed/50);
    return realSpeed;
}

void MainWindow::on_acceptButton_clicked()
{
    if (ui->neckAngle->text() != "" && controller->setNeckAngle(ui->neckAngle->text().toDouble()));
    else {
        controller->clearNeckAngle();
        ui->neckAngle->setText("");
    }
    if (ui->shoulderAngle->text() != "" && controller->setShoulderAngle(ui->shoulderAngle->text().toDouble()));
    else {
        controller->clearShoulderAngle();
        ui->shoulderAngle->setText("");
    }
    if (ui->elbowAngle->text() != "" && controller->setElbowAngle(ui->elbowAngle->text().toDouble()));
    else {
        controller->clearElbowAngle();
        ui->elbowAngle->setText("");
    }
    if (ui->waistAngle->text() != "" && controller->setWaistAngle(ui->waistAngle->text().toDouble()));
    else {
        controller->clearWaistAngle();
        ui->waistAngle->setText("");
    }
    if (ui->flippersAngle->text() != "" && controller->setFlippersAngle(ui->flippersAngle->text().toDouble()));
    else {
        controller->clearFlippersAngle();
        ui->flippersAngle->setText("");
    }
}

void MainWindow::handleVideoFrame(char *data, int length)
{
    VideoFramePacket *p = (VideoFramePacket*) data;
    qDebug() << "received video frame with size " << length;
    delete p;
}


void MainWindow::on_stop_all_position_Button_clicked()
{
    robot->stopAll();
    ui->elbowAngle->setText("");
    ui->neckAngle->setText("");
    ui->shoulderAngle->setText("");
    ui->waistAngle->setText("");
    ui->flippersAngle->setText("");
}

void MainWindow::on_resetPositionButton_clicked()
{
    renderWidget->mScene.mainModel.resetModelMatrix();
    renderWidget->mScene.mainCamera.reset();
    renderWidget->update();
}

void MainWindow::on_settingsButton_clicked()
{
    renderSettings->show();
}

void MainWindow::robotDisconnect()
{
    on_stopAll_clicked();
    on_stop_all_position_Button_clicked();
    setEnabledAllControls(false);
    robot->disconnectFromEngineer();
    ui->connectButton->setText("Connecting...");
    robot->connectToEngineer();
}

/*{
    on_stop_all_position_Button_clicked();
    ui->elbowAngle->setText("0");
    ui->neckAngle->setText("12");
    ui->shoulderAngle->setText("0");
    ui->waistAngle->setText("0");
    ui->flippersAngle->setText("175");
    on_acceptButton_clicked();
}*/
