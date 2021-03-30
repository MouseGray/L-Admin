#include "server.h"

#include <QTimer>

#ifdef VERSION_1

Server::Server(QObject *parent) :
    QTcpServer(parent),
    m_connectionsCount(0),
    m_clients(10, nullptr)
{
}

size_t Server::connectionCount() const
{
    return m_connectionsCount;
}

void Server::sendData(int userid, const QString &title, const QJsonValue &data)
{
    sendData(getSocket(userid), title, data);
}

void Server::sendData(QTcpSocket *socket, const QString &title, const QJsonValue &data)
{
    QJsonObject json;
    json.insert("title", title);
    json.insert("data", data);

    socket->write(QJsonDocument(json).toJson());
    socket->flush();
}

void Server::sendStatusData(QTcpSocket *socket, const QString &status, int code, const QString &text)
{
    QJsonObject json;
    json.insert("title", "status");
    json.insert("status", status);
    json.insert("code", code);
    json.insert("text", text);

    socket->write(QJsonDocument(json).toJson());
    socket->flush();
}

void Server::waitingTimeout(QTcpSocket *visitor)
{
    auto visitorIt = m_waiting.find(visitor);
    if (visitorIt == m_waiting.end()) return;
    m_waiting.erase(visitorIt);
    m_connectionsCount--;
    sendStatusData(visitor, "error", 4, "Waiting time out.");
    visitor->close();
    visitor->deleteLater();
}

void Server::readData_slot()
{
    auto socket = qobject_cast<QTcpSocket*>(sender());
    assert(socket != nullptr);

    auto data = socket->readAll();

    auto doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    auto obj = doc.object();

    try {
        auto title = ljson::jsonToValue<QString>(obj["title"]);

        if (title == "status") {
            auto status = ljson::jsonToValue<QString>(obj["status"]);
            if(status == "ok") {

            }
            if(status == "error") {
                switch (ljson::jsonToValue<int>(obj["code"])) {
                    case -1: {
                        auto text = ljson::jsonToValue<QString>(obj["text"]);
                        emit error(text);
                        break;
                    }
                    case 1: {
                        auto text = ljson::jsonToValue<QString>(obj["text"]);
                        emit error(text);
                        break;
                    }
                }
            }
            return;
        }

        if(title == "place") {
            auto id = ljson::jsonToValue<int>(obj["data"]);

            if (id < 0 || id >= static_cast<int>(m_clients.size())) {
                sendStatusData(socket, "error", 1, "Incorrect place.");
                return;
            }

            auto visitorIt = m_waiting.find(socket);
            if (visitorIt == m_waiting.end()) return;
            m_waiting.erase(visitorIt);

            if (m_clients[id] == nullptr) {
                m_clients[id] = socket;
                sendData(socket, "accept", "Connection established.");
                StateChanged(id, WorkplaceState::ConnectedNN);
                return;
            }
            sendData(socket, "reject", "The requested id is busy.");

            socket->close();
            socket->deleteLater();
            m_connectionsCount--;
            return;
        }
        if(title == "start"){
            auto id = getId(socket);
            if (is_not_correct_id(id)) return;

            StateChanged(id, WorkplaceState::Working);
            return;
        }
        if(title == "accept"){
            auto id = getId(socket);
            if (is_not_correct_id(id)) return;

            StateChanged(id, WorkplaceState::Waiting);
            return;
        }
        if(title == "complete"){
            auto id = getId(socket);
            if (is_not_correct_id(id)) return;

            auto data = ljson::jsonToValue<QJsonObject>(obj["data"]);
            NewData(id, data);
            StateChanged(id, WorkplaceState::Checking);
            return;
        }
        if(title == "work") {
            emit workUpdated(ljson::jsonToValue<QJsonObject>(obj["data"]));
            return;
        }
        sendStatusData(socket, "error", -1, "Unknown command.");
    }  catch (std::invalid_argument& ex) {
        sendStatusData(socket, "error", 1, ex.what());
        qDebug() << ex.what();
    }

}

void Server::clientDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    auto visitorIt = m_waiting.find(client);
    if (visitorIt != m_waiting.end()) {
        m_waiting.erase(visitorIt);
        client->deleteLater();
        return;
    }
    auto clientIt = std::find(m_clients.begin(), m_clients.end(), client);
    if (clientIt != m_clients.end()) {
        auto id = std::distance(m_clients.begin(), clientIt);
        StateChanged(id, WorkplaceState::Disconnected);
        m_clients[id] = nullptr;
        client->deleteLater();
        return;
    }
}

void Server::clientError(QAbstractSocket::SocketError err)
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    auto visitorIt = m_waiting.find(client);
    if (visitorIt != m_waiting.end()) {
        m_waiting.erase(visitorIt);
        client->deleteLater();
        qDebug() << "Visitor error: " << err;
        return;
    }
    auto clientIt = std::find(m_clients.begin(), m_clients.end(), client);
    if (clientIt != m_clients.end()) {
        auto id = std::distance(m_clients.begin(), clientIt);
        StateChanged(id, WorkplaceState::Disconnected);
        m_clients[id] = nullptr;
        client->deleteLater();
        qDebug() << "Client[" << id << "] error: " << err;
        return;
    }
}

void Server::incomingConnection(qintptr handle)
{
    try {
        auto client = new QTcpSocket(this);
        if (!client->setSocketDescriptor(handle)) {
            qDebug() << "[" << client->error() << "] when trying to connect a new client.";
            client->deleteLater();
            return;
        }
        if (m_connectionsCount == m_clients.size()) {
            sendStatusData(client, "error", 6, "Connection limit exceeded");
            client->close();
            client->deleteLater();
            return;
        }

        QTimer timer;
        auto wto = std::bind(&Server::waitingTimeout, this, client);
        timer.singleShot(waitingTime, wto);

        try {
            m_waiting.insert(client);
        }  catch (std::bad_alloc& ex) {
            qDebug() << "[" << ex.what() << "] when trying to put on queue.";
            client->close();
            client->deleteLater();
            return;
        }
        connect(client, &QTcpSocket::readyRead, this, &Server::readData_slot);
        connect(client, &QTcpSocket::disconnected, this, &Server::clientDisconnected);
        connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientError(QAbstractSocket::SocketError)));
        m_connectionsCount++;
    }
    catch (std::bad_alloc& ex) {
        qDebug() << "[" << ex.what() << "] when trying to connect a new client.";
    }
}

QTcpSocket *Server::getSocket(int userid)
{
    assert(userid > 0);
    assert(userid < static_cast<int>(m_clients.size()));
    return m_clients[userid];
}

int Server::getId(const QTcpSocket *socket)
{
    assert(socket != nullptr);

    auto clientIt = std::find(m_clients.begin(), m_clients.end(), socket);
    if (clientIt == m_clients.end()) return -1;

    return std::distance(m_clients.begin(), clientIt);
}



#else
Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(Connected()));
}

void Server::Start(int port)
{
    server->listen(QHostAddress::AnyIPv4, port);
}

void Server::Stop()
{
    server->close();
}

void Server::AskID(int uID)
{
    send(uID, "getID", "null");
}

void Server::RejectID(int uID)
{
    send(uID, "NCON", "null");
}

void Server::AcceptID(int uID)
{
    send(uID, "CON", "null");
}

void Server::Read(int uID, QByteArray& bytes)
{
    auto doc = QJsonDocument::fromJson(bytes);
    auto obj = doc.object();

    auto title = obj["title"];
    if(title == QJsonValue::Undefined) return;

    if(title == "getID"){
        auto data = obj["data"];
        if(data == QJsonValue::Undefined) return;

        int id = data.toInt(67);
        qDebug() << id<< "id";
        if (GetUID(id) == -1){
            clients[uID]->ID = id;
            qDebug() << QString::number(id) + "a";
            StateChanged(id, WorkplaceState::ConnectedNN);
            AcceptID(uID);
        }
        else {
            qDebug() << QString::number(id);
            RejectID(uID);
            clients[uID]->socket->close();
            clients.erase(clients.find(uID));
        }
        return;
    }
    if(title == "TSK"){
        auto data = obj["data"];
        if(data == QJsonValue::Undefined) return;
        if(data == "start") StateChanged(clients[uID]->ID, WorkplaceState::Working);
        return;
    }
    if(title == "CTSK"){
        auto data = obj["data"];
        if(data == QJsonValue::Undefined) return;
        NewData(clients[uID]->ID, data.toObject());
        StateChanged(clients[uID]->ID, WorkplaceState::Checking);
        return;
    }
}

void Server::ReadyRead()
{
    QByteArray data;
    for(auto it = clients.begin(); it != clients.end(); it++){
        data = it.value()->socket->readAll();
        if(data.isEmpty()) continue;
        qDebug() << it.key()<< QString(data);
        Read(it.key(), data);
    }
}

int Server::GetUID(int ID)
{
    for(QMap<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++){
        if (it.value()->ID == ID) return it.key();
    }
    qDebug() << "-1";
    return -1;
}

void Server::SendTask(int ID, QByteArray &bytes)
{
    auto doc = QJsonDocument::fromJson(bytes);
    send(GetUID(ID), "TSK", doc.object());
}

int Server::GetConnectionAmount()
{
    return clients.size();
}

void Server::send(int user, const QString id, QJsonValue data)
{
    QJsonObject json;
    json.insert("title", id);
    json.insert("data", data);
    auto doc = QJsonDocument(json);
    qDebug() << "Sended: " << doc.toJson();
    clients[user]->socket->write(doc.toJson());
    clients[user]->socket->flush();
}

void Server::Connected()
{
    Client* client = new Client();
    client->socket = server->nextPendingConnection();
    connect(client->socket, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(client->socket, SIGNAL(readyRead()), this, SLOT(ReadyRead()));

    int uID = client->socket->socketDescriptor();
    clients.insert(uID, client);
    qDebug() << clients.size();
    AskID(uID);
}

void Server::Disconnected()
{
    QTcpSocket* client = static_cast<QTcpSocket*>(sender());
    for(QMap<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++){
        if (it.value()->socket == client){
            StateChanged(it.value()->ID, WorkplaceState::Disconnected);
            clients.erase(it);
            break;
        }
    }
}
#endif
