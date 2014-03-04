#include "worker.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutexLocker>
#include <QDebug>
#include <QtNetwork>

QMutex Worker::mutex;

Worker::Worker(qintptr socketDescriptor, QObject *parent, QThread *_self, QTcpSocket *_client, QTcpServer *_server) :
    QObject(parent), socketFd(socketDescriptor),
    log("chathack_workerdaemon.log"), self(_self), client(_client),
    server(_server)
{
    myCmds << "slogin" << "clogin" << "sjoin" << "cjoin" << "sleave"
               << "cleave" << "slogout" << "clogout" << "sexit" << "cexit"
               << "sulroom" << "culroom" << "ssmroom" << "csmroom" << "crecvmsg"
               << "suuid" << "cuuid";
    connect(this,SIGNAL(shouldRun()),this,SLOT(run()));
    uuid = -1;
}

Worker::~Worker()
{
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
    client = new QTcpSocket();
    if (!client->setSocketDescriptor(socketFd))
    {
        mutex.lock();
        log.log("Worker: Thread error, invalid socket descriptor.\n");
        mutex.unlock();
        return;
    }

    //qDebug() << "Worker: client address is " << client->peerAddress();

    connect(self,SIGNAL(finished()),client,SLOT(deleteLater()));
    connect(client,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(client,SIGNAL(disconnected()),this,SLOT(onDisconnect()));

    mutex.lock();
    log.log("Worker: Worker thread spawned successfully.\n");
    mutex.unlock();
}

bool Worker::processRequest(QString cmd) // will spawn a thread to handle client request
{
    QStringList args(parse(cmd));
    QString qryString;
    if(args.size() == 0)
        return false;
    if(args[0] != args[args.size()-1])
        return false;
    int cntrl = myCmds.indexOf(args[0]);
    switch (cntrl)
    {
    case 0:
        //slogin|room|username|type|slogin
        //clogin|room|uuid|status|clogin
        qryString = QString("cmd=slogin&c="+args[1]+"&u1=&u2="+args[2]+"&t="+args[3]+"&m=");
        emit netRequest(qryString);
        //mgr->get( QNetworkRequest(QUrl(url_base+qryString)) );
        //write_c(QString(myCmds[cntrl+1]+"|hack|28|0|"+myCmds[cntrl+1]+"\n"));
        break;
    case 2:
        //sjoin|uuid|channel|type|sjoin
        //cjoin|channel|status|cjoin
        qryString = QString("cmd=sjoin&c="+args[2]+"&u1="+args[1]+"&u2=&t="+args[3]+"&m=");
        emit netRequest(qryString);
        break;
    case 4:
        //cleave|channel|status|cleave
        //sleave|uuid|channel|type|sleave
        qryString = QString("cmd=sleave&c="+args[2]+"&u1="+args[1]+"&u2=&t="+args[3]+"&m=");
        emit netRequest(qryString);
        break;
    case 6:
        //clogout|status|clogout
        qryString = QString("cmd=slogout&c=&u1="+args[1]+"&u2=&t=&m=");
        emit netRequest(qryString);
        break;
    case 8:
        //cexit|status|cexit
        qryString = QString("cmd=sexit&c=&u1="+args[1]+"&u2=&t=&m=");
        emit netRequest(qryString);
        break;
    case 10:
        //culroom|<comma delimited list of users>|culroom
        qryString = QString("cmd=sulroom&c="+args[1]+"&u1=&u2=&t=&m=");
        emit netRequest(qryString);
        break;
    case 12:
        //csmroom
        //csmroom|room|status|csmroom
        //ssmroom|uuid|room|type|message (must escape vbar)|ssmroom
        qryString = QString("cmd=ssmroom&c="+args[2]+"&u1="+args[1]+"&u2=&t="+args[3]+"&m="+args[4]);
        //mgr->get( QNetworkRequest(QUrl(url_base+qryString)) );
        write_c(QString(myCmds[cntrl+1]+"|hack|0|"+myCmds[cntrl+1]+"\n"));
        //crecvmsg
        //crecvmsg|room|user|message|crecvmsg
        qryString = QString("cmd=srecvmsg&r="+args[1]+"&u1="+args[1]+"&u2=&c=&t=&m=");
        //mgr->get( QNetworkRequest(QUrl(url_base+qryString)) );
        write_c(QString(myCmds[cntrl+2]+"|hack|dan|"+args[4]+"|"+myCmds[cntrl+2]+"\n"));
        break;
    case 15:
        if(args[1].length() > 0)
        {
            uuid = atoi(args[1].toStdString().c_str());
            write_c(QString(myCmds[cntrl+1]+"|0|"+myCmds[cntrl+1]+"\n"));
        }
        else
        {
            write_c(QString(myCmds[cntrl+1]+"|1|"+myCmds[cntrl+1]+"\n"));
        }
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
    log.log("Worker: Started writing response to client.\n");
    mutex.unlock();
    QString tmp(msg);
    int msgLen = tmp.length();
    tmp[msgLen] = '\n';
    if(client->write(tmp.toStdString().c_str()) != -1)
    {
        log.log("Worker: Finished writing response to client.\n");
        return true;
    }
    log.log("Worker: Failed to write response to client.\n");
    return false;
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

void Worker::onHttpFinish(QNetworkReply *rply)
{
    QByteArray bts = rply->readAll();
    QString str(bts);
    rply->deleteLater();
    write_c(str);
}

bool Worker::isOpen()
{
    return client->isOpen();
}
