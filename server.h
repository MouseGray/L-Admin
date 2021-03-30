#ifndef SERVER_H
#define SERVER_H

#define VERSION_1

#include <set>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QMap>

#include <QJsonObject>
#include <QJsonDocument>

#include "workplace.h"
#include "l_json.h"

struct Client
{
    QTcpSocket* socket = nullptr;
    int ID = 0;
};

// TODO: Проверка допустимости ID

#define is_correct_id(id) (id != -1)
#define is_not_correct_id(id) (id == -1)

#ifdef VERSION_1
class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    inline size_t connectionCount() const;

    void sendData(int userid, const QString& title, const QJsonValue& data);
    void sendData(QTcpSocket* socket, const QString& title, const QJsonValue& data);
    void sendStatusData(QTcpSocket* socket, const QString& status, int code, const QString& text = "");

    void waitingTimeout(QTcpSocket* visitor);
signals:
    void StateChanged(int ID, WorkplaceState);
    void NewData(int ID, QJsonObject json);

    void error(const QString& text);
    void workUpdated(const QJsonObject& json);
public slots:
    void readData_slot();

    void clientDisconnected();
    void clientError(QAbstractSocket::SocketError err);

    // QTcpServer interface
protected:
    void incomingConnection(qintptr handle) override;

private:
    QTcpSocket* getSocket(int userid);
    int getId(const QTcpSocket* socket);

    size_t m_connectionsCount;
    std::set<QTcpSocket*> m_waiting;
    std::vector<QTcpSocket*> m_clients;

    static const int waitingTime = 3000;
};
#else
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void Start(int port);
    void Stop();
    void AskID(int uID);
    void RejectID(int uID);
    void AcceptID(int uID);
    void SendTask(int ID, QByteArray& bytes);
    inline int GetConnectionAmount();

    void send(int user, const QString id, QJsonValue data);
signals:
    void StateChanged(int ID, WorkplaceState);
    void NewData(int ID, QJsonObject json);
public slots:
    void Connected();
    void Disconnected();
    void ReadyRead();
    //void Error(QAbstractSocket::SocketError);
private:
    int GetUID(int ID);
    void Read(int uID, QByteArray& bytes);

    QTcpServer* server = nullptr;
    QMap<int, Client*> clients;
};
#endif

#endif // SERVER_H
