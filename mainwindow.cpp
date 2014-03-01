#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loginErrorTimer(0), movable(false),
    c(new Client()),
    workerThread(new QThread(this))
{
    workerThread->start();
    connect(workerThread,SIGNAL(finished()),workerThread,SLOT(deleteLater()));
    ui->setupUi(this);
    ui->menuBar->hide();
    ui->mainToolBar->hide();
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    hideLoggedInStuff();
    //ui->textBrowser->setOpenExternalLinks(true);
    //
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(closeButton()));
    connect(ui->logoutButton,SIGNAL(clicked()),this,SLOT(logoutButton()));
    connect(ui->toggleMicButton,SIGNAL(clicked()),this,SIGNAL(toggleMic()));


    connect(ui->loginButton,SIGNAL(clicked()),this,SLOT(loginButton()));
    connect(ui->roomNameLine,SIGNAL(returnPressed()),this,SLOT(loginButton()));
    connect(ui->usernameLine,SIGNAL(returnPressed()),this,SLOT(loginButton()));

    connect(ui->sendButton,SIGNAL(clicked()),this,SLOT(sendMessageButton()));
    connect(ui->chatBox,SIGNAL(returnPressed()),this,SLOT(sendMessageButton()));

    //TEST
    connect(this,SIGNAL(tryLogin(QString,QString)),this,SLOT(login()));
    connect(this,SIGNAL(tryLogout(QString)),this,SLOT(logout()));
    connect(this,SIGNAL(trySendMessage(QString)),this,SLOT(handleSendMessage(QString)));
    //END TEST

    connect(this,SIGNAL(badLogin(QString)),this,SLOT(displayLoginError(QString)));
    connect(c,SIGNAL(LOGIN(QString,QString)), c,SLOT(login(QString,QString)));
    connect(this,SIGNAL(startConnection(QString,QString)),c,SLOT(start_run(QString,QString)));
    c->moveToThread(workerThread);
    emit startConnection("zach", "1");
}

MainWindow::~MainWindow()
{
    delete ui;
    c->deleteLater();
    //workerThread->terminate();

    workerThread->wait();
}

void MainWindow::closeButton()
{
    emit tryClose();
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

void MainWindow::logoutButton()
{
    emit tryLogout("Current Room");
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    mouseClickX = e->x();
    mouseClickY = e->y();

    movable = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
   Q_UNUSED(e)
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

void MainWindow::showLoggedInStuff()
{
    ui->logoutButton->show();
    ui->toggleMicButton->show();
    ui->chatIcon->show();
    ui->roomLabel->show();
    ui->chatMessageLabel->show();
}

void MainWindow::hideLoggedInStuff()
{
    ui->logoutButton->hide();
    ui->toggleMicButton->hide();
    ui->chatIcon->hide();
    ui->roomLabel->hide();
    ui->chatMessageLabel->hide();
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

void MainWindow::login()
{
    showLoggedInStuff();
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    ui->roomLabel->setText(QString("%1@%2").arg(ui->usernameLine->text()).arg(ui->roomNameLine->text()));
}

void MainWindow::logout()
{
    hideLoggedInStuff();
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void MainWindow::sendMessageButton()
{
    emit trySendMessage(ui->chatBox->text().toHtmlEscaped());
    ui->chatBox->clear();
}

void MainWindow::handleSendMessage(QString msg)
{
    qDebug() << msg;
}
