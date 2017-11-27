TEMPLATE = subdirs
SUBDIRS = robot_control \
    robot_gui
robot_gui.depends = robot_control
CONFIG += ordered static
