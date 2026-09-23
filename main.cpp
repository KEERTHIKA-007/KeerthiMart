#include <drogon/drogon.h>
#include "model/User.h"

int main()
{
    drogon::app().loadConfigFile("config/database.json");

    // Registration API
    drogon::app().registerHandler(
        "/api/register",
        [](const drogon::HttpRequestPtr& req,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback)
        {
            auto json = req->getJsonObject();

            if (!json)
            {
                Json::Value result;
                result["message"] = "Invalid JSON";

                auto response =
                    drogon::HttpResponse::newHttpJsonResponse(result);

                response->setStatusCode(drogon::k400BadRequest);
                callback(response);
                return;
            }

            std::string name = (*json)["name"].asString();
            std::string email = (*json)["email"].asString();
            std::string password = (*json)["password"].asString();
            std::string role = (*json)["role"].asString();

            auto dbClient =
                drogon::app().getDbClient();

            dbClient->execSqlAsync(
                "INSERT INTO users "
                "(name, email, password_hash, role) "
                "VALUES ($1, $2, $3, $4)",
                
                [callback](const drogon::orm::Result& result)
                {
                    Json::Value jsonResult;
                    jsonResult["message"] =
                        "Registration successful";

                    auto response =
                        drogon::HttpResponse::newHttpJsonResponse(
                            jsonResult);

                    callback(response);
                },

                [callback](const drogon::orm::DrogonDbException& e)
                {
                    Json::Value jsonResult;
                    jsonResult["message"] =
                        "Registration failed";

                    auto response =
                        drogon::HttpResponse::newHttpJsonResponse(
                            jsonResult);

                    response->setStatusCode(
                        drogon::k500InternalServerError);

                    callback(response);
                },

                name,
                email,
                password,
                role
            );
        },
        {drogon::Post});

    // Serve frontend
    drogon::app().registerHandler(
        "/",
        [](const drogon::HttpRequestPtr&,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback)
        {
            auto response =
                drogon::HttpResponse::newFileResponse(
                    "frontend/index.html");

            callback(response);
        });

    // Start server
    drogon::app()
        .addListener("127.0.0.1", 8080)
        .setThreadNum(4)
        .run();

    return 0;
}