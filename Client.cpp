#include "Client.h"
#include <iostream>
#include <optional>

ClientManager::ClientManager(const std::string& connStr) : conn_(connStr) {}

void ClientManager::createTables() {
    pqxx::work tx{ conn_ };
    tx.exec(
        "CREATE TABLE IF NOT EXISTS clients ("
        "id SERIAL PRIMARY KEY, "
        "first_name VARCHAR(40) NOT NULL, "
        "last_name VARCHAR(40) NOT NULL, "
        "email TEXT UNIQUE NOT NULL);"
    );
    tx.exec(
        "CREATE TABLE IF NOT EXISTS phones ("
        "id SERIAL PRIMARY KEY, "
        "client_id INTEGER NOT NULL REFERENCES clients(id) ON DELETE CASCADE, "
        "phone VARCHAR(20) NOT NULL);"
    );
    tx.exec("CREATE INDEX IF NOT EXISTS idx_phones_client_id ON phones(client_id);");
    tx.exec("CREATE INDEX IF NOT EXISTS idx_phones_phone ON phones(phone);");
    tx.commit();
    std::cout << "[OK] Таблицы clients и phones созданы (или уже существовали)." << std::endl;
}

int ClientManager::addClient(const std::string& firstName, const std::string& lastName, const std::string& email) {
    pqxx::work tx{ conn_ };
    pqxx::result r = tx.exec(
        "INSERT INTO clients (first_name, last_name, email) "
        "VALUES ($1, $2, $3) RETURNING id", { firstName, lastName, email }
    );
    int id = r[0][0].as<int>();
    tx.commit();
    std::cout << "[OK] Добавлен клиент id=" << id << std::endl;
    return id;
}

void ClientManager::addPhone(int clientId, const std::string& phone) {
    pqxx::work tx{ conn_ };
    tx.exec("INSERT INTO phones (client_id, phone) VALUES ($1, $2)", { clientId, phone });
    tx.commit();
    std::cout << "[OK] Телефон " << phone << " добавлен клиенту id=" << clientId << std::endl;
}

void ClientManager::updateClient(int clientId, const std::string& newFirstName, const std::string& newLastName, const std::string& newEmail) {
    pqxx::work tx{ conn_ };
    if (!newFirstName.empty()) { tx.exec("UPDATE clients SET first_name=$1 WHERE id=$2", { newFirstName, clientId }); }
    if (!newLastName.empty()) { tx.exec("UPDATE clients SET last_name=$1 WHERE id=$2", { newLastName, clientId }); }
    if (!newEmail.empty()) { tx.exec("UPDATE clients SET email=$1 WHERE id=$2", { newEmail, clientId }); }
    tx.commit();
    std::cout << "[OK] Данные клиента id=" << clientId << " обновлены." << std::endl;
}

void ClientManager::deletePhone(int clientId, const std::string& phone) {
    pqxx::work tx{ conn_ };
    pqxx::result r = tx.exec("DELETE FROM phones WHERE client_id=$1 AND phone=$2", { clientId, phone });
    tx.commit();
    std::cout << "[OK] Удалено телефонов: " << r.affected_rows() << std::endl;
}

void ClientManager::deleteClient(int clientId) {
    pqxx::work tx{ conn_ };
    pqxx::result r = tx.exec("DELETE FROM clients WHERE id=$1", { clientId });
    tx.commit();
    std::cout << "[OK] Удалено клиентов: " << r.affected_rows() << std::endl;
}

std::vector<ClientManager::ClientInfo> ClientManager::findClient(const std::string& query) {
    pqxx::work tx{ conn_ };
    pqxx::result rows = tx.exec(
        "SELECT c.id, c.first_name, c.last_name, c.email, "
        "COALESCE(p.phone, '') AS phone "
        "FROM clients c "
        "LEFT JOIN phones p ON p.client_id = c.id "
        "WHERE c.first_name ILIKE $1 "
        "OR c.last_name ILIKE $1 "
        "OR c.email ILIKE $1 "
        "OR p.phone ILIKE $1 "
        "ORDER BY c.id, p.id;",
        pqxx::params{ "%" + query + "%" }
    );
    tx.commit();

    std::vector<ClientInfo> result;
    int currentId = -1;
    for (const auto& row : rows) {
        int id = row["id"].as<int>();
        if (id != currentId) {
            ClientInfo ci;
            ci.id = id;
            ci.firstName = row["first_name"].as<std::string>();
            ci.lastName = row["last_name"].as<std::string>();
            ci.email = row["email"].as<std::string>();
            result.push_back(std::move(ci));
            currentId = id;
        }
        std::string phone = row["phone"].as<std::string>();
        if (!phone.empty()) { result.back().phones.push_back(phone); }
    }
    return result;
}

void ClientManager::printClient(const ClientInfo& c) {
    std::cout << "id=" << c.id << " | " << c.firstName << " " << c.lastName << " | email: " << c.email << std::endl;
    if (c.phones.empty()) { std::cout << "(телефонов нет)" << std::endl; }
    else {
        for (const auto& p : c.phones) {
            std::cout << "тел: " << p << std::endl;
        }
    }
}