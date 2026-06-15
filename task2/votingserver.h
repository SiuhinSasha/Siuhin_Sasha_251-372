#ifndef VOTINGSERVER_H
#define VOTINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include <QTimer>

class VotingServer : public QObject
{
    Q_OBJECT

public:
    explicit VotingServer(QObject *parent = nullptr);
    ~VotingServer();

    bool start(quint16 port = 33333);
    void stop();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void showResults();

private:
    void broadcast(const QString &message, QTcpSocket *exclude = nullptr);
    void broadcastCount();
    void processVote(QTcpSocket *socket, const QString &option);
    void startVoting();
    void checkAllVoted();

    QTcpServer *m_server;
    QList<QTcpSocket *> m_clients;
    QMap<QTcpSocket *, QString> m_votes;
    int m_maxClients;
    QString m_currentQuestion;
    QStringList m_voteOptions;
    bool m_votingInProgress;
};

#endif // VOTINGSERVER_H
