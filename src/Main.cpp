#include <iostream>

#include "config/config_parser.h"
#include "postgresql/PostgresqlConnection.h"

using namespace std;

int main(int argc, const char* argv[])
{
    const ConfigParams config = parseConfig(argc, argv);

    auto postgresConnection = make_unique<PostgresqlConnection>(config.postgres);
    // ==== Example insertion ====
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    // see PostgresqlConnection#prepareStatements()
    transaction1->exec_prepared(PostgresqlConnection::STATEMENT_TEST, "billy", "0109050402");
    transaction1->commit();

    // ==== Example iteration (all in-memory retrieving (BAD, but simple)) ====
    auto transaction2 = postgresConnection->createTransaction();
    pqxx::result result(transaction2->exec("SELECT * FROM test;"));
    for (auto row : result)
    {
        std::cout << "Name : " << row["name"] << " Tel : " << row["num_tel"] << "\n";
    }
    transaction2->commit();

    // ==== Example iteration (cursor-based approach) ====
    // ! Warning : not tested, but should work
    auto transaction3 = postgresConnection->createTransaction();
    string query = "SELECT * FROM test;";
    pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned>
        cursor(*(transaction3.get()), query, "myCursor", false);
    size_t idx = 0;
    size_t rowsPerBatch = 10; // We will get 10 rows for each cursor iteration (batch-retrieving)
    pqxx::result res;
    do
    {
        res = cursor.retrieve(idx, idx + rowsPerBatch);
        idx += rowsPerBatch;

        // same as example above here
        for (auto row : result)
        {
            std::cout << "Name : " << row["name"] << " Tel : " << row["num_tel"] << "\n";
        }
    } while (res.size() == rowsPerBatch); // If we have less results than wanted, we were on our last loop
    transaction3->commit();

    // ==== Example iteration (stream-based approach) ====
    // ! Warning : not tested, but should work
    auto transaction4 = postgresConnection->createTransaction();
    pqxx::stream_from stream = pqxx::stream_from::query(*(transaction3.get()), "SELECT * FROM test;");
    // The tuple should have the same form as we want to retrieve
    std::tuple<std::string, std::string> row;
    while(stream >> row)
    {
        std::cout << "Name : " << std::get<0>(row) << " Tel : " << std::get<0>(row) << "\n";
    }
    transaction4->commit();

    return 0;
}
