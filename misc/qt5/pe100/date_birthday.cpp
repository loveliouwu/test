#include "date_birthday.h"
#include "ui_date_birthday.h"

date_birthday::date_birthday(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::date_birthday)
{
    ui->setupUi(this);


    //ui->list_view->setModel();


}

date_birthday::~date_birthday()
{
    delete ui;
}
