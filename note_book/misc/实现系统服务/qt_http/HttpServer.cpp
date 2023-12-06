#include<QHttpServer>
#include<QHttpServerResponse>
#include"HttpServer.h"
#include<QJsonObject>



MyHttpServer::MyHttpServer()
{
    server.route("/", [](){ return "page test"; });
    server.route("/test", [](){ return "page /test"; });
    // server.route("/test/<arg>", [](QString arg){ return "page /test/" + arg; });

    //默认是处理所有支持的请求方式，如果要指定请求方式，则
    server.route("/get", QHttpServerRequest::Method::Get, [](){
        return QString("/get");
    });
    //返回本地（类中）参数
    server.route("/get/count", QHttpServerRequest::Method::Get, [this](){
        return QString::number(this->get_count++);
    });
    //POST 未测试
    server.route("/post", QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest &request) {
            qDebug() << "post query" << request.query().toString();
            qDebug() << "post body" << request.body();
            return QJsonObject{
             {
                 {"method", "post"}
             }
        };
    });
    //测试接收数据
    server.route("/data/<arg>", [](QString data){ return "/data/<arg> " + data; });
    server.route("/data/<arg>/<arg>", [](QString data, QString data2){ return "/data/<arg>/<arg> " + data + "  " + data2; });
    //测试返回数据
    server.route("/response" , [](){
        QHttpServerResponse response("Hello world");
        response.setHeader("Access-Control-Allow-Methods", "POST,GET,PUT,DELETE");
        response.setHeader("IV", "112233445566778899");
        qDebug(response.data());
        return response;
    });
    server.route("/json", [](){
        return QJsonObject{
            {
                {"key1", "11111111"},
                {"key2", "22222222"}
            }
        };
    });

    //调用本地函数
    server.route("/call/local/<arg>", [this](QString in){
        return this->local_func(in);
    });



    server.listen( QHostAddress::Any, 8848);
    printf("server start\n");
}

QString MyHttpServer::local_func(QString in)
{
    QString ret = "[Call Local]: " + in;
    qDebug()<<ret;
    return ret;
}
