#include "messenger.h"

#include <QUdpSocket>
#include <QNetworkInterface>

#include "platform.h"
#include "buddymessage.h"

#ifdef Q_OS_ANDROID
#include "androidutils.h"
#endif

Messenger::Messenger(quint16 defaultPort, QObject *parent) : QObject(parent), protocolDefaultPort(defaultPort) {
    socket = new QUdpSocket(this);
}

Messenger::~Messenger() {
#ifdef Q_OS_ANDROID
    delete lock;
#endif
    delete socket;
}


bool Messenger::start(quint16 listenPort) {
    if (socket->state() == QUdpSocket::BoundState) {
        if (listenPort == localPort) {
            return true;
        }
        stop();
    }

    localPort = listenPort;
    // Do NOT use Qt::QueuedConnection, otherwise the readyRead signal will go wrong
    connect(socket, &QUdpSocket::readyRead, this, &Messenger::processDatagram, Qt::UniqueConnection);

#ifdef Q_OS_ANDROID
    // acquire MulticastLock for receiving multicast messages
    // https://developer.android.com/reference/android/net/wifi/WifiManager.MulticastLock
    if (lock == nullptr) {
        lock = new AndroidMulticastLock();
        lock->acquire();
    }
#endif

    return socket->bind(QHostAddress::AnyIPv4, listenPort);
}

void Messenger::stop() {
    sayGoodbye();
    socket->disconnect(this);
    socket->close();
#ifdef Q_OS_ANDROID
    if (lock != nullptr) {
        lock->release();
    }
#endif
}

void Messenger::processDatagram() {
    QByteArray datagram;
    qint64 size;
    while ((size = socket->pendingDatagramSize()) != -1) {
        datagram.resize(size);

        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(datagram.data(), size, &sender, &senderPort);

        if (badAddrs.contains(sender)) {
            continue;
        }
        if (localAddrs.contains(sender)) {
            // sent by self, ignore
            int count = localAddrs.value(sender) + 1;
            if (count > 5) {
                qDebug() << "detected broadcast storm from" << sender.toString();
                badAddrs.append(sender);
            }
            localAddrs.insert(sender, count);
            continue;
        }
        BuddyMessage message = BuddyMessage::parse(datagram);
        if (message.isValid()) {
            processMessage(message, sender);
        }
    }
}

void Messenger::processMessage(const BuddyMessage &message, const QHostAddress &sender) {
    switch (message.getType()) {
        case BuddyMessage::MSG_HELLO_BROADCAST:
        case BuddyMessage::MSG_HELLO_UNICAST: {
            Peer peer(sender, message.getSignature(), protocolDefaultPort);
            peers[sender] = peer;
            if (message.getType() == BuddyMessage::MSG_HELLO_BROADCAST) {
                sayHello(sender, protocolDefaultPort);
            }
            emit buddyFound(peer);
            break;
        }

        case BuddyMessage::MSG_GOODBYE:
            if (peers.contains(sender)) {
                Peer peer = peers[sender];
                emit buddyGone(peer);
                peers.remove(sender);
            }
            break;

        case BuddyMessage::MSG_HELLO_PORT_BROADCAST:
        case BuddyMessage::MSG_HELLO_PORT_UNICAST: {
            Peer peer = Peer(sender, message.getSignature(), message.getPort());
            if (message.getType() == BuddyMessage::MSG_HELLO_PORT_BROADCAST) {
                sayHello(sender, message.getPort());
            }
            emit buddyFound(peer);
            break;
        }
        case BuddyMessage::MSG_INVALID:
            break;
    }
}


void Messenger::sayHello() {
    if (socket->state() != QUdpSocket::BoundState) {
        return;
    }
    broadcastMessage(BuddyMessage(BuddyMessage::broadcastType(socket->localPort() == protocolDefaultPort), socket->localPort(), getSystemSignature()));
}

void Messenger::sayHello(const QHostAddress &target, quint16 port) {
    if (socket->state() != QUdpSocket::BoundState) {
        return;
    }
    BuddyMessage message(BuddyMessage::unicastType(socket->localPort() == protocolDefaultPort), socket->localPort(), getSystemSignature());
    sendPacket(message.serialize(), target, port);
}

void Messenger::sayGoodbye() {
    if (socket->state() != QUdpSocket::BoundState) {
        return;
    }
    broadcastMessage(BuddyMessage::goodbye());
}

// Send message from all interfaces
void Messenger::broadcastMessage(const BuddyMessage &message) {
    QByteArray packet = message.serialize();

    // Look for all the discovered ports
    QList<quint16> ports;
    ports.append(protocolDefaultPort);
    foreach(const Peer &peer, peers) {
        if (ports.contains(peer.port) == false) {
            ports.append(peer.port);
        }
    }

    // recreate the local ip addresses list
    localAddrs.clear();

    // broadcast to all interfaces
    const QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface: ifaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) == false) {
            continue;
        }
        const QList<QNetworkAddressEntry> addrs = iface.addressEntries();
        for (const QNetworkAddressEntry &addr: addrs) {
            QHostAddress ipAddr = addr.ip();
            if (ipAddr.protocol() == QAbstractSocket::IPv4Protocol && !ipAddr.isLoopback()) {
                if (badAddrs.contains(ipAddr)) {
                    qDebug() << "skip bad addr" << ipAddr.toString() << " of " << iface.name();
                    continue;
                }
                localAddrs.insert(ipAddr, 0);
                for (quint16 port: ports) {
                    sendPacket(packet, addr.broadcast(), port);
                }
            }
        }
    }
}


void Messenger::sendPacket(const QByteArray &data, const QHostAddress &target, quint16 port) {
    socket->writeDatagram(data.data(), data.length(), target, port);
    socket->flush();
}


QString Messenger::getSystemSignature() {
    static QString signature;
    if (signature.isEmpty()) {
        signature = QStringLiteral("%1 at %2 (%3)").arg(Platform::getSystemUsername(), Platform::getHostname(), Platform::getPlatformName());
    }
    return signature;
}
