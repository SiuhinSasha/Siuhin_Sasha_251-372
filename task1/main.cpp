#include <QCoreApplication>
#include <QDebug>
#include "echoserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    EchoServer server;
    if (!server.start(65432)) {
        return 1;
    }

    return app.exec();
}
