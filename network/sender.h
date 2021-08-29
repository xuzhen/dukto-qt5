#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QAbstractSocket>
#include "filedata.h"

class QTcpSocket;

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(const QString &dest, quint16 port, QObject *parent = nullptr);

    void sendFiles(const QStringList &paths);
    void sendFile(const QString &path, const QString &name = QString());
    void sendText(const QString &text);
    void abort();

signals:
    void started(qint64 totalSize);
    void progress(qint64 total, qint64 sent);
    void completed();
    void aborted(QString error);

private slots:
    void sendData();
    void connectionError(QAbstractSocket::SocketError error);

private:
    void reportError(const QString &error);
    void disconnectSlots();

    QTcpSocket *socket;
    QString dest;
    quint16 port;

    QList<FileData> filesToSend;
    int currentFileIndex = 0;
    FileData *currentFile = nullptr;
    QByteArray textToSend;
    qint64 totalBytes = 0;
    qint64 totalBytesSent = 0;

    enum SEND_PHASE {
        PHASE_TOTAL_ELEMENTS_AND_SIZE,
        PHASE_ELEMENT_NAME_AND_SIZE,
        PHASE_ELEMENT_DATA,
        PHASE_FINALIZATION,
    } sendStatus = PHASE_TOTAL_ELEMENTS_AND_SIZE;

    static QByteArray textElementName;
};

#endif // SENDER_H
