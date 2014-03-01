#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <iostream>
#include <string>
#include "logwriter.h"

//#include "socketreader.h"
//#include "datareader.h"

//class QTcpServer;
class QTcpSocket;


using namespace std;

#define MAX_CONNECTS 10


class Server : public QTcpServer
{
    Q_OBJECT
    public:
        Server( QObject* parent = 0 ); // default cnstr
        ~Server(); // default destr
        void listen();

    protected:
        void incomingConnection(qintptr sfd);
        QStringList myCmds;

    private:
        bool setup(); // initial server setup
        bool processRequest(QString cmd); // will spawn a thread to handle client request
        void read();
        bool write_c(QString msg);
        QStringList parse(QString cmd);
        qintptr socketFd; // server socket file descriptor
        quint16 svrPort; // server port to listen on
        QTcpServer *server;
        QTcpSocket *socket;
        LogWriter log;

    private slots:
        void onNewConnect(); // handler for new connection from client
        void onDisconnect();
        void onReadyRead();
        void onAcceptError(QAbstractSocket::SocketError socketError);
};

#endif
