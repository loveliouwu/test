#include<QHttpServer>

class MyHttpServer{

public:
    QHttpServer server;
    int get_count = 0;

    explicit MyHttpServer();

    QString local_func(QString in);
};
