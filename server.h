#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QHash>
#include <string>
#include "logwriter.h"
#include "worker.h"

class QTcpSocket;

// QNetworkAccessManager
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
        QHash<QThread *,Worker *> workers;
        QNetworkAccessManager *mgr;
        QString url_base;

    signals:
        void onHttpFinishWorker(QNetworkReply *);

    protected:
        virtual void incomingConnection(qintptr handle);

    private slots:
        void onDisconnect(QThread *t);
        void runRequest(QString qryString);
        void onHttpFinish(QNetworkReply *rpy);
private:
        Worker * lastWorker;
};

#endif
