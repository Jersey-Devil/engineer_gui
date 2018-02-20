#include "robotconfiguration.h"


RobotConfiguration::RobotConfiguration(int platformForwardSpeed, int platformRotateSpeed, int shouldersSpeed, int neckSpeed, int elbowSpeed, int waistSpeed, bool light){
    this->platformForwardSpeed = platformForwardSpeed;
    this->platformRotateSpeed = platformRotateSpeed;
    this->shouldersSpeed = shouldersSpeed;
    this->neckSpeed = neckSpeed;
    this->elbowSpeed = elbowSpeed;
    this->waistSpeed = waistSpeed;
    this->lightValue = light;
}


RobotConfiguration::RobotConfiguration(const char *) : RobotConfiguration()
{

}

RobotConfiguration::~RobotConfiguration(){}
