#include "Client.h"
#include <Windows.h>
#include <iostream>
#include <string>

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        std::string connStr = "host=localhost port=5432 dbname=Client user=postgres password=????";

        ClientManager mgr(connStr);
        mgr.createTables();

        int ivanId = mgr.addClient("Иван", "Кузнецов", "ivankuznetsov@mail.ru");
        int mariyId = mgr.addClient("Мария", "Мельникова", "mariymelinikova@yandex.ru");
        int nikitaId = mgr.addClient("Никита", "Кандалинцев", "nikitakandalinchev@gmail.com");

        mgr.addPhone(ivanId, "+79530518857");
        mgr.addPhone(mariyId, "+79526566558");
        mgr.addPhone(mariyId, "+79556556555");

        std::cout << "\nПоиск по имени 'Иван'\n";
        auto found = mgr.findClient("Иван");
        for (const auto& c: found) { ClientManager::printClient(c); }

        std::cout << "\nПоиск по телефону '555'\n";
        found = mgr.findClient("444");
        for (const auto& c: found) { ClientManager::printClient(c); }

        std::cout << "\nПоиск по email 'nikitakandalinchev@gmail.com'\n";
        found = mgr.findClient("nikitakandalinchev@gmail.com");
        for (const auto& c: found) { ClientManager::printClient(c); }

        mgr.updateClient(ivanId, "Иван", "", "ivankuznetsov@yandex.ru");

        std::cout << "\nПосле обновления\n";
        found = mgr.findClient("Иван");
        for (const auto& c: found) { ClientManager::printClient(c); }

        mgr.deletePhone(mariyId, "+79526577558");

        mgr.deleteClient(nikitaId);

        std::cout << "\nПосле удаления Никита\n";
        found = mgr.findClient("Никита");
        if (found.empty()) { std::cout << "Никита не найден — удаление сработало." << std::endl; }
        else { for (const auto& c : found) ClientManager::printClient(c); }
    }
    catch (pqxx::sql_error e) { std::cout << e.what() << std::endl; }
    return EXIT_SUCCESS;
}