#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    ~Client();
signals:
  //  void ConnectionTimeout();
  //  void ServerResponseError();
   // void LoginSuccess(QString current_channel);
   // void LoginFailure();
   // void ForcedDisconnect();
      void LOGIN(QString n1, QString n2);
      void LOGOUT();
      void EXIT();
public slots:
    void handleConnection();
    void start_run(QString n1, QString n2);
    void login(QString Cname, QString Rname);
    void logout();
    void exit();
   // void SendMessage(Qstring message);
   // void toggleMic();
   // void SwapChannels(Qstring name);
    void ReadSocket();

private:
    bool hasCompleteMsg(QString &result);
    void ParseMessage(QString &msg);

    QTcpSocket *tcpSocket;
    bool micOn;
    bool exitFlag;
    quint16 blockSize;
    std::vector<QString> cur_args;
};

#endif // CLIENT_H
