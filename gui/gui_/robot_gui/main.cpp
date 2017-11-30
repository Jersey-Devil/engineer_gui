#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include <mainwindow.h>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window(new QWidget);
    window.show();
    qDebug() << "c++ standart: " << __cplusplus;
    qDebug() << "compiler ver: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
    return app.exec();
}
