#ifndef WORKER_H
#define WORKER_H

#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QThread>
#include <QMutex>
#include "logwriter.h"

class QTcpSocket;

// QNetworkAccessManager
#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class Worker : public QObject
{
Q_OBJECT
public:
    Worker(qintptr socketDescriptor, QObject *parent, QThread *_self, QTcpSocket *_client, QTcpServer *_server);
    ~Worker();

    void startRun();
    bool isOpen();
    //void HttpFinish(QNetworkReply *rpy);

    static QMutex mutex;

private:
    bool processRequest(QString cmd); // will spawn a thread to handle client request
    void read();
    bool write_c(QString msg);
    QStringList parse(QString cmd);
    QTcpServer *server;
    QTcpSocket *client;
    LogWriter log;

    QStringList myCmds;
    qintptr socketFd; // server socket file descriptor
    QThread *self;
    int uuid;

private slots:
    void onReadyRead();
    void onDisconnect();
    void run();
    void onHttpFinish(QNetworkReply *rpy);

signals:
    void shouldRun();
    void clientDisconnect(QThread *);
    void netRequest(QString);

    friend class Server;
};

#endif // WORKER_H
