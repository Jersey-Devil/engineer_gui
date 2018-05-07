#include "robotpositioncontroller.h"
#include "myudpclient.h"
#include "robotpackets.h"
#include "QDebug"
#include "QObject"
#include "robot.h"
#include "robotsi.h"

int comparePosition(int, int, int id);
int abs(int) noexcept;
int calcSpeed(int, int, int, RobotPositionController*);

RobotPositionController::RobotPositionController(Robot *r): RobotController(r)
{
    joints = 0U;
    j = new Joints;
    positionInfo = new TelemetryPacket;
}

RobotPositionController::~RobotPositionController(){
    delete positionInfo;
}

double RobotPositionController::getAngleById(int id, int position) //grad
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
        qDebug() << "wrong id in getAngleById";
        return 0;
    }
}

bool checkAngleRange(double start, double finish, double checked) {
    return (checked >= start) && (checked <= finish);
}

bool RobotPositionController::setFlippersAngle(double angle)
{
    if (!checkAngleRange(-93.0, 230.0, angle)) return false;
    this->flippersAngle = angle;
    joints |= 0b10000U;
    qDebug() << "set flippers, joints = " << QString::number(joints,2);
    return true;
}

void RobotPositionController::clearFlippersAngle()
{
    joints &= ~(0b10000U);
    qDebug() << "clear flippers, joints = " << QString::number(joints,2);
}

double RobotPositionController::getFlippersAngle()
{
    return this->flippersAngle;
}

bool RobotPositionController::setWaistAngle(double angle)
{
    if (!checkAngleRange(-214.2, 137.3, angle)) return false;
    this->waistAngle = angle;
    joints |= 0b1000U;
    qDebug() << "set waist, joints = " << QString::number(joints,2);
    return true;
}

void RobotPositionController::clearWaistAngle()
{
    joints &= ~(0b1000U);
    qDebug() << "clear waist, joints = " << QString::number(joints,2);
}

double RobotPositionController::getWaistAngle()
{
    return this->waistAngle;
}

bool RobotPositionController::setElbowAngle(double angle)
{
    if (!checkAngleRange(-216.9, 0.0, angle)) return false;
    this->elbowAngle = angle;
    joints |= 1U;
    qDebug() << "set elbow, joints = " << QString::number(joints,2);
    return true;
}

void RobotPositionController::clearElbowAngle()
{
    joints &= ~(1U);
    qDebug() << "clear elbow, joints = " << QString::number(joints,2);
}

double RobotPositionController::getElbowAngle()
{
    return this->elbowAngle;
}

bool RobotPositionController::setShoulderAngle(double angle)
{
    if (!checkAngleRange(-16.4, 114.2, angle)) return false;
    this->shoulderAngle = angle;
    joints |= 0b100U;
    qDebug() << "set shoulder, joints = " << QString::number(joints,2);
    return true;
}

void RobotPositionController::clearShoulderAngle()
{
    joints &= ~(0b100U);
    qDebug() << "clear shoulder, joints = " << QString::number(joints,2);
}

double RobotPositionController::getShoulderAngle()
{
    return this->shoulderAngle;
}

bool RobotPositionController::setNeckAngle(double angle)
{
    if (!checkAngleRange(0.0, 188.8, angle)) return false;
    this->neckAngle = angle;
    joints |= 0b10U;
    qDebug() << "set neck, joints = " << QString::number(joints,2);
    return true;
}

void RobotPositionController::clearNeckAngle()
{
    joints &= ~(0b10U);
    qDebug() << "clear neck, joints = " << QString::number(joints,2);
}

double RobotPositionController::getNeckAngle()
{
    return this->neckAngle;
}

int RobotPositionController::getMaxSpeed(int id)
{
    switch (id) { //wtf with this shit???
    case 4:
        return 9000;
//        return this->robot->configuration->elbowSpeed;
    case 5:
        return 9000;
//        return this->robot->configuration->neckSpeed;
    case 6:
        return 9000;
//        return this->robot->configuration->shouldersSpeed;
    case 7:
        return 8000;
//        return this->robot->configuration->waistSpeed;
    case 10:
    default:
        qDebug() << "wrong id in getMaxSpeed";
        return 0;
    }
}

double RobotPositionController::getAngleRange(int id)
{
    switch (id) {
    case 4:
        return 216.9;
    case 5:
        return 188.8;
    case 6:
        return 114.2 + 16.4;
    case 7:
        return 214.2 + 137.3;
    case 10:
    default:
        qDebug() << "wrong id in getAngleRange";
        return 0;
    }
}

void RobotPositionController::stopTask()
{
    joints = 0U;
}

void RobotPositionController::handleTelemetry(char *data){
    delete positionInfo;
    positionInfo = (TelemetryPacket*) data;
    for (size_t i = 0; i < positionInfo->NUMBER_OF_MOTORS; ++i) {
        if (!hasPositionData(positionInfo->M_DATA[i].DEVICE_ID)) continue;
        int speed = 10000;
        switch (positionInfo->M_DATA[i].DEVICE_ID) {
        case 4: //elbow 1
            j->elbow = getAngleById(4,positionInfo->M_DATA[i].POSITION);
            if ((joints & 1U) == 0) {
                setAngleByMotorId(4, positionInfo->M_DATA[i].POSITION);
            } else {
                speed = calcSpeed(positionInfo->M_DATA[i].POSITION,
                                  getMotorPositionById(positionInfo->M_DATA[i].DEVICE_ID),
                                  positionInfo->M_DATA[i].DEVICE_ID,
                                  this);
                if (speed == 0) {
                    this->stopElbowNeck();
                    joints &= ~(0b1U);
                    qDebug() << "elbow finished, joints = " << QString::number(joints,2)
                             << "angle " << positionInfo->M_DATA[i].POSITION;
                }
                else this->elbowNeck(speed);
            }
            break;
        case 5: //neck 10
            j->neck = getAngleById(5,positionInfo->M_DATA[i].POSITION);
            if ((joints >> 1U & 1U) == 0) {
                setAngleByMotorId(5, positionInfo->M_DATA[i].POSITION);
            } else {
                if ((joints & 1U) != 0) {
                    setAngleByMotorId(5, positionInfo->M_DATA[i].POSITION);
                    qDebug() << "skipped neck coz has elbow tack";
                    break;
                }
                speed = calcSpeed(positionInfo->M_DATA[i].POSITION,
                                  getMotorPositionById(positionInfo->M_DATA[i].DEVICE_ID),
                                  positionInfo->M_DATA[i].DEVICE_ID,
                                  this);
                this->stopElbowNeck();
                if (speed == 0) {
                    this->stopNeck();
                    joints &= ~(0b10U);
                    qDebug() << "neck finished, joints = " << QString::number(joints,2);
                }
                else {

                    this->neck(-speed);
                }
            }
            break;
        case 6: //shoulder 100
            j->shoulder = getAngleById(6,positionInfo->M_DATA[i].POSITION);
            if ((joints >> 2U & 1U) == 0) {
                setAngleByMotorId(6, positionInfo->M_DATA[i].POSITION);
            } else {
                speed = calcSpeed(positionInfo->M_DATA[i].POSITION,
                                  getMotorPositionById(positionInfo->M_DATA[i].DEVICE_ID),
                                  positionInfo->M_DATA[i].DEVICE_ID,
                                  this);
                if (speed == 0) {
                    this->stopWaistUpDown();
                    joints &= ~(0b100U);
                    qDebug() << "shoulder finished, joints = " << QString::number(joints,2);
                }
                else {
                    this->waistUpDown(speed);
                    qDebug() << "shoulder speed "  << speed;
                }
            }
            break;
        case 7: //waist 1000
            j->waist = getAngleById(7,positionInfo->M_DATA[i].POSITION);
            if ((joints >> 3U & 1U) == 0) {
                setAngleByMotorId(7, positionInfo->M_DATA[i].POSITION);
            } else {
                speed = calcSpeed(positionInfo->M_DATA[i].POSITION,
                                  getMotorPositionById(positionInfo->M_DATA[i].DEVICE_ID),
                                  positionInfo->M_DATA[i].DEVICE_ID,
                                  this);
                if (speed == 0) {
                    this->stopWaist();
                    joints &= ~(0b1000U);
                    qDebug() << "waist finished, joints = " << QString::number(joints,2);
                }
                else this->waist(speed);
            }
            break;
        case 10: //flippers 10000
            this->j->flippers = getAngleById(10,positionInfo->M_DATA[i].POSITION);
            if ((joints >> 4U & 1U) == 0) {
                setAngleByMotorId(10, positionInfo->M_DATA[i].POSITION);
            } else {
                int cmp = comparePosition(positionInfo->M_DATA[i].POSITION,
                                          getMotorPositionById(positionInfo->M_DATA[i].DEVICE_ID), 10);
                if (cmp == -1) this->setFlippersUp();
                if (cmp == 0) {
                    this->stopFlippers();
                    joints &= ~(0b10000U);
                    qDebug() << "flippers finished, joints = " << QString::number(joints,2);
                }
                if (cmp == 1) this->setFlippersDown();
            }
            break;
        default:
            qDebug() << "wrong id while parsing positions";
            break;
        }
    }
    emit jointsUpdated(j);
}

void RobotPositionController::testModel()
{
    j->waist += 1.0;
    emit jointsUpdated(j);
}

int getMinSpeed(int id) {
    switch (id) {
    case 4:
        return 8000;
    case 5:
        return 6500;
    case 6:
        return 7000;
    case 7:
        return 7000;
    case 10:
    default:
        return 0;
    }
}

int calcSpeed(int current, int desired, int id, RobotPositionController* r) {
    int range = r->getAngleRange(id) / 4; //param pam pam :)
    int speed = 0;
    double c = getMinSpeed(id) - 1500;
    double a = -(r->getMaxSpeed(id) - c) / (range*range - 2.0*range);
    double b = -2.0 * a * range;
    int d = abs(r->getAngleById(id, current) - r->getAngleById(id, desired));//calc in grad
    if (d >= range) speed = r->getMaxSpeed(id);
    else speed = a*d*d + b*d + c;
    switch (comparePosition(current, desired, id)) {
    case -1:
        return speed;
    case 0:
        return 0;
    case 1:
        return -speed;
    default:
        return 0;
    }
}

/*void RobotPositionController::updateAngles()
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
        if (cmp == 0) speed = 0;
        if (cmp == -1) speed = 5000;
        switch (positionInfo->M_DATA[i].DEVICE_ID) {
        case 4: //elbow
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
            break;
        case 10: //flippers
            if (speed < 0) this->setFlippersDown();
            if (speed == 0) {
                this->stopFlippers();

            }
            if (speed > 0) this->setFlippersUp();
            break;
        default:
            break;
        }
    }
}*/

inline int comparePosition(int current, int desired, int id) {
    int treshold = 800;
    bool small = false;
    if (abs(current - desired) < 7) small = true;
    switch (id) { //this shit needs testing
    case 4: //elbow
        treshold = 300;
        if (small) treshold = 100;
        break;
    case 5: //neck
        treshold = 300;
        if (small) treshold = 100;
        break;
    case 6: //shoulder
        treshold = 300;
        if (small) treshold = 100;
        break;
    case 7: //waist
        treshold = 200;
        if (small) treshold = 50;
        break;
    case 10:
    default:
        treshold = 600;
        break;
    }
    if (abs(current - desired) < treshold) return 0;
    if (current > desired) return 1;
    return -1;
}

inline int abs(int n) noexcept{
    if (n < 0) return -n;
    return n;
}

int RobotPositionController::getMotorPositionById(int id)
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
        qDebug() << "wrong id in getMotorPositionById";
        return 0;
    }
}

void RobotPositionController::setAngleByMotorId(int id, int position)
{
    double angle = getAngleById(id, position);
    switch (id) {
    case 4: //elbow
        j->elbow = angle;
        elbowAngle = angle;
        break;
    case 5: //neck
        j->neck = angle;
        neckAngle = angle;
        break;
    case 6: //shoulder
        j->shoulder = angle;
        shoulderAngle = angle;
        break;
    case 7: //waist
        j->waist = angle;
        waistAngle = angle;
        break;
    case 10: //flippers
        j->flippers = angle;
        flippersAngle = angle;
        break;
    default:
        qDebug() << "wrong id in setAngleByMotorId";
    }
}

bool RobotPositionController::hasPositionData(int id)
{
    return id == 4 || id == 5 || id == 6 || id == 7 || id == 10;
}
