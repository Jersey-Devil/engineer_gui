#include <QtNetwork/QUdpSocket>
#include "myudpclient.h"
#include "QTimer"
#include "QDateTime"
#include "QtNetwork/QUdpSocket"
#include "QTextEdit"
#include "QHostAddress"
#include "QDataStream"
#include "QByteArray"
#include "QDebug"
#include "QThread"
#include "QFile"
#include "robotcontroller.h"
#include "QUdpSocket"

/**
 * @brief UDPClient::UDPClient
 * class to communicate with robot
 * uses Qt UDP socket
 *
 * @param controller - robot's controller class, needed to access to packet
 */
UDPClient::UDPClient(RobotController *controller):QObject()
{
    //configure conection values
    m_pudp = new QUdpSocket(this);
    robotAddress = new QHostAddress("10.42.0.1");
    this->controller = controller;
    //timer for package sending
    timer = new QTimer();
}




/**
 * @brief UDPClient::listenRobot
 * handles robot's packages
 * See robotPackets.h and Servosila documentation
 *
 * NOTE: it ignores packets with image from cameras and get only telemetry
 * after getting and emits robot's signal, so telemetry values pass to MainWindow and
 * RobotPositionController classes
 */
void UDPClient::listenRobot(){
    //if first got, then emit connected
    if(!isConntected){
        isConntected = true;
        emit controller->connectedToRobot();

    }

    QByteArray baDatagram;
    do {

        baDatagram.resize(m_pudp->pendingDatagramSize());
        m_pudp->readDatagram(baDatagram.data(),baDatagram.size());

    }while(m_pudp->hasPendingDatagrams());
    QDataStream in(&baDatagram, QIODevice::ReadOnly);

    do {
        //275 is telemetry packet weight in bytes
        //packets from cameras weight is bigger
        char* buffer = new char[275];
        in.readRawData(buffer,275);

        //first byte is FRAME_TYPE_ID
        //if it is 2, then it is TelemetryPacket
        if(buffer[0]==2){
            emit controller->robot->telemetryChanged(buffer);
            return;
        }else{
            //remove unnecessary
            delete[] buffer;
            return;
        }
    }while (!in.atEnd());

}



UDPClient::~UDPClient(){
    m_pudp->close();
    delete m_pudp;
    delete robotAddress;
    delete timer;
}


/**
 * @brief UDPClient::sendPacket
 * method to send packets
 * @param packet to send
 */
void UDPClient::sendPacket(RemoteControlPacket packet){
    QByteArray baDatagram;
    QDataStream out(&baDatagram,QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_2);
    out.writeRawData((char*)&packet,57);
    m_pudp->writeDatagram(baDatagram,*robotAddress,ROBOT_PORT);

}

QTimer *UDPClient::getTimer()
{
    return timer;
}

void UDPClient::moveFieldsToThread(QThread *t)
{
    timer->moveToThread(t);
//    m_pudp->moveToThread(t);
}

//can be used for logs
void UDPClient::writeInputToFile(char *data){
    QString filename="/home/vladm/Desktop/robot_engineer/qt_helloworld/robot_logs/logs.txt";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        stream << atoi(data)<< endl;
        stream.flush();

    }
}

//connect to robot method
void UDPClient::connectToRobot(){
    //bind to port and address
    m_pudp->bind(*robotAddress,ROBOT_PORT);

    //connect stuff
    connect(m_pudp,SIGNAL(readyRead()),this,SLOT(listenRobot()));
    connect(m_pudp,SIGNAL(connected()),this,SLOT(startTimerTask()));

    //start sending packets
    startTimerTask();
}

/**
 * @brief UDPClient::startTimerTask
 * starts to send packets from RobotController object
 * it sends it value very often due to UDP-kind socket
 *
 * if robot does not get packet during 1 second, it drops connection
 */
void UDPClient::startTimerTask(){
    connect(timer, SIGNAL(timeout()), this, SLOT(sendLivePackets()));
    timer->start(300);
}

//method which invoked by timer
void UDPClient::sendLivePackets(){

    RemoteControlPacket *packet = controller->packet;
    sendPacket(*packet);
}

//called when user presses "Disconnect" button
void UDPClient::disconnectFromRobot(){
    m_pudp->disconnectFromHost();
    //stop timer
    timer->stop();
    //remove signals
    disconnect(m_pudp,SIGNAL(readyRead()),this,SLOT(listenRobot()));
    disconnect(m_pudp,SIGNAL(connected()),this,SLOT(startTimerTask()));
    disconnect(timer, SIGNAL(timeout()), this, SLOT(sendLivePackets()));

}
