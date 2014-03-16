#include "worker.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutexLocker>
#include <QDebug>
#include <QtNetwork>
#include <stdlib.h>

QMutex Worker::mutex;

Worker::Worker(qintptr socketDescriptor, QObject *parent, QThread *_self, QTcpSocket *_client, QTcpServer *_server, QHash<QThread *, Worker *> *_workers) :
    QObject(parent), socketFd(socketDescriptor),
    log("worker_daemon.log"), self(_self), client(_client),
    server(_server)
{
    myCmds << "slogin" << "clogin" << "sjoin" << "cjoin" << "sleave"
               << "cleave" << "slogout" << "clogout" << "sexit" << "cexit"
               << "sulroom" << "culroom" << "ssmroom" << "csmroom" << "crecvmsg"
               << "suuid" << "cuuid";
    connect(this,SIGNAL(shouldRun()),this,SLOT(run()));
    workers = _workers;
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
        //ssmroom|uuid|room|type|message (must escape vbar)|ssmroom --12
        //csmroom|room|status|csmroom --13
        qryString = QString("cmd=suidlroom&c="+args[2]+"&u1="+args[1]+"&u2=&t="+args[3]+"&m="+args[4]);
        emit netRequest(qryString);
        /*write_c(QString(myCmds[cntrl+1]+"|hack|0|"+myCmds[cntrl+1]+"\n"),client);*/

        //crecvmsg|room|user|message|crecvmsg --14
        qryString = QString("cmd=srecvmsg&r="+args[1]+"&u1="+args[1]+"&u2=&c=&t=&m=");
        /*write_c(QString(myCmds[cntrl+2]+"|hack|dan|"+args[4]+"|"+myCmds[cntrl+2]+"\n"),client);*/
        break;
    case 15:
        if(args[1].length() > 0)
        {
            QString uuidStr = args[1].simplified();
            uuidStr.replace( " ", "" );
            //uuid = atoi(uuidStr.toStdString().c_str());
            uuid = uuidStr;
            qDebug() << "uuid is: " << uuid;
            write_c(QString(myCmds[cntrl+1]+"|0|"+myCmds[cntrl+1]+"\n"),client);
        }
        else
        {
            write_c(QString(myCmds[cntrl+1]+"|1|"+myCmds[cntrl+1]+"\n"),client);
        }
        break;
    default:
        write_c(QString("Hello, client. Idk wtf you want.\n"),client);
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

bool Worker::write_c(QString msg, QTcpSocket *sfd)
{
    mutex.lock();
    log.log("Worker: Started writing response to client.\n");
    mutex.unlock();
    QString tmp(msg);
    int msgLen = tmp.length();
    tmp[msgLen] = '\n';
    if(sfd->write(tmp.toStdString().c_str()) != -1 && sfd->waitForBytesWritten(50))
    {
        log.log("Worker: Finished writing `"+msg.toLocal8Bit()+"` to client.\n");
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

    /* must handle message broadcast replies differently */
    QStringList reply = parse(str);
    if(reply.size()==6 && reply[0]=="cuidlroom")
    {
        // convert comma delimited uuid into QList<QString>
        QStringList users = reply[1].split(",");
        // write response message to caller
        write_c(QString("csmroom|"+reply[3]+"|0|csmroom"),client);
        // write receive messages to callees
        messageClients(users,QString("crecvmsg|"+reply[3]+"|"+reply[4]+"|"+reply[2]+"|crecvmsg"));
    }
    else
    {
        write_c(str,client);
    }
}

bool Worker::isOpen()
{
    return client->isOpen();
}

QString Worker::getUuid()
{
    return uuid;
}

QTcpSocket* Worker::getClientSocket()
{
    return client;
}

void Worker::messageClients(QStringList users, QString msg) // 03/14/14 should look over.
{
    qDebug() << "Users size is " << users.size() << " , " << users;
    qDebug() << "Workers size is " << workers->size();
    QHash<QThread*, Worker *>::const_iterator it = workers->constBegin();
    for (int i = 0; i < users.size(); i++)
    {
        qDebug() << "Users index: " << i;
        qDebug() << "Users.at(index): " << users.at(i);
        while (it != workers->constEnd())
        {
            qDebug() << "Worker UUID: " << it.value()->getUuid();
            if(it.value()->getUuid() == users.at(i))
            {
                // it.key(); it.value();
                qDebug() << "Match found!";
                if(it.value()->getClientSocket()->isOpen())
                {
                    write_c(msg,it.value()->getClientSocket());
                }
            }
            it++;
        }
        it = workers->constBegin();
    }
}
