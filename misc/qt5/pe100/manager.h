#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QString>
#include <QFile>
#include <QMessageBox>
namespace Ui {
class manager;
}

class manager : public QMainWindow
{
    Q_OBJECT

public:
    explicit manager(QWidget *parent = nullptr);
    ~manager();


protected:
    void closeEvent(QCloseEvent *event);//when close the window

private:
    Ui::manager *ui;
    QString file_name;
    QFile *file;


signals:
    void manager_exit();

private slots:
    void open_device();
    void close_device();
    void read_from_device();
    void write_to_device();
    void delete_device();
    void encrypt();
    void decrypt();
};

#endif // MANAGER_H
