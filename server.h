#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QHash>
#include <string>
#include "logwriter.h"
#include "worker.h"

class QTcpSocket;

using namespace std;

#define MAX_CONNECTS 10


class Server : public QTcpServer
{
    Q_OBJECT
    public:
        Server( QObject* parent = 0 ); // default cnstr
        ~Server(); // default destr
        void tryListen();

    private:
        int socketFd; // server socket file descriptor
        quint16 svrPort; // server port to listen on
        QTcpSocket *socket;
        LogWriter log;
        QHash<QThread *,Worker *> workers;

    protected:
        virtual void incomingConnection(qintptr handle);

    private slots:
        void onDisconnect(QThread *t);
};

#endif
