#include "robotpositioncontroller.h"
#include "myudpclient.h"
#include "robotpackets.h"
#include "QDebug"
#include "QObject"
#include "robot.h"
#include "robotsi.h"

RobotPositionController::RobotPositionController(Robot *r): RobotController(r)
{
    robot = r;
    hasTask = false;
    positionInfo = new TelemetryPacket;
}

RobotPositionController::~RobotPositionController(){
    delete positionInfo;
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

void RobotPositionController::evaluateTask()
{
    this->hasTask = true;
}

void RobotPositionController::handleTelemetry(char *data){
    qDebug() << "telemetry handled";
    delete positionInfo;
    positionInfo = (TelemetryPacket*) data;
    if (hasTask) {

    } else {
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

void RobotPositionController::setAngleByMotorId(int id, int position)
{
    switch (id) {
    case 4: //elbow
        elbowAngle = (-216.9 + ((double)(position - 23500) / (63000 - 23500)) * (216.9));
        break;
    case 5: //neck
        neckAngle = (188.8 - ((double)(position - 7900) / (42000 - 7900)) * 188.8);
        break;
    case 6: //shoulder
        shoulderAngle = (-16.4 + ((double)(position - 40500) / (64400 - 40500)) * (114.2 + 16.4));
        break;
    case 7: //waist
        waistAngle = (-214.2 + ((double)(position - 1000) / (65000 - 1000)) * (137.3 + 214.2));
        break;
    case 10: //flippers
        flippersAngle = (230 - ((double)(position - 1070) / (64300 - 1070)) * (230 + 93));
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
