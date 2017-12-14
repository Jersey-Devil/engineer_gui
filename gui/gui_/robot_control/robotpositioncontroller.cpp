#include "robotpositioncontroller.h"
#include "myudpclient.h"
#include "robotpackets.h"
#include "QDebug"
#include "QObject"
#include "robot.h"
#include "robotsi.h"

int comparePosition(int, int);
int abs(int);

RobotPositionController::RobotPositionController(Robot *r): RobotController(r)
{
    robot = r;
    hasTask = false;
    positionInfo = new TelemetryPacket;
}

RobotPositionController::~RobotPositionController(){
    delete positionInfo;
}

int RobotPositionController::getAngleById(int id, int position)
{
    switch (id) {
    case 4: //elbow
        return (-216.9 + ((double)(position - 23500) / (63000 - 23500)) * (216.9));
    case 5: //neck
        return (188.8 - ((double)(position - 7900) / (42000 - 7900)) * 188.8);
    case 6: //shoulder
        return (-16.4 + ((double)(position - 40500) / (64400 - 40500)) * (114.2 + 16.4));
    case 7: //waist
        return (-214.2 + ((double)(position - 1000) / (65000 - 1000)) * (137.3 + 214.2));
    case 10: //flippers
        return (230 - ((double)(position - 1070) / (64300 - 1070)) * (230 + 93));
    default:
        return 0;
        //....
    }
}

void RobotPositionController::setFlippersAngle(int angle)
{
    this->flippersAngle = angle;
}

int RobotPositionController::getFlippersAngle()
{
    return this->flippersAngle;
}

void RobotPositionController::setWaistAngle(int angle)
{
    this->waistAngle = angle;
}

int RobotPositionController::getWaistAngle()
{
    return this->waistAngle;
}

void RobotPositionController::setElbowAngle(int angle)
{
    this->elbowAngle = angle;
}

int RobotPositionController::getElbowAngle()
{
    return this->elbowAngle;
}

void RobotPositionController::setShoulderAngle(int angle)
{
    this->shoulderAngle = angle;
}

void RobotPositionController::setNeckAngle(int angle)
{
    this->neckAngle = angle;
}

void RobotPositionController::startTask()
{
    this->hasTask = true;
    qDebug() << "task started";
}

void RobotPositionController::stopTask()
{
    this->hasTask = false;
    qDebug() << "task stopped";
}

void RobotPositionController::handleTelemetry(char *data){
    qDebug() << "telemetry handled";
    delete positionInfo;
    positionInfo = (TelemetryPacket*) data;
    if (hasTask) {
        qDebug() << "executing task";
        evaluateTask();
    } else {
        qDebug() << "updating angles";
        updateAngles();
    }
}

void RobotPositionController::updateAngles()
{
    for (u_int8_t i = 0; i < positionInfo->NUMBER_OF_MOTORS; ++i) {
        if (!hasPositionData(positionInfo->M_DATA[i].DEVICE_ID)) continue;
        setAngleByMotorId(positionInfo->M_DATA[i].DEVICE_ID, positionInfo->M_DATA[i].POSITION);
    }
}

void RobotPositionController::evaluateTask()
{
    for (u_int8_t i = 0; i < positionInfo->NUMBER_OF_MOTORS; ++i) {
        if (!hasPositionData(positionInfo->M_DATA[i].DEVICE_ID)) continue;
        int speed = 0;
        int cmp = comparePosition(positionInfo->M_DATA[i].POSITION, getMotorPositionByAngle(positionInfo->M_DATA[i].DEVICE_ID));
        if (cmp == 1) speed = -5000;
        else if (cmp == 0) speed = 0;
        switch (positionInfo->M_DATA[i].DEVICE_ID) {
       /* case 4: //elbow
            this->elbowNeck(speed);
            break;
        case 5: //neck
            this->neck(speed);
            break;
        case 6: //shoulder
            this->waistUpDown(speed);
            break;
        case 7: //waist
            this->waist(speed);
            break;*/
        case 10: //flippers
            if (speed < 0) this->setFlippersDown();
            if (speed == 0) this->stopFlippers();
            if (speed > 0) this->setFlippersUp();
        default:
            break;
        }
    }
}

int comparePosition(int current, int desired) {
    int treshold = 1000;
    if (abs(current - desired) < treshold) return 0;
    if (current > desired) return 1;
    return -1;
}

int abs(int n) {
    if (n < 0) return -n;
    return n;
}

int RobotPositionController::getMotorPositionByAngle(int id)
{
    switch (id) {
    case 4: //elbow
        return 23500 + ((elbowAngle + 216.9) / (216.9)) * (63000 - 23500);
    case 5: //neck
        return 7900 + ((188.8 - neckAngle) / (188.8)) * (42000 - 7900);
    case 6: //shoulder
        return 40500 + ((shoulderAngle + 16.4) / (114.2 + 16.4)) * (64400 - 40500);
    case 7: //waist
        return 1000 + ((waistAngle + 214.2) / (137.3 + 214.2)) * (65000 - 1000);
    case 10: //flippers
        return 1070 + ((230 - flippersAngle) / (230.0 + 93.0)) * (64300 - 1070);
    default:
        return 0;
        //....
    }
}

void RobotPositionController::setAngleByMotorId(int id, int position)
{
    int angle = getAngleById(id, position);
    switch (id) {
    case 4: //elbow
        elbowAngle = angle;
        break;
    case 5: //neck
        neckAngle = angle;
        break;
    case 6: //shoulder
        shoulderAngle = angle;
        break;
    case 7: //waist
        waistAngle = angle;
        break;
    case 10: //flippers
        flippersAngle = angle;
        break;
    default:
        break;
        //....
    }
}

bool RobotPositionController::hasPositionData(int id)
{
    return id == 4 || id == 5 || id == 6 || id == 7 || id == 10;
}


/*void RobotPositionController::rotateWaist(){
    qDebug("ENTERED");

    int delta = executePositionValue(angle);
    if(deltaApproximateEquality(this->startTelemetry, positionInfo->M_DATA[5].POSITION, delta)) {
      timer->stop();
      robot->stopAll();
    }
}

void RobotPositionController::rotateElbow(){
    int delta = executePositionValue(angle);
    if(deltaApproximateEquality(this->startTelemetry, positionInfo->M_DATA[2].POSITION, delta)) {
      timer->stop();
      robot->stopAll();
    }
}

void RobotPositionController:: startTimerTask(int angle){
    //this->startTelemetry = positionInfo->M_DATA[5].POSITION;
    this->startTelemetry = positionInfo->M_DATA[2].POSITION;
    this->angle = angle;
    timer->start(100);
//    if ( angle > 0 )
//        robot->turnWaist(10000);
//    else robot->turnWaist(-10000);
        if ( angle > 0 )
            robot->turnElbowAndNeck(10000);
        else robot->turnElbowAndNeck(-10000);
}

int RobotPositionController::executePositionValue(int angle){
    return ((65535*angle)/360);
}

bool RobotPositionController::deltaApproximateEquality(int first_telemtry, int current_telemetry, int angle_delta){
    if (angle_delta>0){
    if((std::abs(current_telemetry-first_telemtry)+ANGLE_DELTA) > angle_delta) return true;
        return false;
    }else
    {
        if((-std::abs(current_telemetry-first_telemtry)-ANGLE_DELTA) < angle_delta) return true;
            return false;

    }
}*/
