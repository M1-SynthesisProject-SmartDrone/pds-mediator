#ifndef __MONGODBCONNECTION_H__
#define __MONGODBCONNECTION_H__

#include <memory>

// All includes convinient to use mongodb api
#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "config/ConfigParams.h"

class MongodbConnection
{
private:
    /**
     * A program must have only one mongodb instance running 
     * (connections are made by mongocxx::client instances)
     */
    inline static const mongocxx::instance INSTANCE{};

    const MongoDbParams m_params;
    const std::string m_databaseName;
    std::unique_ptr<mongocxx::client> m_client;

public:
    MongodbConnection(MongoDbParams params);
    ~MongodbConnection();

    mongocxx::v_noabi::database getDatabase();
};

#endif // __MONGODBCONNECTION_H__