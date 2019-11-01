#include "manager.h"
#include "ui_manager.h"
#include <QDebug>
manager::manager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::manager)
{
    ui->setupUi(this);
    QDesktopWidget *deskwidget = QApplication::desktop();
    move((deskwidget->width() - this->width())/2,(deskwidget->height() - this->height())/2);//move to senter


    file_name = "device_file";
    file = new QFile(file_name);
    QObject::connect(ui->push_opendevice,SIGNAL(clicked()),this,SLOT(open_device()));
    QObject::connect(ui->push_closedevice,SIGNAL(clicked()),this,SLOT(close_device()));
    QObject::connect(ui->push_read,SIGNAL(clicked()),this,SLOT(read_from_device()));
    QObject::connect(ui->push_write,SIGNAL(clicked()),this,SLOT(write_to_device()));
    QObject::connect(ui->push_delete,SIGNAL(clicked()),this,SLOT(delete_device()));
    QObject::connect(ui->push_encrypt,SIGNAL(clicked()),this,SLOT(encrypt()));
    QObject::connect(ui->push_decrypt,SIGNAL(clicked()),this,SLOT(decrypt()));


    ui->device_info->clear();
    ui->push_closedevice->setEnabled(false);
    ui->push_write->setEnabled(false);
    ui->push_read->setEnabled(false);
    ui->push_decrypt->setEnabled(false);
    ui->push_encrypt->setEnabled(false);


}

manager::~manager()
{
    delete ui;
}


void manager::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);//avoid warning
    qDebug()<<"close manager window";
    emit close_device();
    emit manager_exit();
}


void manager::open_device()
{
    qDebug()<<"open device";
    if(file->isOpen())
    {
        QMessageBox::warning(this,"OpenDevice Error!","Device has opened!",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    if(!file->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,"OpenDevice Error!","OpenDevice Error!",QMessageBox::Ok,QMessageBox::Ok);
    }
    ui->push_read->setEnabled(true);
    ui->push_write->setEnabled(true);
    ui->push_decrypt->setEnabled(true);
    ui->push_encrypt->setEnabled(true);
    ui->push_closedevice->setEnabled(true);
}

void manager::close_device()
{
    if(!file->isOpen())
    {
        return ;
    }
    file->close();
    ui->device_info->clear();
    ui->push_closedevice->setEnabled(false);
    ui->push_write->setEnabled(false);
    ui->push_read->setEnabled(false);
    ui->push_decrypt->setEnabled(false);
    ui->push_encrypt->setEnabled(false);
    qDebug()<<"close device";
}

void manager::read_from_device()
{
    if(!file->isOpen())
    {
        file->open(QIODevice::ReadWrite);
    }
    ui->device_info->clear();
    file->seek(0);
    QTextStream textstream(file);
    QString read_data = textstream.readAll();
    if(read_data.isEmpty())
    {
        QMessageBox::warning(nullptr,"Read error","Read nothing!",QMessageBox::Ok,QMessageBox::Ok);
        return ;
    }
    ui->device_info->setText(read_data);
}

void manager::write_to_device()
{
    if(!file->isOpen())
    {
        file->open(QIODevice::ReadWrite);
    }
    QTextStream textstream(file);
    textstream<<ui->device_info->toPlainText();
    textstream.flush();
    ui->device_info->clear();
}

void manager::delete_device()
{
    if(file->exists())
    {
        file->remove();
        ui->device_info->clear();
        ui->push_closedevice->setEnabled(false);
        ui->push_write->setEnabled(false);
        ui->push_read->setEnabled(false);
        ui->push_decrypt->setEnabled(false);
        ui->push_encrypt->setEnabled(false);
    }
    else {
        return ;
    }
}


void manager::encrypt()
{
    if(!file->isOpen())
    {
        file->open(QIODevice::ReadWrite);
    }
    QTextStream enc_data(file);
    file->seek(0);
    QByteArray bytes;
    QString read_data = enc_data.readAll();
    bytes = read_data.toUtf8();
    int size = bytes.size();
    qDebug()<<"encrypt size = "<<size;
    for(int i = 0;i<size;i++)
    {
        bytes[i] = bytes[i] ^ 0x55;
    }
    file->seek(0);
    enc_data<<bytes;
    enc_data.flush();
}

void manager::decrypt()
{
    if(!file->isOpen())
    {
        file->open(QIODevice::ReadWrite);
    }
    QTextStream dec_data(file);
    file->seek(0);
    QByteArray bytes;
    bytes = dec_data.readAll().toUtf8();
    int size = bytes.size();
    qDebug()<<"decrypt size = "<<size;
    for(int i = 0;i<size;i++)
    {
        bytes[i] = bytes[i] ^ 0x55;
    }
    file->seek(0);
    dec_data<<bytes;
    dec_data.flush();
}
