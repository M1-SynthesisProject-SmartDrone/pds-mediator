#ifndef __CONFIGPARAMS_H__
#define __CONFIGPARAMS_H__

#include <string>

struct PostgresParams
{
    std::string username;
    std::string password;
    std::string databaseName;
    std::string host;
    int port;
};

struct MongoDbParams
{
    std::string databaseName;
    std::string host;
    int port;
};


/**
 * This struct contains all data parsed in the config file
 */
struct ConfigParams
{
    PostgresParams postgres;
    MongoDbParams mongoDb;

    ConfigParams(PostgresParams postgres, MongoDbParams mongoDb)
    {
        this->postgres = postgres;
        this->mongoDb = mongoDb;
    }
};


#endif // __CONFIGPARAMS_H__