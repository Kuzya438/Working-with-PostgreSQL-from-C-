#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>

class ClientManager {
private:
    pqxx::connection conn_;

public:
    struct ClientInfo {
        int id;
        std::string firstName;
        std::string lastName;
        std::string email;
        std::vector<std::string> phones;
    };
    explicit ClientManager(const std::string& connStr);
    void createTables();
    int addClient(const std::string& firstName, const std::string& lastName, const std::string& email);
    void addPhone(int clientId, const std::string& phone);
    void updateClient(int clientId, const std::string& newFirstName, const std::string& newLastName, const std::string& newEmail);
    void deletePhone(int clientId, const std::string& phone);
    void deleteClient(int clientId);
    std::vector<ClientInfo> findClient(const std::string& query);
    static void printClient(const ClientInfo& c);
};
