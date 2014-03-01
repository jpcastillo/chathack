#include "client.h"

#include <vector>
#include <string>
#include <iostream>
#include <QDebug>
#include <QWidget>
#include <QApplication>

const QHostAddress DEFAULT_HOST("localhost");
const quint16 DEFAULT_PORT = 6499;
const quint16 BLOCK_SIZE = 1;
const int NUM_VALID_MSGS = 6;
const QString validMsgs[NUM_VALID_MSGS] = {"clogin", "cjoin", "clogout", "cexit", "culroom", "csmroom" };



Client::Client(QObject *parent) :
    QObject(parent), micOn(false),exitFlag(false)
{
    tcpSocket = new QTcpSocket(this);
    blockSize = BLOCK_SIZE;
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(exit()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(ReadSocket()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(handleConnection()));

}

Client::~Client()
{
    tcpSocket->deleteLater();
    exitFlag = true;
}

void Client::handleConnection()
{
    qDebug() << "HANDLED CONNECTION!";
}


void Client::login(QString Cname, QString Rname)
{
    tcpSocket -> connectToHost(QHostAddress::LocalHost, DEFAULT_PORT);
    qDebug() << "STARTING LOGIN!";
    if(!tcpSocket -> waitForConnected())
    {
        qDebug() << "ConnectionTimeout" << "";
        //emit ConnectionTimeout();
        return;
    }
    else
    {
        qDebug() << "Connected To Host" << "";
        //emit connected();
    }
    QString msg("slogin|" + Rname+ '|' + Cname + "|slogin\n");
    qDebug() << msg << "";
    int test = tcpSocket -> write(msg.toStdString().c_str());

    QString recieved = "";
    qDebug() << "test = " << test << "Entering wait loop" << "";
    while(!hasCompleteMsg(recieved))
    {
        QApplication::processEvents();
        sleep(1);
        if(exitFlag)
        {
            qDebug() << "Exiting from login";
            return;
        }
        //qDebug() << "POOOP!";

        //ReadSocket();
    }
    qDebug() << "Exiting wait loop" << "";
    if(recieved != "clogin")
    {
        qDebug() << "Server Response Error" << "";
        //emit ServerResponseError(); //Will probably never happen Server should respond correctly...
        cur_args.clear();
        return;
    }
    std::vector<QString> recieved_args;
    for (int i = 1; i < cur_args.size(); ++i)
    {
        if(cur_args[i] != "clogin")
        {
               recieved_args.push_back(cur_args[i]);
        }
        else
        {
            break;
        }
    }
    if(recieved_args.size() != 3)
    {
        qDebug() << "Server response error" << "";
        //emit ServerResponseError();
        cur_args.clear();
        return;
    }
    if(recieved_args[2] == "1")
    {
      //  qDebug() << "LOGIN SUCCESS: " << recieved_args[0] << "";
        //emit LoginSuccess(recieved_args[0]);
    }
    else
        qDebug() << "Login Failure" << "";
        //emit LoginFailure();

    cur_args.clear();

    qDebug() << "Exit Login";

}


void Client::logout()
{
    tcpSocket -> disconnectFromHost();
    if(!tcpSocket -> waitForDisconnected())
    {
        //Force Connection Close
        tcpSocket -> close();
        qDebug() << "Forced Disconnect" << "";
        //emit ForcedDisconnect();
    }
    qDebug() << "Disconnected" << endl;
    //emit disconnected();

}

void Client::exit()
{
    //Force Connection Close
    exitFlag = true;
    tcpSocket -> close();
    qDebug() << "Disconnected" << "";
    //emit disconnected();
}

void Client::ReadSocket()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    qDebug() << QString("ReadSocket %1");
    /*if (tcpSocket->bytesAvailable() < blockSize)
    {
        qDebug() << tcpSocket->bytesAvailable();
        return;
    }*/

    int bytesRead;

    char nextRead[80];
    qDebug() << "BEFORE";
    bytesRead = in.readRawData(nextRead, 80);
    if(bytesRead == -1)
    {
        nextRead[0] = 0;
    }
    else
    {
        nextRead[bytesRead-1] = 0;
    }
    qDebug() << "AFTER";
    QString temp(nextRead);
    qDebug() << temp;
    ParseMessage(temp);

    qDebug() << "Done ReadSocket";
}

void Client::ParseMessage(QString &msg)
{
    qDebug() << QString("Parse Message: %1").arg(msg);
    QString cur_arg = "";
    for(int i = 0; i < msg.length(); ++i)
    {
        if(msg[i] == '|')
        {
            qDebug() << (int)msg[i].toLatin1();
            cur_args.push_back(cur_arg);
            cur_arg = "";

        }
        /*else if(msg[i] == '\n')
        {
            cur_args.push_back(cur_arg);
            cur_arg = "";
            break;
        }*/
        else
        {
          qDebug() << (int)msg[i].toLatin1();
          cur_arg += msg[i];
        }
    }

    if(cur_arg.length())
    {
        cur_args.push_back(cur_arg);
        cur_arg = "";
    }

    qDebug() << QString("Done Parsing: %1").arg(cur_args.size());

    msg = "";
}

bool Client::hasCompleteMsg(QString &result)
{
    static int size = 0, lastSize = 0;

    if(cur_args.size() >= 2 && (cur_args[0] == cur_args[cur_args.size() - 1]))
    {
        for(int i = 0; i < NUM_VALID_MSGS; ++i)
        {
            if( validMsgs[i] == cur_args[0])
            {
                result = validMsgs[i];
                return true;
            }
        }
    }
    size = cur_args.size();
    if(size != lastSize)
         qDebug() << size;
    lastSize = size;
    result = "";
    return false;
}

void Client::start_run(QString n1, QString n2)
{
    emit LOGIN(n1, n2);
}
