#include "worker.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutexLocker>
#include <QDebug>

QMutex Worker::mutex;

Worker::Worker(qintptr socketDescriptor, QObject *parent, QThread *_self, QTcpSocket *_client, QTcpServer *_server) :
    QObject(parent), socketFd(socketDescriptor),
    log("chathack_workerdaemon.log"), self(_self), client(_client),
    server(_server)
{
    myCmds << "slogin" << "clogin" << "sjoin" << "cjoin" << "sleave"
               << "cleave" << "slogout" << "clogout" << "sexit" << "cexit"
               << "sulroom" << "culroom" << "ssmroom" << "csmroom";
    connect(this,SIGNAL(shouldRun()),this,SLOT(run()));
}

Worker::~Worker()
{
    //qDebug() << "destroyed";
    mutex.lock();
    log.log("Worker: I was just killed.\n");
    mutex.unlock();
}

void Worker::startRun()
{
    emit shouldRun();
}

void Worker::run()
{
    //qDebug() << "void Worker::run()";
    client = new QTcpSocket();
    if (!client->setSocketDescriptor(socketFd))
    {
        mutex.lock();
        log.log("Worker: Thread error, invalid socket descriptor.\n");
        mutex.unlock();
        return;
    }

    connect(self,SIGNAL(finished()),client,SLOT(deleteLater()));
    connect(client,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(client,SIGNAL(disconnected()),this,SLOT(onDisconnect()));

    mutex.lock();
    log.log("Worker: Worker thread spawned successfully.\n");
    mutex.unlock();

    //client->waitForDisconnected();
}


bool Worker::setup() // initial server setup
{
    return false;
}

bool Worker::processRequest(QString cmd) // will spawn a thread to handle client request
{
    //a  "sjoin"  b  "sjoin"
    //slogin|channel|type|slogin
    QStringList args(parse(cmd));
    qDebug() << "size " << args.size();
    if(args.size() == 0)
        return false;
    //qDebug() << "a " << args[0] << " b "<< args[args.size()-1];
    if(args[0] != args[args.size()-1])
        return false;
    int cntrl = myCmds.indexOf(args[0]);
    switch (cntrl)
    {
    case 0:
        //qDebug() << "wtf happened";
        write_c(QString("Hello, client. I got your login command!!\n"));
        break;
    case 1:
        write_c(QString("Hello, client. I got your join command!!\n"));
        break;
    default:
        write_c(QString("Hello, client. Idk wtf you want.\n"));
        break;
    }

    return true;
}

void Worker::onReadyRead()
{
    read();
}

void Worker::read()
{
    QString log_str("");
    QByteArray clientByteArray;
    while(client->canReadLine())
    {
        clientByteArray = client->readLine();
        if( !clientByteArray.contains("\0") )
        {
            log_str = "Worker: Read null character from client. Ending read.\n";
            break;
        }
        log_str = "Worker: Read the following from client... " + QString(clientByteArray.constData());
    }

    {
        QMutexLocker locker(&mutex);
        log.log(log_str);
    }

    mutex.lock();
    log.log("Worker: Server finished reading from client.\n");
    mutex.unlock();

    if(clientByteArray.constData()[strlen(clientByteArray.constData())-1] == '\n')
        clientByteArray.chop(1);
    if (processRequest( QString(clientByteArray.constData()) ) )
    {
        mutex.lock();
        log.log("Worker: Processed client request successfully.\n");
        mutex.unlock();
    }
    else
    {
        mutex.lock();
        log.log("Worker: Could not processed client request.\n");
        mutex.unlock();
    }
}


bool Worker::write_c(QString msg)
{
    mutex.lock();
    log.log("Worker: Writing response to client.\n");
    mutex.unlock();
    QString tmp(msg);
    int msgLen = tmp.length();
    tmp[msgLen] = '\n';
    return client->write(tmp.toStdString().c_str()) != -1;
}

QStringList Worker::parse(QString cmd)
{
    QRegExp regex("(\\||\\\n)"); // vertical bar
    return cmd.split(regex);
}


void Worker::onDisconnect()
{
    mutex.lock();
    log.log("Worker: Client disconnection occurred.\n");
    mutex.unlock();
    emit clientDisconnect(self);
}
