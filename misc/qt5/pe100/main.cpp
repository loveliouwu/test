#include "pe100.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pe100 w;

    QObject::connect(&w,SIGNAL(press_exit_slgnal()),&a,SLOT(quit()));


    w.show();

    return a.exec();
}
