#include "PostgresqlConnection.h"

#include <sstream>

using namespace std;

void PostgresqlConnection::prepareStatements()
{
    // Obviously, all names should be unique (and preffered to be constants)
    // TODO : this is a test and need to be changed with appropriate prepared statements
    m_connection->prepare(STATEMENT_TEST, "INSERT INTO test(name, num_tel) VALUES($1, $2)");
    //etc.
}

PostgresqlConnection::PostgresqlConnection(PostgresParams params)
{
    m_params = params;
    stringstream sstream;
    sstream << "user=" << params.username
        << " password=" << params.password
        << " dbname=" << params.databaseName
        << " hostaddr=" << params.host
        << " port=" << params.port;
    string connectionStr = sstream.str();

    m_connection = new pqxx::connection(connectionStr);
    if (!m_connection->is_open()) 
    {
        throw runtime_error("Failed to open connection on postgresql database");
    }
    prepareStatements();
}

PostgresqlConnection::~PostgresqlConnection()
{
    delete m_connection;
}


std::unique_ptr<pqxx::work> PostgresqlConnection::createTransaction()
{
    return make_unique<pqxx::work>(*m_connection);
}
