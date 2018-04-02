#include "robot.h"
#include <robotcontroller.h>
#include <robotpackets.h>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include "robotpositioncontroller.h"

namespace QAppPriv
{
    static int argc = 1;
    static char appname[] = "robot_control.so";
    static char * argv[] = {appname, nullptr};
    static QCoreApplication * pApp = nullptr;
    static QThread * pThread = nullptr;
}
/**
 * @brief Robot::Robot
 * Main object to control robot
 */
Robot::Robot():QObject()
{
    if (QAppPriv::pThread == nullptr)
    {
        // Separate thread for application thread
        QAppPriv::pThread = new QThread();
        // Direct connection is mandatory
        connect(QAppPriv::pThread, SIGNAL(started()), this, SLOT(onExec()), Qt::DirectConnection);
        QAppPriv::pThread->start();
    }
    //controller based on speed values
//    controller = new RobotController(this);
//    controller = nullptr;

    //robot's speed configurations
    //can be added in Settings window
    configuration = new RobotConfiguration("default");

    //controller based on position values
    controller = new RobotPositionController(this);
//    positionController = nullptr;


    //handle client's got telemetry from robot
    connect(this, SIGNAL(telemetryChanged(char*)),controller, SLOT(handleTelemetry(char*)));
}

//move robot's platform in direct way
//so both servo motors are moved in the same direction
void Robot::moveD(int speed){
    //minus because forward is <(lt)0, back is >(gt)0
    controller->movePlatformDirect(-speed);
}

//move robot's platform in rotational way
//robot's motors start to move in different directions
//can be used together with moveD() method to let the robot move in circle
void Robot::moveR(int speed){
    controller->movePlatformRotate(speed);
}

//turn robot's light
//not work after first attempt
void Robot::turnLight(){
    controller->turnLight();
}

//move robot's flippers, if direction -1, then down, 0-stop, 1-up
void Robot::flippers(int direction){
    controller->stopTask();
    switch (direction){
    case 1: controller->setFlippersUp(); break;
    case -1: controller->setFlippersDown(); break;
    default: controller->stopFlippers(); break;
    }
}

//opens robot's gripper(actually they go back)
void Robot::openGripper(){
    controller->stopTask();
    controller->gripper(true);
}

//closes gripper, so it can hold objects
void Robot::closeGripper(){
    controller->stopTask();
    controller->gripper(false);
}

//return config
RobotConfiguration* Robot::getConfiguration(){
    return configuration;
}

void Robot::onExec()
{
    if (QCoreApplication::instance() == nullptr)
    {
        QAppPriv::pApp = new QCoreApplication(QAppPriv::argc, QAppPriv::argv);
        QAppPriv::pApp->exec();
        if (QAppPriv::pApp)
            delete QAppPriv::pApp;
    }
}

void Robot::connected()
{
    if (!this->isConnected && this->isConnecting) {
        this->isConnected = true;
        this->isConnecting = false;
    }
}

/**
 * @brief Robot::turnElbowAndNeck
 * turns both elbow and neck joints
 * so robot's hand goes up
 * @param speed - the speed with which they are moved
 *
 * NOTE: you can't move only elbow
 */
void Robot::turnElbowAndNeck(int speed){
    controller->stopTask();
    controller->elbowNeck(-speed);
}

//turn only neck(head joint)
//NOTE: dangerous move
void Robot::turnNeck(int speed){
    controller->stopTask();
    controller->neck(-speed);
}

/**
 * @brief Robot::turnWaist
 * turns robot's waist left and right
 * NOTE: you can't move platform and waist because of collision of AXIS and BUTTON values
 * @param speed
 */
void Robot::turnWaist(int speed){
    controller->stopTask();
    qDebug() << "turned waist speed " << speed;
    controller->waist(speed);
}

/**
 * @brief Robot::moveWaist
 * here waist is the same as shoulder
 * it moves hand up or down
 * NOTE: this may be the most dangerous movement
 * @param speed
 */
void Robot::moveWaist(int speed){
    controller->stopTask();
    controller->waistUpDown(speed);
}

//opens or closes gripper
void Robot::gripper(int direction){
    controller->stopTask();
    switch (direction) {
    case 1: controller->gripper(true); break;
    case -1: controller->gripper(false); break;
    default: controller->stopGripper();
        break;
    }
}

//stops all motors
//used for PANIC button
void Robot::stopAll(){
    controller->stopTask();
    controller->stopElbowNeck();
    controller->stopNeck();
    controller->stopWaist();
    controller->stopWaistUpDown();
    controller->stopPlatformD();
    controller->stopPlatformR();
    controller->stopFlippers();
    controller->stopGripper();
}

Robot::~Robot(){

    delete controller;
    delete configuration;
//    if (positionController != nullptr) delete positionController;
    //fixme position controller
}

//set client to connect to robot
//see RobotController class for details
void Robot::connectToEngineer(){
    if (!this->isConnected && !this->isConnecting) {
        this->isConnecting = true;
        emit controller->connectClient();
    }
}

void Robot::disconnectFromEngineer()
{
    if (this->isConnected || this->isConnecting) {
        this->isConnecting = false;
        this->isConnected = false;
        emit controller->disconnectClient();
    }
}


