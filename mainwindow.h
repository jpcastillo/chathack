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
    void displayLoginError(QString);

signals:
    void logout();
    void toggleMic();
    void login();
    void badLogin(QString);
    void tryLogin(QString, QString);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);

private:
    Ui::MainWindow *ui;
    int mouseClickX, mouseClickY;
    int loginErrorTimer;
    bool movable;
};

#endif // MAINWINDOW_H
