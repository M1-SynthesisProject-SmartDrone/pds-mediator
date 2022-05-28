#include "config_parser.h"

#include <filesystem>
#include <libconfig.h++>

using namespace std;
namespace fs = std::filesystem;

/**
 * Find the config file path from the executable path
 */
fs::path getConfigPath(char *argv[])
{
    // argv[0] contains the executable called
    fs::path exePath(argv[0]);
    fs::path exeDirectoryPath = exePath.parent_path();
    fs::path configPath(exeDirectoryPath.string() + "/config.cfg");
    if (!fs::exists(configPath))
    {
        throw runtime_error("Config file not found at path : " + configPath.string());
    }
    return configPath;
}

ConfigParams parseConfig(int argc, char *argv[])
{
    fs::path configPath = getConfigPath(argv);

    libconfig::Config config;
    config.readFile(configPath.c_str());

    const libconfig::Setting& root = config.getRoot();

    const auto& postgresSettings = root["postgresql"];
    const auto postgresConfig = PostgresParams{
        postgresSettings["username"],
        postgresSettings["password"],
        postgresSettings["database_name"],
        postgresSettings["host"],
        postgresSettings["port"]
    };

    const auto& mongoSettings = root["mongodb"];
    const auto mongoConfig = MongoDbParams{
        mongoSettings["database_name"],
        mongoSettings["host"],
        mongoSettings["port"]
    };

    const auto& commSettings = root["ports"];
    const auto communcationConfig = CommunicationpParams{
        commSettings["inputport"],
        commSettings["outputport"],


    };

    return ConfigParams(postgresConfig, mongoConfig, communcationConfig);
}