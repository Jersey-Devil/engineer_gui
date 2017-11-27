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

    return app.exec();
}
