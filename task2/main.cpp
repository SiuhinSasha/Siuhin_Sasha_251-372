#include <QCoreApplication>
#include <QDebug>
#include "votingserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    VotingServer server;
    if (!server.start(33333)) {
        return 1;
    }

    qInfo() << "Voting server is running on port 33333";
    qInfo() << "Max clients: 7";
    qInfo() << "Use telnet/nc to connect";

    return app.exec();
}
