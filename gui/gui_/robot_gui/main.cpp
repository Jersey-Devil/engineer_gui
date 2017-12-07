#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include <mainwindow.h>
#ifdef QT_DEBUG
#define BUILD "Debug"
#else
#define BUILD "Release"
#endif
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window(new QWidget);
    window.show();
    qDebug() << "compiled with g++ ver." << __VERSION__
             << "c++ std." << __cplusplus << "Qt ver." << QT_VERSION_STR << "on" << __TIMESTAMP__;
    qDebug() << BUILD << "version";
    return app.exec();
}
