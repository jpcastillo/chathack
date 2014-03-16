#include <QCoreApplication>
#include <iostream>
#include "server.h"
#include <QDebug>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server myServer;
    myServer.tryListen();

    return a.exec();
}
