#include "echoserver.h"
#include <QDebug>

EchoServer::EchoServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
}

EchoServer::~EchoServer()
{
    stop();
}

bool EchoServer::start(quint16 port)
{
    if (!m_server->listen(QHostAddress::Any, port)) {
        qCritical() << "Server failed to start:" << m_server->errorString();
        return false;
    }
    connect(m_server, &QTcpServer::newConnection, this, &EchoServer::onNewConnection);
    qInfo() << "Echo server started on port" << port;
    return true;
}

void EchoServer::stop()
{
    for (auto *client : m_clients) {
        client->close();
        client->deleteLater();
    }
    m_clients.clear();
    m_server->close();
}

void EchoServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *client = m_server->nextPendingConnection();
        m_clients.append(client);

        qInfo() << "New client connected:" << client->peerAddress().toString();

        connect(client, &QTcpSocket::readyRead, this, &EchoServer::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &EchoServer::onClientDisconnected);
    }
}

void EchoServer::onReadyRead()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    qInfo() << "Received:" << data;
    socket->write(data);
    socket->flush();
}

void EchoServer::onClientDisconnected()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    qInfo() << "Client disconnected:" << socket->peerAddress().toString();
    m_clients.removeOne(socket);
    socket->deleteLater();
}
