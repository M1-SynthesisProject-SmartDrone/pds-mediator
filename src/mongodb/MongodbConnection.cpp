#include "MongodbConnection.h"

#include <sstream>

using namespace std;

MongodbConnection::MongodbConnection(MongoDbParams params) : 
    m_params(params),
    m_databaseName(params.databaseName)
{
    stringstream ss;
    ss << "mongodb://" << m_params.host << ":" << m_params.port;
    mongocxx::uri uri(ss.str());
    m_client = make_unique<mongocxx::client>(uri);
}

MongodbConnection::~MongodbConnection()
{}

mongocxx::v_noabi::database MongodbConnection::getDatabase()
{
    return m_client->database(m_databaseName);
}

