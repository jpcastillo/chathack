#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QHash>
#include <string>
#include "logwriter.h"
#include "worker.h"

class QTcpSocket;

#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

using namespace std;

#define MAX_CONNECTS 10

//client->peerAddress()


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
        QHash<QThread *,Worker *> *workers;
        QNetworkAccessManager *mgr, *svr_mgr;
        QString url_base;
        //QList<QTcpSocket*> getClientSockets(QList<QString> users);

    signals:
        void onHttpFinishWorker(QNetworkReply *);

    protected:
        virtual void incomingConnection(qintptr handle);

    private slots:
        void onDisconnect(QThread *t);
        void runRequest(QString qryString);
        void onHttpFinish(QNetworkReply *rpy);
        void onSvrHttpFinish(QNetworkReply *rpy);
private:
        Worker * lastWorker;
};

/*
 * how to handle msg broadcast:
 * worker does net request to server for list of users in channel (uuids)
 * worker sends list of uuids to server --x
 * server searches hash for match of each uuid and pushes client socket onto list --x
 * server sends back worker a list of client sockets --x
 * worker searches hash for math of each uuid and pushes client socket onto list
 * worker writes message to each socket in list
 *
*/
#endif
