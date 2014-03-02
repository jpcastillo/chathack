#include "server.h"
#include "worker.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

Server::Server(QObject* parent) :
    QTcpServer(parent), log("chathack_daemon.log")
{
    /*myCmds << "slogin" << "clogin" << "sjoin" << "cjoin" << "sleave"
           << "cleave" << "slogout" << "clogout" << "sexit" << "cexit"
           << "sulroom" << "culroom" << "ssmroom" << "csmroom";*/
    server = new QTcpServer(this);
    svrPort = 6501;
    //qDebug() << "Constructing Server";
    //connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnect()));
    //listen();
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnect()));

    qDebug() << server->listen(QHostAddress::Any,svrPort);//(QString("169.235.31.7")),svrPort);
    if( server->isListening() )
        log.log("Server: Listening on port.\n");
    else
        log.log("Server: Unable to listen on port.\n");
}

Server::~Server()
{
    //qDebug() << "Server done";
    delete(server);
}

/*void Server::listen()
{
    // QHostAddress::LocalHost, svrPort);
    server->listen(QHostAddress::Any,svrPort);//QString("169.235.31.7")),svrPort);
    if( server->isListening() )
        log.log("Server: Listening on port.\n");
    else
        log.log("Server: Unable to listen on port.\n");
}*/

void Server::incomingConnection(qintptr handle) // handler for new connection from client
{
    log.log("Server: New connection request.\n");
    //socket = server->nextPendingConnection();

    qDebug() << (long long)handle;
    if(1)//socket->state() == QTcpSocket::ConnectedState)
    {
        log.log("Server: New client connection successful.\n");
        QThread *thread = new QThread();
        Worker *worker = new Worker(handle,0,thread,NULL);//socket->socketDescriptor(),0,thread,socket);
        connect(thread,SIGNAL(finished()),worker,SLOT(deleteLater()));
        connect(thread,SIGNAL(finished()),socket,SLOT(deleteLater()));
        connect(worker,SIGNAL(clientDisconnect(QThread*)),this,SLOT(onDisconnect(QThread*)));
        connect(socket,SIGNAL(readyRead()),worker,SLOT(onReadyRead()));
        connect(socket,SIGNAL(disconnected()),worker,SLOT(onDisconnect()));
        connect(this,SIGNAL(destroyed()),thread,SLOT(quit()));
        //socket->moveToThread(thread);

        worker->moveToThread(thread);
        thread->start();
        worker->startRun();
    }
    else
    {
        log.log("Server: New client connection failed .\n");
    }
}

void Server::onDisconnect(QThread *t)
{
    log.log("Server: Client disconnection occurred.\n");
    socket->deleteLater();
    t->deleteLater();
    t->quit();
    t->wait(); // need to change!
    t->deleteLater();
}

void Server::onNewConnect()
{
    qDebug() << "You shouldn't be here!";
}
