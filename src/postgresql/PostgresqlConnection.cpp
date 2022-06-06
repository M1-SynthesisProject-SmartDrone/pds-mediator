#include "PostgresqlConnection.h"

#include <sstream>

using namespace std;

void PostgresqlConnection::prepareStatements()
{
    // Obviously, all names should be unique (and preffered to be constants)
    // TODO : this is a test and need to be changed with appropriate prepared statements
    m_connection->prepare(SAVE_TRIP_ENTRY, "INSERT INTO tr_basic(idTrajet, name, creationDate) VALUES ($1, $2, $3)");    //etc.
    m_connection->prepare(SAVE_TRIP_PXDATA, "INSERT INTO tr_basic_pxdatas (idTrajet, idPoint, pressure, temperature, batteryRemaining) VALUES ($1, $2, $3, $4, $5)");
    m_connection->prepare(SAVE_HISTORIC_TRIP_ENTRY, "INSERT INTO tr_historic(idLaunch, idTrajet, name, launchingDate) VALUES ($1, $2, $3, $4)");    //etc.
    m_connection->prepare(SAVE_BASIC_TRIP_POINT, "INSERT INTO tr_basic_points(idTrajet, idPoint, latitude, longitude, height, rotation, registerDate, isCheckpoint) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)");    //etc.
    m_connection->prepare(SAVE_HISTORIC_TRIP_POINT, "INSERT INTO tr_historic_points(idLaunch, idTrajet, idPoint, latitude, longitude, height, rotation, registerDate, isCheckpoint) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)");    //etc.
    m_connection->prepare(SAVE_ERROR_DATA, "INSERT INTO tr_historic_errorpoints(idlaunch, idtrajet, idpoint, action, registerdate, idpointstartmanual, idpointendmanual) VALUES ($1, $2, $3, $4, $5, $6)");
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
