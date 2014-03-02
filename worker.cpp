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
               << "sulroom" << "culroom" << "ssmroom" << "csmroom" << "crecvmsg";
    connect(this,SIGNAL(shouldRun()),this,SLOT(run()));
    uuid = -1;
}

Worker::~Worker()
{
    //qDebug() << "destroyed";
    mutex.lock();
    log.log("Worker: I just died.\n");
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
    //qDebug() << "size " << args.size();
    if(args.size() == 0)
        return false;
    //qDebug() << "a " << args[0] << " b "<< args[args.size()-1];
    if(args[0] != args[args.size()-1])
        return false;
    int cntrl = myCmds.indexOf(args[0]);
    switch (cntrl)
    {
    case 0:
        //clogin
        //clogin|room|uuid|status|clogin
        write_c(QString(myCmds[cntrl+1]+"|hack|28|0|"+myCmds[cntrl+1]));
        break;
    case 2:
        //cjoin
        //cjoin|channel|status|cjoin
        write_c(QString(myCmds[cntrl+1]+"|hack|0|"+myCmds[cntrl+1]));
        break;
    case 4:
        //cleave
        //cleave|channel|status|cleave
        write_c(QString(myCmds[cntrl+1]+"|hack|0|"+myCmds[cntrl+1]));
        break;
    case 6:
        //clogout
        //clogout|status|clogout
        write_c(QString(myCmds[cntrl+1]+"|0|"+myCmds[cntrl+1]));
        break;
    case 8:
        //cexit
        //cexit|status|cexit
        write_c(QString(myCmds[cntrl+1]+"|0|"+myCmds[cntrl+1]));
        break;
    case 10:
        //culroom
        //culroom|<comma delimited list of users>|culroom
        write_c(QString(myCmds[cntrl+1]+"|john,brian|"+myCmds[cntrl+1]));
        break;
    case 12:
        //csmroom
        //csmroom|room|status|csmroom
        write_c(QString(myCmds[cntrl+1]+"|hack|0|"+myCmds[cntrl+1]));
        //crecvmsg
        //crecvmsg|room|user|message|crecvmsg
        write_c(QString(myCmds[cntrl+2]+"|hack|28|dan|"+args[4]+"|"+myCmds[cntrl+2]));
        break;
    default:
        write_c(QString("Hello, client. Idk wtf you want."));
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
