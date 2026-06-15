#include "votingserver.h"
#include <QDebug>
#include <QStringList>

VotingServer::VotingServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_maxClients(7)
    , m_votingInProgress(false)
{
}

VotingServer::~VotingServer()
{
    stop();
}

bool VotingServer::start(quint16 port)
{
    if (!m_server->listen(QHostAddress::Any, port)) {
        qCritical() << "Server failed to start:" << m_server->errorString();
        return false;
    }
    connect(m_server, &QTcpServer::newConnection, this, &VotingServer::onNewConnection);
    qInfo() << "Voting server started on port" << port;
    return true;
}

void VotingServer::stop()
{
    for (auto *client : m_clients) {
        client->close();
        client->deleteLater();
    }
    m_clients.clear();
    m_votes.clear();
    m_server->close();
}

void VotingServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *client = m_server->nextPendingConnection();

        if (m_clients.size() >= m_maxClients) {
            client->write("Server is busy. Please try again later.\n");
            client->flush();
            client->disconnectFromHost();
            qInfo() << "Rejected client (busy):" << client->peerAddress().toString();
            return;
        }

        m_clients.append(client);
        m_votes[client] = QString();

        QString welcome = "Welcome to the Voting Server!\n"
                          "Current clients: " + QString::number(m_clients.size()) +
                          "/" + QString::number(m_maxClients) + "\n";
        client->write(welcome.toUtf8());
        client->flush();

        connect(client, &QTcpSocket::readyRead, this, &VotingServer::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &VotingServer::onClientDisconnected);

        qInfo() << "Client connected:" << client->peerAddress().toString()
                << "Total:" << m_clients.size();

        broadcastCount();

        if (m_clients.size() == m_maxClients && !m_votingInProgress) {
            startVoting();
        }
    }
}

void VotingServer::onReadyRead()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll().trimmed();
    QString msg = QString::fromUtf8(data).trimmed();

    qInfo() << "Received from" << socket->peerAddress().toString() << ":" << msg;

    if (msg.startsWith("/vote ", Qt::CaseInsensitive)) {
        QString option = msg.mid(6).trimmed();
        processVote(socket, option);
    } else if (msg.startsWith("/yes", Qt::CaseInsensitive) && !m_votingInProgress) {
        startVoting();
    } else if (msg.startsWith("/no", Qt::CaseInsensitive) && !m_votingInProgress) {
        broadcast("Voting ended. Disconnecting all clients.\n");
        QTimer::singleShot(1000, this, &VotingServer::stop);
    } else {
        socket->write(("Unknown command: " + msg + "\n"
                       "Available commands:\n"
                       "  /vote <option>  - cast your vote\n").toUtf8());
        socket->flush();
    }
}

void VotingServer::onClientDisconnected()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    qInfo() << "Client disconnected:" << socket->peerAddress().toString();
    m_clients.removeOne(socket);
    m_votes.remove(socket);
    socket->deleteLater();

    broadcastCount();

    if (m_votingInProgress && m_clients.size() < m_maxClients) {
        broadcast("Not enough clients for voting. Waiting...\n");
        m_votingInProgress = false;
        m_votes.clear();
    }
}

void VotingServer::broadcast(const QString &message, QTcpSocket *exclude)
{
    for (auto *client : m_clients) {
        if (client != exclude) {
            client->write(message.toUtf8());
            client->flush();
        }
    }
}

void VotingServer::broadcastCount()
{
    QString msg = "Current clients: " + QString::number(m_clients.size()) +
                  "/" + QString::number(m_maxClients) + "\n";
    broadcast(msg);
}

void VotingServer::processVote(QTcpSocket *socket, const QString &option)
{
    if (!m_votingInProgress) {
        socket->write("No voting in progress.\n");
        socket->flush();
        return;
    }

    if (!m_votes[socket].isEmpty()) {
        socket->write("You have already voted.\n");
        socket->flush();
        return;
    }

    m_votes[socket] = option;
    socket->write(("Vote recorded: " + option + "\n").toUtf8());
    socket->flush();

    qInfo() << "Vote from" << socket->peerAddress().toString() << ":" << option;

    checkAllVoted();
}

void VotingServer::startVoting()
{
    m_votingInProgress = true;
    m_votes.clear();
    for (auto *client : m_clients) {
        m_votes[client] = QString();
    }

    m_currentQuestion = "What is your favorite programming language?";
    m_voteOptions << "C++" << "Python" << "Java" << "JavaScript";

    QString msg = "\n===== VOTING STARTED =====\n"
                  "Question: " + m_currentQuestion + "\n"
                  "Options:\n";
    for (int i = 0; i < m_voteOptions.size(); ++i) {
        msg += "  " + QString::number(i + 1) + ". " + m_voteOptions[i] + " (vote: /vote " + m_voteOptions[i] + ")\n";
    }
    msg += "Cast your vote with: /vote <option>\n";

    broadcast(msg);
    QTimer::singleShot(30000, this, &VotingServer::showResults);
}

void VotingServer::checkAllVoted()
{
    for (auto it = m_votes.begin(); it != m_votes.end(); ++it) {
        if (it.value().isEmpty()) return;
    }

    showResults();
}

void VotingServer::showResults()
{
    if (!m_votingInProgress) return;
    m_votingInProgress = false;

    QMap<QString, int> results;
    for (auto it = m_votes.begin(); it != m_votes.end(); ++it) {
        if (!it.value().isEmpty()) {
            results[it.value()]++;
        }
    }

    QString msg = "\n===== VOTING RESULTS =====\n"
                  "Question: " + m_currentQuestion + "\n";
    for (auto it = results.begin(); it != results.end(); ++it) {
        msg += "  " + it.key() + ": " + QString::number(it.value()) + " vote(s)\n";
    }
    msg += "Total votes: " + QString::number(m_clients.size()) + "\n";
    msg += "===========================\n"
           "Continue? (/yes or /no)\n";

    broadcast(msg);

    QTimer::singleShot(15000, this, [this]() {
        if (!m_votingInProgress) {
            broadcast("Timeout. Disconnecting all clients.\n");
            QTimer::singleShot(1000, this, &VotingServer::stop);
        }
    });
}
