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
    PostgresParams m_params;

    /**
     * Prepare statements beforehand, in order to be used in transactions after.
     */
    void prepareStatements();

public:
    pqxx::connection *m_connection;
    // String consts naming prepared statements
    inline static const std::string SAVE_TRIP_ENTRY = "save_trip_entry";
    inline static const std::string SAVE_BASIC_TRIP_POINT = "save_basic_trip_point";
    inline static const std::string SAVE_HISTORIC_TRIP_ENTRY = "save_historic_trip_entry";
    inline static const std::string SAVE_HISTORIC_TRIP_POINT = "save_historic_trip_point";
    inline static const std::string SAVE_TRIP_PXDATA = "save_trip_pxdata";
    inline static const std::string SAVE_HISTORIC_TRIP_PXDATA = "save_historic_trip_pxdata";
    inline static const std::string GET_CHECKPOINT_BASICTRIP = "get_checkpoint_basictrip";
    inline static const std::string SAVE_ERROR_DATA = "save_error_data";
    inline static const std::string GET_ONE_PATH = "get_one_path";

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