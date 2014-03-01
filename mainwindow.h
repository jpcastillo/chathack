#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void closeButton();
    void loginButton();
    void logoutButton();
    void displayLoginError(QString);
    void login();
    void logout();
    void sendMessageButton();
    void handleSendMessage(QString msg);

signals:
    void tryClose();
    void tryLogout(QString);
    void toggleMic();
    void badLogin(QString);
    void tryLogin(QString, QString);
    void trySendMessage(QString);

    //zach's signals
//    void ConnectionTimeout();
//    void ServerResponseError();
//    void LoginSuccess(QString current_channel);
//    void LoginFailure();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);

private:
    void showLoggedInStuff();
    void hideLoggedInStuff();

    Ui::MainWindow *ui;
    int mouseClickX, mouseClickY;
    int loginErrorTimer;
    bool movable;
};

#endif // MAINWINDOW_H
