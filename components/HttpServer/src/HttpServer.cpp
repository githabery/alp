#include "HttpServer.hpp"
#include "IAudioManager.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTcpServer>

HttpServer HttpServer::sHttpServer;

IHttpServer& IHttpServer::create () {
    return HttpServer::sHttpServer;
}

void HttpServer::setupRoutes()
{
    // GET /channels
    m_server.route("/channels", []() {
        auto& manager = IAudioManager::create();
        const auto& channels = manager.getChannelsInfo();

        QJsonArray arr;
        for (const auto& [id, info] : channels) {
            QJsonObject obj;
            obj["id"] = id;
            obj["name"] = info.name;
            obj["active"] = info.active;

            QJsonArray funcsArray;
            for (const auto& func : info.functions)
                funcsArray.append(manager.functionTypeToString(func));
            obj["functions"] = funcsArray;

            arr.append(obj);
        }

        return QHttpServerResponse("application/json", QJsonDocument(arr).toJson());
    });

    m_server.route("/functions", []() {
        auto& manager = IAudioManager::create();
        const auto& functions = manager.getFunctionTypes();

        QJsonArray arr;
        for (const auto& function : functions) {
            QJsonObject obj;
            obj["name"] = function;

            arr.append(obj);
        }
        return QHttpServerResponse("application/json", QJsonDocument(arr).toJson());
    });

    m_server.route("/channels/add", QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest &request) {
            const auto query = request.query();
            if (!query.hasQueryItem("id"))
                return QHttpServerResponse("application/json", R"({"error": "Missing 'id'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);

            bool ok = false;
            int id = query.queryItemValue("id").toInt(&ok);
            if (!ok)
                return QHttpServerResponse("application/json", R"({"error": "Invalid 'id'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            IAudioManager& manager = IAudioManager::create();
            manager.addChannel(id);
            manager.saveSettings();

            return QHttpServerResponse("application/json", R"({"status": "added"})",
                                       QHttpServerResponse::StatusCode::Ok);
    });

    // POST /channels/remove
    m_server.route("/channels/remove", QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest &request) {
            const auto queryParams = request.query();

            // Перевіряємо наявність параметра id
            if (!queryParams.hasQueryItem("id")) {
                return QHttpServerResponse("application/json", R"({"error":"Missing 'id'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            // Перетворимо id на int
            bool ok = false;
            int id = queryParams.queryItemValue("id").toInt(&ok);
            if (!ok) {
                return QHttpServerResponse("application/json", R"({"error":"Invalid 'id'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            // Видаляємо канал id
            IAudioManager& manager = IAudioManager::create();
            manager.removeChannel(id);
            manager.saveSettings();

            return QHttpServerResponse("application/json", R"({"status":"removed"})");
    });

    // POST /channels/add-func
    m_server.route("/channels/add-func", QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest &request) {
            const auto queryParams = request.query();
            if (!queryParams.hasQueryItem("id")) {
                return QHttpServerResponse("application/json", R"({"error":"Missing 'id'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }
            else if (!queryParams.hasQueryItem("func")) {
                return QHttpServerResponse("application/json", R"({"error":"Missing 'function'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }
            bool ok = false;
            int id = queryParams.queryItemValue("id").toInt(&ok);
            if (!ok) {
                return QHttpServerResponse("application/json", R"({"error":"Invalid 'id'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }
            IAudioManager& manager = IAudioManager::create();
            QString func = queryParams.queryItemValue("func");

            if (!manager.addFunctionToChannel(id, manager.functionTypeFromString(func))) {
                return QHttpServerResponse("application/json", R"({"error":"Invalid 'function'"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }
            manager.saveSettings();

            return QHttpServerResponse("application/json", R"({"status":"added"})");

    });

    // POST /channels/remove-func
    m_server.route("/channels/remove-func", QHttpServerRequest::Method::Post,
        [](const QHttpServerRequest &request) {
            const auto queryParams = request.query();
            if (!queryParams.hasQueryItem("id")) {
                return QHttpServerResponse("application/json", R"({"error":"Missing 'id'"})",
                                            QHttpServerResponse::StatusCode::BadRequest);
            }
            else if (!queryParams.hasQueryItem("func")) {
                return QHttpServerResponse("application/json", R"({"error":"Missing 'function'"})",
                                            QHttpServerResponse::StatusCode::BadRequest);
            }
            bool ok = false;
            int id = queryParams.queryItemValue("id").toInt(&ok);
            if (!ok) {
                return QHttpServerResponse("application/json", R"({"error":"Invalid 'id'"})",
                                            QHttpServerResponse::StatusCode::BadRequest);
            }
            IAudioManager& manager = IAudioManager::create();
            QString func = queryParams.queryItemValue("func");

            if (!manager.removeFunctionFromChannel(id, manager.functionTypeFromString(func))) {
                return QHttpServerResponse("application/json", R"({"error":"Invalid 'function'"})",
                                            QHttpServerResponse::StatusCode::BadRequest);
            }
            manager.saveSettings();

            return QHttpServerResponse("application/json", R"({"status":"removed"})");

    });


    // Додамо заголовки після кожного запиту
    m_server.addAfterRequestHandler(&m_server, [](const QHttpServerRequest &, QHttpServerResponse &resp) {
        auto headers = resp.headers();
        headers.append(QHttpHeaders::WellKnownHeader::Server, "QtHttpServer");
        headers.append("Access-Control-Allow-Origin", "*");
        resp.setHeaders(std::move(headers));
    });
}

bool HttpServer::start(quint16 port)
{
    auto tcpserver = std::make_unique<QTcpServer>();
    if (!tcpserver->listen(QHostAddress::Any, port) || !m_server.bind(tcpserver.get())) {
        qWarning() << "Failed to listen on port";
        return false;
    }

    m_port = tcpserver->serverPort();
    tcpserver.release();
    return true;
}

quint16 HttpServer::port() const
{
    return m_port;
}


