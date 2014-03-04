#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QtNetwork>

Server::Server(QObject* parent) :
    QTcpServer(parent), log("chathack_daemon.log")
{
    svrPort = 6501;
    mgr = new QNetworkAccessManager();
    url_base = "http://192.168.62.195/chathack/?";
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onHttpFinish(QNetworkReply*)));
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
    Worker *worker = new Worker(handle,0,thread,NULL,this);
    workers.insert(thread,worker);

    connect(thread,SIGNAL(finished()),worker,SLOT(deleteLater()));
    connect(worker,SIGNAL(clientDisconnect(QThread*)),this,SLOT(onDisconnect(QThread*)));
    connect(this,SIGNAL(destroyed()),thread,SLOT(quit()));
    connect(worker,SIGNAL(netRequest(QString)),this,SLOT(runRequest(QString)));

    worker->moveToThread(thread);
    thread->start();
    worker->startRun();
}

void Server::onDisconnect(QThread *t)
{
    log.log("Server: Client disconnection occurred.\n");
    Worker *toDelete = workers.find(t).value();
    workers.remove(t);
    delete(toDelete);
    t->quit();
    t->wait();
    t->deleteLater();
}

void Server::runRequest(QString qryString)
{
    lastWorker = (Worker*)sender();
    //if(lastWorker->isOpen())
    {
        //qDebug() << "Server: runRequest " + qryString;
        mgr->get( QNetworkRequest(QUrl(url_base+qryString)) );
    }
}

void Server::onHttpFinish(QNetworkReply *rpy)
{
    if(lastWorker->isOpen())
    {
        connect(this,SIGNAL(onHttpFinishWorker(QNetworkReply*)),lastWorker,SLOT(onHttpFinish(QNetworkReply*)));
        emit onHttpFinishWorker(rpy);
        disconnect(this,SIGNAL(onHttpFinishWorker(QNetworkReply*)),lastWorker,SLOT(onHttpFinish(QNetworkReply*)));
    }
}
