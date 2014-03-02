#include "database.h"
#include <QDebug>
#include <QtNetwork>

Database::Database(QObject *parent) :
    QNetworkAccessManager(parent)
{
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onfinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
}

void Database::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    qDebug() << str;
}

void Database::retrieve(QString qryString)
{
    //mgr->get(QNetworkRequest(QUrl("http://192.168.62.193/chathack/?"+qryString)));
}
