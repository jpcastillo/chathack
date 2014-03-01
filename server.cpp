#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>

Server::Server(QObject* parent) :
    QTcpServer(parent), log("chathack_daemon.log")
{
    myCmds << "slogin" << "clogin" << "sjoin" << "cjoin" << "sleave"
           << "cleave" << "slogout" << "clogout" << "sexit" << "cexit"
           << "sulroom" << "culroom" << "ssmroom" << "csmroom";
    server = new QTcpServer(this);
    svrPort = 6501;
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnect()));
    connect(server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onAcceptError(QAbstractSocket::SocketError)));
    listen();
}

Server::~Server()
{
    delete(socket);
    delete(server);
}

void Server::listen()
{
    // QHostAddress::LocalHost, svrPort);
    server->listen(QHostAddress(QString("169.235.31.7")),svrPort);
    if( server->isListening() )
        log.log("is listening on port " + QString(svrPort) + "...\n");
    else
        log.log("not listening!!\n");
}

void Server::incomingConnection(qintptr sfd)
{
    log.log("incoming connection...\n");
}

bool Server::setup() // initial server setup
{
    return false;
}

bool Server::processRequest(QString cmd) // will spawn a thread to handle client request
{
    QStringList args(parse(cmd));
    qDebug() << "size " << args.size();
    if(args.size() == 0)
        return false;
    qDebug() << "a " << args[0] << " b "<< args[args.size()-1];
    if(args[0] != args[args.size()-1])
        return false;
    int cntrl = myCmds.indexOf(args[0]);
    switch (cntrl)
    {
    case 0:
        qDebug() << "wtf happened";
        write_c(QString("Hello, client. I got your login command!!\n"));
        break;
    default:
        write_c(QString("Hello, client. Idk wtf you want.\n"));
        break;
    }
    //foreach(const QString &str, args)
        //qDebug() << str;

    return true;
}

void Server::onNewConnect() // handler for new connection from client
{
    log.log("New connection request...\n");
    socket = server->nextPendingConnection();

    if(socket->state() == QTcpSocket::ConnectedState)
    {
        printf("New client connection ocurred.\n");
    }

    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void Server::onDisconnect()
{
    log.log("Client disconnection occurred!!\n");
    disconnect(socket, SIGNAL(disconnected()));
    disconnect(socket, SIGNAL(readyRead()));
    socket->deleteLater();
}

void Server::onReadyRead()
{
    read();
}

void Server::onAcceptError(QAbstractSocket::SocketError socketError)
{
    log.log("Accept error occurred...\n");
}

void Server::read()
{
    QByteArray clientByteArray;
    while(socket->canReadLine())
    {
        clientByteArray = socket->readLine();
        if( !clientByteArray.contains("\0") )
        {
            log.log("Read null character from client... Ending read...\n");
            break;
        }
        log.log("server read the following from client: " + QString(clientByteArray.constData()));
    }
    log.log("Server finished reading from client...\n");
    if (processRequest( QString(clientByteArray.constData()) ) )
    {
        qDebug() << "process worked";
    }
    else
    {
        qDebug() << "process failed";
    }
}


bool Server::write_c(QString msg)
{
    log.log("Writing to client...\n");
    QString tmp(msg);
    int msgLen = tmp.length();
    tmp[msgLen] = '\n';
    //socket->write(tmp.toStdString().c_str());
    return socket->write(tmp.toStdString().c_str()) != -1;
}

QStringList Server::parse(QString cmd)
{
    QRegExp regex("(\\|)"); // vertical bar
    return cmd.split(regex);
}
