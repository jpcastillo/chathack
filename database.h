#ifndef DATABASE_H
#define DATABASE_H

#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

class Database : public QNetworkAccessManager
{
Q_OBJECT
public:
    Database( QObject *parent = 0 );
    void retrieve(QString qryString);
private slots:
    void onfinish(QNetworkReply *);
};

#endif // DATABASE_H
