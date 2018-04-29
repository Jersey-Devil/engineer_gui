#ifndef JOINTS_H
#define JOINTS_H
struct Joints
{
    double waist;
    double shoulder;
    double elbow;
    double neck;
    double flippers;

    Joints() {
        waist = shoulder = elbow = neck = flippers = 0.0;
    }
};
#endif // JOINTS_H
