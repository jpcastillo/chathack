#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QDebug>

#define CHATHACKTEST

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loginErrorTimer(0), movable(false)
{
    ui->setupUi(this);
    ui->menuBar->setVisible(false);
    ui->mainToolBar->setVisible(false);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui->logoutButton->hide();
    ui->toggleMicButton->hide();
    //
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(closeButton()));
    connect(ui->logoutButton,SIGNAL(clicked()),this,SIGNAL(logout()));
    connect(ui->toggleMicButton,SIGNAL(clicked()),this,SIGNAL(toggleMic()));


    connect(ui->loginButton,SIGNAL(clicked()),this,SLOT(loginButton()));

    connect(this,SIGNAL(badLogin(QString)),this,SLOT(displayLoginError(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeButton()
{
    emit logout();
    close();
}

void MainWindow::loginButton()
{
    static QRegExp validName("([0-9a-zA-Z])+");
    QString roomName(ui->roomNameLine->text()), username(ui->usernameLine->text());
    if(!validName.exactMatch(roomName))
    {
        emit badLogin("Illegal Room Name!");
        return;
    }
    if(!validName.exactMatch(username))
    {
        emit badLogin("Illegal Username!");
        return;
    }

    emit tryLogin(username, roomName);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    mouseClickX = e->x();
    mouseClickY = e->y();

    movable = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    movable = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(movable)
        move(e->globalX()-mouseClickX,e->globalY()-mouseClickY);

}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == loginErrorTimer)
    {
        e->accept();
        ui->loginErrorLabel->setText("");
    }

}

void MainWindow::displayLoginError(QString msg)
{
    ui->loginErrorLabel->setText(msg);
    if(loginErrorTimer){
        killTimer(loginErrorTimer);
        loginErrorTimer = 0;
    }
    loginErrorTimer = startTimer(2000);

}
