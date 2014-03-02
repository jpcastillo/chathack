#include "server.h"
#include "worker.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

Server::Server(QObject* parent) :
    QTcpServer(parent), log("chathack_daemon.log")
{
    svrPort = 6501;
    tryListen();
}

Server::~Server()
{
    log.log("Server: I just died.\n");
}

void Server::tryListen()
{
    // QHostAddress::LocalHost, svrPort);
    this->listen(QHostAddress::Any,svrPort);
    if( this->isListening() )
        log.log("Server: Listening on port.\n");
    else
        log.log("Server: Unable to listen on port.\n");
}

void Server::incomingConnection(qintptr handle) // handler for new connection from client
{
    log.log("Server: New connection request.\n");
    QThread *thread = new QThread();
    Worker *worker = new Worker(handle,0,thread,NULL,this);//socket->socketDescriptor(),0,thread,socket);
    connect(thread,SIGNAL(finished()),worker,SLOT(deleteLater()));
    connect(worker,SIGNAL(clientDisconnect(QThread*)),this,SLOT(onDisconnect(QThread*)));
    connect(this,SIGNAL(destroyed()),thread,SLOT(quit()));
    //connect(socket,SIGNAL(readyRead()),worker,SLOT(onReadyRead()));
    //connect(socket,SIGNAL(disconnected()),worker,SLOT(onDisconnect()));
    //connect(thread,SIGNAL(finished()),socket,SLOT(deleteLater()));

    worker->moveToThread(thread);
    thread->start();
    worker->startRun();
}

void Server::onDisconnect(QThread *t)
{
    log.log("Server: Client disconnection occurred.\n");
    t->quit();
    t->wait();
    t->deleteLater();
}
