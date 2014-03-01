#include "database.h"
#include <QDebug>
//#include "qtdir/src/sql/drivers/psql/qsql_mysql.cpp"

Database::Database(QString host, int port, QString dbname, QString user, QString pass)
{
    connection = QSqlDatabase::addDatabase("QMYSQL");
    connection.setHostName(host);
    connection.setPort(port);
    connection.setDatabaseName(dbname);
    connection.setUserName(user);
    connection.setPassword(pass);
    if (!connection.open()) {
        qDebug() << "Cannot open database";
        /*
         QMessageBox::critical(0, qApp->tr("Cannot open database"),
             qApp->tr("Unable to establish a database connection.\n"
                      "This example needs SQLite support. Please read "
                      "the Qt SQL driver documentation for information how "
                      "to build it.\n\n"
                      "Click Cancel to exit."), QMessageBox::Cancel);
         return false;
         */
     }
}

Database::~Database()
{
    //
}

QList<QString> Database::getChannelMembers(QString name)
{
    QList<QString> members;
    QString qry1(
                "select users.name"
                 "from channel"
                 "left join users on channel.uid = users.uid"
                 "where channel.name = '" + name + "';"
                );
    QSqlQuery qry( qry1, connection );
    while ( qry.next() )
    {
         QString user = qry.value(0).toString();
         members.push_back(user);
     }
    return members;
}
