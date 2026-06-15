#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class EchoServer : public QObject
{
    Q_OBJECT

public:
    explicit EchoServer(QObject *parent = nullptr);
    ~EchoServer();

    bool start(quint16 port = 65432);
    void stop();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer *m_server;
    QList<QTcpSocket *> m_clients;
};

#endif // ECHOSERVER_H
