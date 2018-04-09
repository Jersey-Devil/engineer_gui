#ifndef ROBOTPOSITIONCONTROLLER_H
#define ROBOTPOSITIONCONTROLLER_H
#include <QObject>
#include "robotpackets.h"
#include "robotcontroller.h"
#include "robot.h"

//1 degree of angle in Servosila's joints
#define ANGLE_DELTA 182

/**
 * @brief The RobotPositionController class
 * This class is used for robot manipulating using position values, like degrees of joints
 */
class RobotPositionController : public RobotController
{
    Q_OBJECT
public:

    //info about current joints position
    TelemetryPacket *positionInfo;

    RobotPositionController(Robot *robot);
    ~RobotPositionController();
    double getAngleById(int id, int position);
    bool setFlippersAngle(double angle);
    void clearFlippersAngle();
    double getFlippersAngle();
    bool setWaistAngle(double angle);
    void clearWaistAngle();
    double getWaistAngle();
    bool setElbowAngle(double angle);
    void clearElbowAngle();
    double getElbowAngle();
    bool setShoulderAngle(double angle);
    void clearShoulderAngle();
    double getShoulderAngle();
    bool setNeckAngle(double angle);
    void clearNeckAngle();
    double getNeckAngle();
    int getMaxSpeed(int id);
    double getAngleRange(int id);
    virtual void stopTask() override;

signals:
    void connectionDrop();
public slots:
    void handleTelemetry(char *data);

private:
    int getMotorPositionById(int id);
    void setAngleByMotorId(int id, int position);
    bool hasPositionData(int id);
    double flippersAngle; // 230.0 -> -93.0
    double waistAngle; // -214.2 -> 137.3
    double elbowAngle; // -216.9 -> 0.0
    double shoulderAngle; // -16.4 -> 114.2
    double neckAngle; // 188.8 -> 0.0

    uint_fast8_t joints = 0;

};

#endif // ROBOTPOSITIONCONTROLLER_H
