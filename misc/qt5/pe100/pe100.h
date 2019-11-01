#ifndef PE100_H
#define PE100_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtCore/QCoreApplication>
#include <QVariant>
#include "manager.h"
#include <QDesktopWidget>
#include <QTimer>
#include <QDateTime>
namespace Ui {
class pe100;
}

class pe100 : public QMainWindow
{
    Q_OBJECT

public:
    explicit pe100(QWidget *parent = nullptr);
    ~pe100();

    void login();
    manager manager_ui;
private:
    Ui::pe100 *ui;
    QVariant user_chose;
    qint8 login_status;
    QString manager_pwd,operator_pwd;
    QString time;
    QTimer *timer;

//    QMainWindow manager_window;

signals:
    void press_exit_slgnal();

private slots:
    void login_by_password();
    void exit_press();
    void user_exit();
    void time_update();
    void help();
};



#endif // PE100_H
