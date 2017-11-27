#include "udpsocket.h"
#include <cstddef>
#include <functional>
#include <exception>
#include <QUdpSocket>
#include <QHostAddress>

UdpSocket::UdpSocket(QObject *parent, const char* const address)
    : QUdpSocket(parent)
{
    this->robotAddress = new QHostAddress(address);
}

UdpSocket::~UdpSocket()
{
    QUdpSocket::close();
    delete robotAddress;
}

bool UdpSocket::hasPackets() const
{
    return QUdpSocket::hasPendingDatagrams();
}

void UdpSocket::connectToRobot()
{
    QUdpSocket::bind(*robotAddress, PORT);
    connect((QUdpSocket*)this, SIGNAL(readyRead()), (UdpSocket*)this, SLOT(notificateClient()));
    connect((QUdpSocket*)this, SIGNAL(connected()), (UdpSocket*)this, SLOT(notificateTimer()));
}

void UdpSocket::disconnectFromRobot()
{
    QUdpSocket::disconnectFromHost();
}

void UdpSocket::removeCallbacks() const
{
    //....
}

void UdpSocket::notificateClient()
{
    packetNotif();
}

void UdpSocket::notificateTimer()
{
    startTime();
}

int64_t UdpSocket::receivePacket(char *data)
{
    if (!QUdpSocket::hasPendingDatagrams()) return -1;
    int64_t expLen = QUdpSocket::pendingDatagramSize();
    if (data != nullptr) delete[] data;
    data = new char[expLen];
    int64_t readLen = QUdpSocket::readDatagram(data, MAX_LEN, robotAddress, &PORT);
    if (expLen != readLen)
        throw std::runtime_error("Expected len != read len while reading datagram");
    return readLen;
}

int64_t UdpSocket::transmitPacket(const char *data, const int64_t len)
{
    return QUdpSocket::writeDatagram(data, len, *robotAddress, PORT);
}

void UdpSocket::setCallbacks(std::function<void(void)> packetNotif,
                             std::function<void(void)> startTime)
{
    this->packetNotif = packetNotif;
    this->startTime = startTime;
}
