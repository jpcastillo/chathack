#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <QScrollBar>

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
    //button slots
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(closeButton()));
    connect(ui->logoutButton,SIGNAL(clicked()),this,SLOT(logoutButton()));
    connect(ui->toggleMicButton,SIGNAL(clicked()),this,SIGNAL(toggleMic()));

    //GUI login slots
    connect(ui->loginButton,SIGNAL(clicked()),this,SLOT(loginButton()));
    connect(ui->roomNameLine,SIGNAL(returnPressed()),this,SLOT(loginButton()));
    connect(ui->usernameLine,SIGNAL(returnPressed()),this,SLOT(loginButton()));

    //send chat messages to client
    connect(ui->sendButton,SIGNAL(clicked()),this,SLOT(sendMessageButton()));
    connect(ui->chatBox,SIGNAL(returnPressed()),this,SLOT(sendMessageButton()));

    //send other commands to client
    connect(this,SIGNAL(tryLogin(QString,QString)),c,SLOT(slogin(QString,QString)));
    connect(this,SIGNAL(tryLogout()),c,SLOT(slogout()));
    connect(this,SIGNAL(trySendMessage(QString,QString,QString)),c,SLOT(ssmroom(QString,QString,QString)));
    //connect(this,SIGNAL(trySendMessage(QString)),this,SLOT(handleSendMessage(QString)));

    //recieve messages from clients and respond to them
    connect(c,SIGNAL(loginSuccess(QString)),this,SLOT(login(QString)));
    connect(c,SIGNAL(logout()),this,SLOT(logout()));
    connect(c,SIGNAL(recievedText(QString,QString,QString)),this,SLOT(handleRecieveText(QString,QString,QString)));
    connect(c,SIGNAL(join(QString)),this,SLOT(joinRoom(QString)));
    connect(c,SIGNAL(die()),this,SLOT(close()));
    connect(c,SIGNAL(leave(QString)),this,SLOT(leaveRoom(QString)));
    connect(c,SIGNAL(usersListRoom(QStringList)),this,SLOT(usersListRoom(QStringList)));

    connect(this,SIGNAL(badLogin(QString)),this,SLOT(displayLoginError(QString)));
    c->moveToThread(workerThread);
    //emit startConnection("zach", "1");
}

MainWindow::~MainWindow()
{
    delete ui;
    c->deleteLater();
    workerThread->quit();

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
    //login();
}

void MainWindow::logoutButton()
{
    emit tryLogout();
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
    ui->leaveButton->show();
    ui->joinButton->show();
}

void MainWindow::hideLoggedInStuff()
{
    ui->logoutButton->hide();
    ui->toggleMicButton->hide();
    ui->chatIcon->hide();
    ui->roomLabel->hide();
    ui->chatMessageLabel->hide();
    ui->leaveButton->hide();
    ui->joinButton->hide();
}

void MainWindow::setRoom(QString room)
{
    QString user = c->userName;
    ui->roomLabel->setText(QString("%1@%2").arg(user).arg(room));
    ui->usersListWidget->clear();
    ui->usersListWidget->addItems(this->users[room]);
    foreach(QListWidgetItem * item, ui->roomsListWidget->selectedItems())
    {
        item->setSelected(false);
    }

    foreach(QListWidgetItem * item, ui->roomsListWidget->findItems(room,Qt::MatchExactly))
    {
        item->setSelected(true);
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

void MainWindow::login(QString room)
{
    showLoggedInStuff();
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    ui->roomsListWidget->clear();
    ui->roomsListWidget->addItem(room);
    setRoom(room);
}

void MainWindow::logout()
{
    hideLoggedInStuff();
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void MainWindow::sendMessageButton()
{
    emit trySendMessage(c->roomName,"0",ui->chatBox->text().toHtmlEscaped());
    ui->chatBox->clear();
}

void MainWindow::handleSendMessage(QString msg)
{
    //qDebug() << msg;
    handleRecieveText("1","dpasillas",msg);
}

void MainWindow::handleRecieveText(QString channel, QString user, QString msg)
{
    QString messageFormat(
                "<p "
                "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                "<span style=\" font-weight:600; color:#36e1d3;\">"
                "%1: "
                "</span>"
                "<span style=\" color:#ffffff\">"
                "%2"
                "</span></p>"
                );
    ui->textBrowser->append(messageFormat.arg(user,msg));
    //qDebug() << QString("HTML escaped: ") + messageFormat.arg(user,msg);

    ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
}

void MainWindow::joinRoom(QString room)
{
    ui->roomsListWidget->addItem(room);
    setRoom(room);
}

void MainWindow::leaveRoom(QString room)
{
    this->roomText.remove(room);
    this->users.remove(room);
}

void MainWindow::usersListRoom(QStringList users)
{
    qDebug() << "void MainWindow::usersListRoom(QStringList users)";
    this->users[c->roomName] = users;
    setRoom(c->roomName);
}
