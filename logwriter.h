#ifndef LOGWRITER_H
#define LOGWRITER_H
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <string>

class LogWriter
{
public:
    LogWriter(QString filename);
    ~LogWriter();
    void log(QString str);

private:
    QString filename;
    QFile myFile;
};

#endif // LOGWRITER_H
