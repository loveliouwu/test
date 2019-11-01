#ifndef DATE_BIRTHDAY_H
#define DATE_BIRTHDAY_H

#include <QMainWindow>

namespace Ui {
class date_birthday;
}

class date_birthday : public QMainWindow
{
    Q_OBJECT

public:
    explicit date_birthday(QWidget *parent = nullptr);
    ~date_birthday();

private:
    Ui::date_birthday *ui;
};

#endif // DATE_BIRTHDAY_H
