#include "mainwindow.h"
#include <QApplication>
#include <mainwindow.h>
#include <QDebug>
#include <QSurfaceFormat>
#ifdef QT_DEBUG
#define BUILD "Debug"
#else
#define BUILD "Release"
#endif
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(format);

    MainWindow window;
    window.show();
    qDebug() << "compiled with g++ ver." << __VERSION__
             << "c++ std." << __cplusplus << "Qt ver." << QT_VERSION_STR << "on" << __TIMESTAMP__;
    qDebug() << BUILD << "version";
    return app.exec();
}
