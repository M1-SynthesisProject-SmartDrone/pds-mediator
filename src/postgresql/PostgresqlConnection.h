#ifndef __POSTGRESQLCONNECTION_H__
#define __POSTGRESQLCONNECTION_H__

#include <pqxx/pqxx>
#include <memory>

#include "config/ConfigParams.h"

// ! Note : this is not a singleton as it would be compilcated to use this class in multiple threads else
// ! For instance, if two transactions are on the same connection and one commit, the other is automatically closed.

/**
 * Wrapper class helping opeining connections towards postgresql database.
 */
class PostgresqlConnection
{
private:
    pqxx::connection* m_connection;
    PostgresParams m_params;

    /**
     * Prepare statements beforehand, in order to be used in transactions after.
     */
    void prepareStatements();
public:
    // String consts naming prepared statements
    inline static const std::string STATEMENT_TEST = "test_statement";

    PostgresqlConnection(PostgresParams params);
    ~PostgresqlConnection();

    /**
     * Create a transaction on the actual connection. 
     * You should not forget to call "commit()" at the end of the transaction.
     */
    std::unique_ptr<pqxx::work> createTransaction();
    // ! NOTE : the "unique_ptr" assures that the pointer is deleted at the end of the scope where it belongs
    // ! If the unique_ptr needs to be passed to another "entity" (class, etc.), the method std::move(ptr) must be called
    // ! see : https://en.cppreference.com/w/cpp/utility/move

};

#endif // __POSTGRESQLCONNECTION_H__