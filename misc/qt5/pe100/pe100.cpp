#include "pe100.h"
#include "ui_pe100.h"
#include <QDebug>
#include <QString>
#include <QColor>
pe100::pe100(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::pe100)
{   
    ui->setupUi(this);
    this->login();

    QDesktopWidget *deskwidget = QApplication::desktop();
    move((deskwidget->width() - this->width())/2,(deskwidget->height() - this->height())/2);
}

pe100::~pe100()
{
    delete ui;
}


void pe100::login()
{

    user_chose = 0;//choose user to login
    login_status = 0;//when login manager:1 operator:2
    manager_pwd = "123456";
    operator_pwd = "123456";

    this->setWindowTitle("PE100m Soft");
    this->setWindowIcon(QIcon(":/pic/soft.ico"));
    this->setFixedSize(761,609);

    ui->login_user->addItem("Manager",user_chose);
    ui->login_user->addItem("Operator",user_chose);

    ui->password_1->setMaxLength(8);
    ui->password_2->setMaxLength(8);
    ui->password_1->setEchoMode(QLineEdit::Password);
    ui->password_2->setEchoMode(QLineEdit::Password);
    ui->user_name->setMaxLength(8);
    ui->user_name->setEchoMode(QLineEdit::Normal);
    ui->user_name->setPlaceholderText("Input user name");

    //login exit
    QObject::connect(ui->login,SIGNAL(clicked()),this,SLOT(login_by_password()));
    QObject::connect(ui->exit,SIGNAL(clicked()),this,SLOT(exit_press()));
    ui->login->setShortcut(Qt::Key_Enter);
    ui->login->setShortcut(Qt::Key_Return);

    //time
    this->timer = new QTimer();
    QObject::connect(this->timer,SIGNAL(timeout()),this,SLOT(time_update()));
    timer->start(1000);
    QDateTime time_t = QDateTime().currentDateTime();
    this->time = time_t.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->time->setReadOnly(true);
    ui->time->setTextColor(QColor("black"));

    ui->time->setText(this->time);

    //help
    QObject::connect(ui->actionHelp,SIGNAL(triggered()),this,SLOT(help()));


    //manager_ui
    manager_ui.setWindowTitle("Manager");
    manager_ui.setFixedSize(800,600);
    QObject::connect(&manager_ui,SIGNAL(manager_exit()),this,SLOT(user_exit()));
}


void pe100::login_by_password()
{
    if(ui->login_user->currentText() == "Manager")
    {
        user_chose = 0;
    }
    else {
        user_chose = 1;
    }
    if(ui->password_1->text() != ui->password_2->text())
    {
        QMessageBox::warning(nullptr,"Error","Input error!",QMessageBox::Yes,QMessageBox::Yes);
        return ;
    }
    else
    {
        if(user_chose == 0)
        {
            if(manager_pwd != ui->password_1->text())
            {
                QMessageBox::warning(nullptr,"Error","Password error!",QMessageBox::Yes,QMessageBox::Yes);
            }
            login_status = 1;
        }
        if(user_chose == 1)
        {
            if(operator_pwd != ui->password_1->text())
            {
                QMessageBox::warning(nullptr,"Error","Password error!",QMessageBox::Yes,QMessageBox::Yes);
            }
            login_status = 2;
        }

    }
    if(login_status == 1)
    {
        qDebug()<<"manager login success";
        manager_ui.show();
        this->hide();
        this->timer->stop();
    }
    else if (login_status == 2) {
        qDebug()<<"operator login success";

    }
}

void pe100::exit_press()
{
    qDebug()<<"press exit!";
    login_status = 0;
    emit press_exit_slgnal();

}

void pe100::user_exit()
{
    qDebug()<<"manager exit!";
    ui->password_1->clear();
    ui->password_2->clear();
    this->timer->start();
    this->show();
}

void pe100::time_update()
{
    //qDebug()<<"time out!";
    QDateTime time_t = QDateTime().currentDateTime();
    this->time = time_t.toString("yyyy-MM-dd hh:mm:ss dddd");
    //qDebug()<<this->time;
    ui->time->setText(this->time);
}

void pe100::help()
{
    qDebug()<<"help";
    QMessageBox::about(nullptr,"Help","Pe100m soft manager.");
}
