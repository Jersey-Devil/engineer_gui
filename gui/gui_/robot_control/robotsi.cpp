#include "robotsi.h"
#include "robot.h"

RobotSI::RobotSI()
{

}

void RobotSI::moveD(double speed)
{
    Robot::moveD(msToMotorSpeedForward(speed));
}

void RobotSI::moveR(double speed)
{
    Robot::moveR(msToMotorSpeedRotate(speed));
}

int RobotSI::msToMotorSpeedForward(double speed)
{
    return (int) (speed * forwardSpeedKoeff);
}

int RobotSI::msToMotorSpeedRotate(double speed)
{
    return (int) (speed * rotateSpeedKoeff);
}

double RobotSI::motorSpeedToMsForward(int speed)
{
    return (double) speed / forwardSpeedKoeff;
}

double RobotSI::motorSpeedToMsRotate(int speed)
{
    return (double) speed / rotateSpeedKoeff;
}
