/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "duktoprotocol.h"

#if defined(Q_OS_WIN)
    #include <windows.h>
#endif

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QNetworkInterface>
#include <QTimer>

#include "platform.h"

#define DEFAULT_UDP_PORT 4644
#define DEFAULT_TCP_PORT 4644

enum MSG_TYPE {
    MSG_HELLO_BROADCAST = 0x01,
    MSG_HELLO_UNICAST = 0x02,
    MSG_GOODBYE = 0x03,
    MSG_HELLO_PORT_BROADCAST = 0x04,
    MSG_HELLO_PORT_UNICAST = 0x05
};

DuktoProtocol::DuktoProtocol(QObject *parent)
    : QObject(parent), mLocalUdpPort(DEFAULT_UDP_PORT), mLocalTcpPort(DEFAULT_TCP_PORT)
{
}

DuktoProtocol::~DuktoProtocol()
{
    delete mCurrentSocket;
    delete mSocket;
    delete mTcpServer;
    delete mCurrentFile;
}

void DuktoProtocol::initialize()
{
    mSocket = new QUdpSocket(this);
    mSocket->bind(QHostAddress::Any, mLocalUdpPort);
    connect(mSocket, &QUdpSocket::readyRead, this, &DuktoProtocol::newUdpData);

    mTcpServer = new QTcpServer(this);
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer, &QTcpServer::newConnection, this, &DuktoProtocol::newIncomingConnection);
}

void DuktoProtocol::setPorts(qint16 udp, qint16 tcp)
{
    mLocalUdpPort = udp;
    mLocalTcpPort = tcp;
}

QByteArray DuktoProtocol::getSystemSignature()
{
    static QByteArray signature;
    if (!signature.isEmpty()) return signature;

    signature = (Platform::getSystemUsername()
              + " at " + Platform::getHostname()
              + " (" + Platform::getPlatformName() + ")").toUtf8();
    return signature;
}

void DuktoProtocol::sayHello(const QHostAddress &dest)
{
    sayHello(dest, mLocalUdpPort);
}

void DuktoProtocol::sayHello(const QHostAddress &dest, qint16 port)
{
    // Preparazione pacchetto
    QByteArray packet;
    if ((port == DEFAULT_UDP_PORT) && (mLocalUdpPort == DEFAULT_UDP_PORT))
    {
        if (dest == QHostAddress::Broadcast)
            packet.append(MSG_HELLO_BROADCAST);
        else
            packet.append(MSG_HELLO_UNICAST);
    }
    else
    {
        if (dest == QHostAddress::Broadcast)
            packet.append(MSG_HELLO_PORT_BROADCAST);
        else
            packet.append(MSG_HELLO_PORT_BROADCAST);
        packet.append((char*)&mLocalUdpPort, sizeof(qint16));
    }
    packet.append(getSystemSignature());

    // Invio pacchetto
    if (dest == QHostAddress::Broadcast) {
        QList<qint16> ports;
        ports.append(DEFAULT_UDP_PORT);
        if (port != DEFAULT_UDP_PORT)
            ports.append(port);
        sendToAllBroadcast(packet, ports);
    }
    else
        mSocket->writeDatagram(packet.data(), packet.length(), dest, port);
}

void DuktoProtocol::sayGoodbye()
{
    // Create packet
    QByteArray packet;
    packet.append(MSG_GOODBYE);

    // Look for all the discovered ports
    QList<qint16> ports;
    ports.append(mLocalUdpPort);
    if (mLocalUdpPort != DEFAULT_UDP_PORT) ports.append(DEFAULT_UDP_PORT);
    QList<Peer> values = mPeers.values();
    for(QList<Peer>::const_iterator p = values.constBegin(); p != values.constEnd(); ++p)
        if(!ports.contains(p->port))
            ports.append(p->port);

    // Send broadcast message to all discovered ports
    sendToAllBroadcast(packet, ports);
}

void DuktoProtocol::newUdpData()
{
    while (mSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(65536);  // Theoretical max length in IPv4
        QHostAddress sender;
        quint16 senderPort;
        int size = mSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        datagram.resize(size);
        handleMessage(datagram, sender);
     }
}

void DuktoProtocol::handleMessage(const QByteArray &data, const QHostAddress &sender)
{
    char msgtype = data.at(0);

    switch(msgtype)
    {
        case MSG_HELLO_BROADCAST:
        case MSG_HELLO_UNICAST: {
            QByteArray signature = data.mid(1);
            if (signature != getSystemSignature()) {
                mPeers[sender.toString()] = Peer(sender, QString::fromUtf8(signature), DEFAULT_UDP_PORT);
                if (msgtype == MSG_HELLO_BROADCAST) sayHello(sender, DEFAULT_UDP_PORT);
                emit peerListAdded(mPeers[sender.toString()]);
            }
            break;
        }
        case MSG_GOODBYE:
            emit peerListRemoved(mPeers[sender.toString()]);
            mPeers.remove(sender.toString());
            break;

        case MSG_HELLO_PORT_BROADCAST:
        case MSG_HELLO_PORT_UNICAST: {
            qint16 port = *(reinterpret_cast<const qint16*>(data.constData() + 1));
            QByteArray signature = data.mid(3);
            if (signature != getSystemSignature()) {
                mPeers[sender.toString()] = Peer(sender, QString::fromUtf8(signature), port);
                if (msgtype == MSG_HELLO_PORT_BROADCAST) sayHello(sender, port);
                emit peerListAdded(mPeers[sender.toString()]);
            }
            break;
        }
    }

}

// Richiesta connessione TCP in ingresso
void DuktoProtocol::newIncomingConnection()
{
    // Recieve connection
    QTcpSocket* s = mTcpServer->nextPendingConnection();
    if(s == nullptr) return;

    // If we are already recieving or sending
    // Pending header connection (timeout 10 sec)
    if ((mIsReceiving | mIsSending) || !s->waitForReadyRead(10000))
    {
        s->close();
        delete s;
        return;
    }

    // Update GUI
    emit receiveFileStart(s->peerAddress().toString());

    // set current TCP socket
    mCurrentSocket = s;

    // Register socket's event handlers
    connect(mCurrentSocket, &QTcpSocket::readyRead, this, &DuktoProtocol::readNewData, Qt::DirectConnection);
    connect(mCurrentSocket, &QTcpSocket::disconnected, this, &DuktoProtocol::closedConnectionTmp, Qt::QueuedConnection);

    // Initialize variables
    mIsReceiving = true;
    mTotalReceivedData = 0;
    mElementSize = -1;
    mReceivedFiles = new QStringList();
    mRootFolderName.clear();
    mRootFolderRenamed.clear();
    mReceivingText = false;
    mRecvStatus = FILENAME;

    // -- Reading General header --
    // Entities number to receive
    mCurrentSocket->read((char*) &mElementsToReceiveCount, sizeof(qint64));
    // total size
    mCurrentSocket->read((char*) &mTotalSize, sizeof(qint64));

    // Start reading data on file
    readNewData();
}

// Processo di lettura principale
void DuktoProtocol::readNewData()
{
    // Fino a che ci sono dati da leggere
    while (mCurrentSocket->bytesAvailable() > 0)
    {
        // In base allo stato in cui mi trovo leggo quello che mi aspetto
        switch (mRecvStatus)
        {
            case FILENAME:
            {
                char c;
                while (true) {
                    if (mCurrentSocket->read(&c, sizeof(c)) < 1) return;
                    if (c == '\0')
                    {
                        mRecvStatus = FILESIZE;
                        break;
                    }
                    mPartialName.append(c);
                }
                break;
            }
            case FILESIZE:
            {
                if(mCurrentSocket->bytesAvailable() < (int)sizeof(qint64)) return;
                mCurrentSocket->read((char*) &mElementSize, sizeof(qint64));
                mElementReceivedData = 0;
                QString name = QString::fromUtf8(mPartialName);
                mPartialName.clear();

                // Se l'elemento corrente è una cartella, la creo e passo all'elemento successivo
                if (mElementSize == -1)
                {
                    // Verifico il nome della cartella "root"
                    // Se non ho ancora trattato questa root, lo faccio ora
                    if (mRootFolderName != name.section("/", 0, 0)) {

                        // Verifico se ho già una cartella con questo nome
                        // nel caso trovo un nome alternativo
                        int i = 2;
                        QString originalName = name;
                        while (QFile::exists(name))
                            name = originalName + " (" + QString::number(i++) + ")";
                        mRootFolderName = originalName;
                        mRootFolderRenamed = name;
                        mReceivedFiles->append(name);

                    }

                    // Se invece l'ho già trattata, allora rinomino questo percorso
                    else if (mRootFolderName != mRootFolderRenamed)
                        name = name.replace(0, name.indexOf('/'), mRootFolderRenamed);

                    // Creo la cartella
                    if (!QDir(".").mkpath(name))
                    {
                        emit receiveFileCancelled();
                        // Chiusura socket
                        if (mCurrentSocket)
                        {
                            mCurrentSocket->disconnect();
                            mCurrentSocket->disconnectFromHost();
                            mCurrentSocket->close();
                            mCurrentSocket->deleteLater();
                            mCurrentSocket = nullptr;
                        }

                        // Rilascio memoria
                        delete mReceivedFiles;
                        mReceivedFiles = nullptr;

                        // Impostazione stato
                        mIsReceiving = false;
                        return;
                    }
                    mRecvStatus = FILENAME;
                    break;
                }

                // Potrebbe essere un invio di testo
                else if (name == "___DUKTO___TEXT___")
                {
                    mReceivedFiles->append(name);
                    mReceivingText = true;
                    mTextToReceive.clear();
                    mCurrentFile = nullptr;
                }

                // Altrimenti creo il nuovo file
                else
                {
                    // Se il file è in una cartella rinominata, devo provvedere di conseguenza
                    if ((name.indexOf('/') != -1) && (name.section("/", 0, 0) == mRootFolderName))
                        name = name.replace(0, name.indexOf('/'), mRootFolderRenamed);

                    // Se il file esiste già cambio il nome di quello nuovo
                    int i = 2;
                    QString originalName = name;
                    while (QFile::exists(name)) {
                        QFileInfo fi(originalName);
                        name = fi.baseName() + " (" + QString::number(i) + ")." + fi.completeSuffix();
                        i++;
                    }
                    mReceivedFiles->append(name);
                    mCurrentFile = new QFile(name);
                    bool ret = mCurrentFile->open(QIODevice::WriteOnly);
                    if (!ret)
                    {
                        emit receiveFileCancelled();
                        // Chiusura socket
                        if (mCurrentSocket)
                        {
                            mCurrentSocket->disconnect();
                            mCurrentSocket->disconnectFromHost();
                            mCurrentSocket->close();
                            mCurrentSocket->deleteLater();
                            mCurrentSocket = nullptr;
                        }

                        // Rilascio memoria
                        delete mReceivedFiles;
                        mReceivedFiles = nullptr;

                        // Impostazione stato
                        mIsReceiving = false;
                        return;
                    }
                    mReceivingText = false;
                }
                mRecvStatus = DATA;
                break;
            }
            case DATA:
            {
                // Provo a leggere quanto mi serve per finire il file corrente
                // (o per svuotare il buffer dei dati ricevuti)
                qint64 s = (mCurrentSocket->bytesAvailable() > (mElementSize - mElementReceivedData))
                           ? (mElementSize - mElementReceivedData)
                           : mCurrentSocket->bytesAvailable();
                QByteArray d = mCurrentSocket->read(s);
                mElementReceivedData += d.size();
                mTotalReceivedData += d.size();
                updateStatus();

                // Salvo i dati letti
                if (!mReceivingText)
                    mCurrentFile->write(d);
                else
                    mTextToReceive.append(d);

                // Verifico se ho completato l'elemento corrente
                if (mElementReceivedData == mElementSize)
                {
                    // Completato, chiudo il file e mi preparo per il prossimo elemento
                    mElementSize = -1;
                    if (!mReceivingText)
                    {
                        mCurrentFile->deleteLater();
                        mCurrentFile = nullptr;
                    }
                    mRecvStatus = FILENAME;
                }
                break;
            }

        }
    }
}

void DuktoProtocol::closedConnectionTmp()
{
    QTimer::singleShot(500, this, &DuktoProtocol::closedConnection);
}

// Chiusura della connessione TCP in ricezione
void DuktoProtocol::closedConnection()
{
    // empty the receive buffer
    readNewData();

    // Closing any current file
    if (mCurrentFile)
    {
        QString name;
        name = mCurrentFile->fileName();
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = nullptr;
        QFile::remove(name);
        emit receiveFileCancelled();
    }
    else if (!mReceivingText) // Receiving file ended
    {
        emit receiveFileComplete(mReceivedFiles, mTotalSize);
        // TODO: notify for recieving file
    }
    else // Receiving text ended
    {
        QString rec = QString::fromUtf8(mTextToReceive);
        emit receiveTextComplete(&rec, mTotalSize);
        // TODO: notify for recieving text
    }

    // closing socket
    if (mCurrentSocket)
    {
        mCurrentSocket->disconnect();
        mCurrentSocket->disconnectFromHost();
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
        mCurrentSocket = nullptr;
    }

    // release memory
    delete mReceivedFiles;
    mReceivedFiles = nullptr;

    // reset status
    mIsReceiving = false;
}

void DuktoProtocol::sendFile(const QString &ipDest, qint16 port, const QStringList &files)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Verifica altre attività in corso
    if (mIsReceiving || mIsSending) return;
    mIsSending = true;

    // File da inviare
    mFilesToSend = expandTree(files);
    mFileCounter = 0;

    // Connessione al destinatario
    mCurrentSocket = new QTcpSocket(this);

    // Gestione segnali
    connect(mCurrentSocket, &QTcpSocket::connected, this, &DuktoProtocol::sendMetaData, Qt::DirectConnection);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(mCurrentSocket, &QTcpSocket::errorOccurred, this, &DuktoProtocol::sendConnectError, Qt::DirectConnection);
#else
    connect(mCurrentSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &DuktoProtocol::sendConnectError, Qt::DirectConnection);
#endif
    connect(mCurrentSocket, &QTcpSocket::bytesWritten, this, &DuktoProtocol::sendData, Qt::DirectConnection);

    // Connessione
    mCurrentSocket->connectToHost(ipDest, port);
}

void DuktoProtocol::sendText(const QString &ipDest, qint16 port, const QString &text)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Verifica altre attività in corso
    if (mIsReceiving || mIsSending) return;
    mIsSending = true;

    // Testo da inviare
    mFilesToSend.clear();
    mFilesToSend.append("___DUKTO___TEXT___");
    mFileCounter = 0;
    mTextToSend = text;

    // Connessione al destinatario
    mCurrentSocket = new QTcpSocket(this);
    connect(mCurrentSocket, &QTcpSocket::connected, this, &DuktoProtocol::sendMetaData, Qt::DirectConnection);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(mCurrentSocket, &QTcpSocket::errorOccurred, this, &DuktoProtocol::sendConnectError, Qt::DirectConnection);
#else
    connect(mCurrentSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &DuktoProtocol::sendConnectError, Qt::DirectConnection);
#endif
    connect(mCurrentSocket, &QTcpSocket::bytesWritten, this, &DuktoProtocol::sendData, Qt::DirectConnection);
    mCurrentSocket->connectToHost(ipDest, port);
}

void DuktoProtocol::sendScreen(const QString &ipDest, qint16 port, const QString &path)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Verifica altre attività in corso
    if (mIsReceiving || mIsSending) return;
    mIsSending = true;

    // File da inviare
    QStringList files;
    files.append(path);
    mFilesToSend = expandTree(files);
    mFileCounter = 0;
    mSendingScreen = true;

    // Connessione al destinatario
    mCurrentSocket = new QTcpSocket(this);

    // Gestione segnali
    connect(mCurrentSocket, &QTcpSocket::connected, this, &DuktoProtocol::sendMetaData, Qt::DirectConnection);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(mCurrentSocket, &QTcpSocket::errorOccurred, this, &DuktoProtocol::sendConnectError, Qt::DirectConnection);
#else
    connect(mCurrentSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &DuktoProtocol::sendConnectError, Qt::DirectConnection);
#endif
    connect(mCurrentSocket, &QTcpSocket::bytesWritten, this, &DuktoProtocol::sendData, Qt::DirectConnection);

    // Connessione
    mCurrentSocket->connectToHost(ipDest, port);
}

void DuktoProtocol::sendMetaData()
{
    // Impostazione buffer di invio
#if defined(Q_OS_WIN)
    int v = 49152;
    ::setsockopt(mCurrentSocket->socketDescriptor(), SOL_SOCKET, SO_SNDBUF, (char*)&v, sizeof(v));
#endif

    // Header
    //  - N. entità (file, cartelle, ecc...)
    //  - Dimensione totale
    //  - Nome primo file
    //  - Dimensione primo (e unico) file (-1 per una cartella)

    QByteArray header;
    qint64 tmp;

    // N. entità
    tmp = mFilesToSend.count();
    header.append((char*) &tmp, sizeof(tmp));
    // Dimensione totale
    mTotalSize = computeTotalSize(mFilesToSend);
    header.append((char*) &mTotalSize, sizeof(mTotalSize));

    // Primo elemento
    header.append(nextElementHeader());

    // Invio header
    mCurrentSocket->write(header);

    // Inizializzazione variabili
    mTotalSize += header.size();
    mSentData = 0;
    mSentBuffer = 0;

    // Aggiornamento interfaccia utente
    updateStatus();
}

void DuktoProtocol::sendData(qint64 b)
{
    QByteArray d;

    // Aggiornamento statistiche
    mSentData += b;
    updateStatus();

    // Verifica se tutti i dati messi nel buffer
    // sono stati inviati
    mSentBuffer -= b;

    // Se ci sono altri dati da inviare, attendo
    // che vengano inviati
    if (mSentBuffer > 0) return;

    // Se si tratta di un invio testuale, butto dentro
    // tutto il testo
    if ((!mTextToSend.isEmpty()) && (mFilesToSend.at(mFileCounter - 1) == "___DUKTO___TEXT___"))
    {
        d.append(mTextToSend.toUtf8().data());
        mCurrentSocket->write(d);
        mSentBuffer = d.size();
        mTextToSend.clear();
        return;
    }

    // Se il file corrente non è ancora terminato
    // invio una nuova parte del file
    if (mCurrentFile)
        d = mCurrentFile->read(10000);
    if (d.size() > 0)
    {
        mCurrentSocket->write(d);
        mSentBuffer = d.size();
        return;
    }

    // Altrimenti chiudo il file e passo al prossimo
    d.append(nextElementHeader());

    // Non ci sono altri file da inviare?
    if (d.size() == 0)
    {
        closeCurrentTransfer();
        return;
    }

    // Invio l'header insime al primo chunk di file
    mTotalSize += d.size();
    if (mCurrentFile)
        d.append(mCurrentFile->read(10000));
    mCurrentSocket->write(d);
    mSentBuffer += d.size();

    return;
}

// Chiusura trasferimento dati
void DuktoProtocol::closeCurrentTransfer(bool aborted)
{
    mCurrentSocket->disconnect();
    mCurrentSocket->disconnectFromHost();
    if (mCurrentSocket->state() != QTcpSocket::UnconnectedState)
        mCurrentSocket->waitForDisconnected(1000);
    mCurrentSocket->close();
    mCurrentSocket->deleteLater();
    mCurrentSocket = nullptr;
    if (mCurrentFile)
    {
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = nullptr;
    }
    mIsSending = false;
    if (!aborted)
        emit sendFileComplete();
    mFilesToSend.clear();
}

// Aggiornamento delle statistiche di invio
void DuktoProtocol::updateStatus()
{
    if (mIsSending)
        emit transferStatusUpdate(mTotalSize, mSentData);
    else if (mIsReceiving)
        emit transferStatusUpdate(mTotalSize, mTotalReceivedData);
}

// In caso di errore di connessione
void DuktoProtocol::sendConnectError(QAbstractSocket::SocketError e)
{
    if (mCurrentSocket)
    {
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
        mCurrentSocket = nullptr;
    }
    if (mCurrentFile)
    {
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = nullptr;
    }
    mIsSending = false;
    emit sendFileError(e);
}

// Dato un elenco di file e cartelle, viene espanso in modo da
// contenere tutti i file e le cartelle contenuti
QStringList DuktoProtocol::expandTree(const QStringList& files)
{
    // Percorso base
    QString bp = files.first();
    if (bp.right(1) == "/") bp.chop(1);
    mBasePath = QFileInfo(bp).absolutePath();
    if (mBasePath.right(1) == "/") mBasePath.chop(1);

    // Iterazione sugli elementi
    QStringList expanded;
    for(QStringList::const_iterator iter = files.constBegin(); iter != files.constEnd(); ++iter) {
        QString path = QDir::cleanPath(*iter);
        if (path.right(1) == "/") path.chop(1);
        addRecursive(expanded, path);
    }

    return expanded;
}

// Aggiunge ricorsivamente tutte le cartelle e file contenuti in una cartella
void DuktoProtocol::addRecursive(QStringList& e, const QString &path)
{
    e.append(path);

    QString tempPath = path + "/";
    if (QFileInfo(path).isDir())
    {
        QStringList entries = QDir(path).entryList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
        for(QStringList::const_iterator iter = entries.constBegin(); iter != entries.constEnd(); ++iter)
            addRecursive(e, tempPath + *iter);
    }
}

// Restituisce l'header da inviare per il prossimo elemento
// da tramsettere
QByteArray DuktoProtocol::nextElementHeader()
{
    QByteArray header;

    // Ricava il nome del file (se non è l'ultimo)
    if (mFilesToSend.size() == mFileCounter) return header;
    QString fullname = mFilesToSend.at(mFileCounter++);

    // Chiusura file precedente, se non è già stato chiuso
    if (mCurrentFile) {
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = nullptr;
    }

    // Verifico se si tratta di un invio testo
    if (fullname == "___DUKTO___TEXT___") {
        header.append(fullname.toUtf8() + '\0');
        qint64 size = mTextToSend.toUtf8().length();
        header.append((char*) &size, sizeof(size));
        return header;
    }

    // Nome elemento
    QString name;

    // Verifico se si tratta di un invio screen
    if (mSendingScreen) {

        name = "Screenshot.jpg";
        mSendingScreen = false;
    }
    else
        name = fullname;

    // Aggiunta nome file all'header
    name.replace(mBasePath + "/", "");
    header.append(name.toUtf8() + '\0');

    // Dimensione elemento
    qint64 size = -1;
    QFileInfo fi2(fullname);
    if (fi2.isFile()) size = fi2.size();
    header.append((char*) &size, sizeof(size));

    // Apertura file
    if (size > -1) {
        mCurrentFile = new QFile(fullname);
        mCurrentFile->open(QIODevice::ReadOnly);
    }

    return header;
}

// Calcola l'occupazione totale di tutti i file da trasferire
qint64 DuktoProtocol::computeTotalSize(const QStringList& e)
{
    // Se è un invio testuale
    if ((e.length() == 1) && (e.first() == "___DUKTO___TEXT___"))
        return mTextToSend.toUtf8().length();

    // Se è un invio normale
    qint64 size = 0;
    for(QStringList::const_iterator iter = e.constBegin(); iter != e.constEnd(); ++iter)
    {
        QFileInfo fi(*iter);
        if (!fi.isDir())
            size += fi.size();
    }
    return size;
}

// Invia un pacchetto a tutti gli indirizzi broadcast del PC
void DuktoProtocol::sendToAllBroadcast(const QByteArray& packet, const QList<qint16>& ports)
{
    // Elenco interfacce disponibili
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    // Iterazione sulle interfacce
    for(QList<QNetworkInterface>::const_iterator iface = ifaces.constBegin(); iface != ifaces.constEnd(); ++iface)
    {
        // Iterazione per tutti gli IP dell'interfaccia
        QList<QNetworkAddressEntry> addrs = iface->addressEntries();

        // Invio pacchetto per ogni IP di broadcast
        for(QList<QNetworkAddressEntry>::const_iterator addr = addrs.constBegin(); addr != addrs.constEnd(); ++addr)
            if ((addr->ip().protocol() == QAbstractSocket::IPv4Protocol) && !addr->broadcast().toString().isEmpty())
            {
                for(QList<qint16>::const_iterator port = ports.constBegin(); port != ports.constEnd(); ++port)
                {
                    mSocket->writeDatagram(packet.data(), packet.length(), addr->broadcast(), *port);
                    mSocket->flush();
                }
            }
    }
}

// Interrompe un trasferimento in corso (utilizzabile solo lato invio)
void DuktoProtocol::abortCurrentTransfer()
{
    // Check if it's sending data
    if (!mIsSending) return;

    // Abort current connection
    closeCurrentTransfer(true);
    emit sendFileAborted();
}

// Aggiorna il buddy name dell'utente locale
void DuktoProtocol::updateBuddyName()
{
    // Invio pacchetto di disconnessione
    sayGoodbye();

    // Invio pacchetto di annuncio con il nuovo nome
    sayHello(QHostAddress::Broadcast, true);
}
