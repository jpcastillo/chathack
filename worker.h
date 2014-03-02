#ifndef WORKER_H
#define WORKER_H

#include <QTcpServer>
#include <QThread>
#include <QMutex>
#include "logwriter.h"


class QTcpSocket;

class Worker : public QObject
{
Q_OBJECT
public:
    Worker(qintptr socketDescriptor, QObject *parent, QThread *_self, QTcpSocket *_client, QTcpServer *_server);
    ~Worker();

    void startRun();

    static QMutex mutex;

private:
    bool setup(); // initial server setup
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

signals:
    void shouldRun();
    void clientDisconnect(QThread *);
};

#endif // WORKER_H
