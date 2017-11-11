#include "myudpserver.h"
#include "QTimer"
#include "QDateTime"
#include "QtNetwork/QUdpSocket"
#include "QTextEdit"
#include "QHostAddress"
#include "QDataStream"
#include "QByteArray"
#include "QDataStream"
#include "myudpclient.h"
#include "QFile"

MyUdpServer::MyUdpServer(QWidget *pwgt): QTextEdit(pwgt)
{
    setWindowTitle("UDP_Server");
    m_pudp = new QUdpSocket(this);
    m_pudp->bind(2425);
    show();
    connect(m_pudp,SIGNAL(readyRead()),SLOT(slotSendDatagram()));
}
void MyUdpServer::slotSendDatagram(){
    QByteArray baDatagram;
    RobotController controller;
    do {
        baDatagram.resize(m_pudp->pendingDatagramSize());
        m_pudp->readDatagram(baDatagram.data(),baDatagram.size());

    }while(m_pudp->hasPendingDatagrams());
    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    RemoteControlPacket *packet = controller.getBasicPacket();
    uint len = 57;
    in.readRawData(((char*)(packet)), len);
    append(((char*) packet));
    QString filename="/home/vlad/Desktop/Robotics/qt_helloworld/robot_logs/logs.txt";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );
        stream << *((char*)packet)<< endl;
    }
}
MyUdpServer::~MyUdpServer(){
    m_pudp->close();
    delete m_pudp;
}
