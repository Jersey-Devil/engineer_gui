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
    Robot *robot;

    //info about current joints position
    TelemetryPacket *positionInfo;

    RobotPositionController(Robot *robot);
    ~RobotPositionController();
    int getAngleById(int id, int position);
    void setFlippersAngle(int angle);
    int getFlippersAngle();
    void setWaistAngle(int angle);
    int getWaistAngle();
    void setElbowAngle(int angle);
    int getElbowAngle();
    void setShoulderAngle(int angle);
    int getShoulderAngle();
    void setNeckAngle(int angle);
    int getNeckAngle();
    void startTask();
    virtual void stopTask() override;

public slots:
    void handleTelemetry(char *data);

private:
    void updateAngles();
    void evaluateTask();
    int getMotorPositionByAngle(int id);
    void setAngleByMotorId(int id, int position);
    bool hasPositionData(int id);
    int flippersAngle;
    int waistAngle;
    int elbowAngle;
    int shoulderAngle;
    int neckAngle;

    bool hasTask;
};

#endif // ROBOTPOSITIONCONTROLLER_H
