#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "config/config_parser.h"
#include "postgresql/PostgresqlConnection.h"
#include "mongodb/MongodbConnection.h"
#include "network/TCPSocket.h"
#include <loguru/loguru.hpp>
#include "request_analyse/RequestAnalyser.h"
#include "threads/bridges/ImageSaver_MessageHolder.h"
#include "threads/ImageSaver_ThreadClass.h"

using namespace std;

void libpqxxExample(unique_ptr<PostgresqlConnection> postgresConnection);
void mongodbExample(unique_ptr<MongodbConnection> mongodbConnection);

void initSignalHandler();
void signalHandler(int number);

int main(int argc, char *argv[])
{
    initSignalHandler();

    LOG_F(INFO, "Starting database server");
    LOG_F(INFO, "Parsing configuration ...");
    const ConfigParams config = parseConfig(argc, argv);

    // ----- VARIABLES -----

    bool isRunning = true;
    char *buffer = (char *)malloc(sizeof(char) * 512);
    sockaddr_in sender;

    // initialise logger
    loguru::init(argc, argv);

    LOG_F(INFO, "Parsing configuration done");
    LOG_F(INFO, "\tInput port : %d\n", config.communication.inputport);
    LOG_F(INFO, "\tOutput port : %d\n", config.communication.outputport);

    // creating object for UDPCommunication
    auto dataInput = make_shared<TCPSocket>();
    dataInput->waitConnection(config.communication.inputport);

    auto dataOutput = make_shared<TCPSocket>();
    dataOutput->waitConnection(config.communication.outputport);

    auto imageToSaveHolder = make_shared<ImageSaver_MessageHolder>();

    ImageSaver_ThreadClass imageSaverThread(config.mongoDb, imageToSaveHolder);
    imageSaverThread.start();

    auto requestAnalyser = RequestAnalyser(config, dataInput, dataOutput, imageToSaveHolder);

    // main thread of the server
    LOG_F(INFO, "-> Starting main thread of the program");
    while (isRunning)
    {
        try
        {
            LOG_F(INFO, "Waiting for a request ... ");
            auto buffer = dataInput->receiveMessage();

            // analyse request & execute appropriate code
            requestAnalyser.parseRequest(buffer);
        }
        catch (exception &ex)
        {
            LOG_F(ERROR, "Error while treating a request : %s", ex.what());
        }
        // await for a request from input IP & entry port
    }

    return EXIT_SUCCESS;
}

void initSignalHandler()
{
    LOG_F(INFO, "Init signal handler");
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGFPE, signalHandler);
}

void signalHandler(int number)
{
    int exitStatus = 0;
    switch (number)
    {
    case SIGINT:
        LOG_F(WARNING, "SIGINT caught");
        exit(0);
        break;
    case SIGQUIT:
        LOG_F(WARNING, "SIGQUIT caught");
        exit(0);
        break;
    case SIGTERM:
        LOG_F(WARNING, "SIGTERM caught");
        exit(1);
        break;
    case SIGFPE:
        LOG_F(WARNING, "SIGFPE caught");
        exit(2);
        break;
    default:
        LOG_F(WARNING, "Unhandled signal caught : %d", number);
        break;
    }
}

// Examples are not tested, just proof of concepts

// void libpqxxExample(unique_ptr<PostgresqlConnection> postgresConnection)
// {
//     // ==== Example insertion ====
//     auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
//     // see PostgresqlConnection#prepareStatements()
//     transaction1->exec_prepared(PostgresqlConnection::STATEMENT_TEST, "billy", "0109050402");
//     transaction1->commit();

//     // ==== Example iteration (all in-memory retrieving (BAD, but simple)) ====
//     auto transaction2 = postgresConnection->createTransaction();
//     pqxx::result result(transaction2->exec("SELECT * FROM test;"));
//     for (auto row : result)
//     {
//         std::cout << "Name : " << row["name"] << " Tel : " << row["num_tel"] << "\n";
//     }
//     transaction2->commit();

//     // ==== Example iteration (cursor-based approach) ====
//     // ! Warning : not tested, but should work
//     auto transaction3 = postgresConnection->createTransaction();
//     string query = "SELECT * FROM test;";
//     pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned>
//         cursor(*(transaction3.get()), query, "myCursor", false);
//     size_t idx = 0;
//     size_t rowsPerBatch = 10; // We will get 10 rows for each cursor iteration (batch-retrieving)
//     pqxx::result res;
//     do
//     {
//         res = cursor.retrieve(idx, idx + rowsPerBatch);
//         idx += rowsPerBatch;

//         // same as example above here
//         for (auto row : result)
//         {
//             std::cout << "Name : " << row["name"] << " Tel : " << row["num_tel"] << "\n";
//         }
//     } while (res.size() == rowsPerBatch); // If we have less results than wanted, we were on our last loop
//     transaction3->commit();

//     // ==== Example iteration (stream-based approach) ====
//     // ! Warning : not tested, but should work
//     auto transaction4 = postgresConnection->createTransaction();
//     pqxx::stream_from stream = pqxx::stream_from::query(*(transaction4.get()), "SELECT * FROM test;");
//     // The tuple should have the same form as we want to retrieve
//     std::tuple<std::string, std::string> row;
//     while (stream >> row)
//     {
//         std::cout << "Name : " << std::get<0>(row) << " Tel : " << std::get<0>(row) << "\n";
//     }
//     transaction4->commit();
// }

// tutorial available here : http://mongocxx.org/mongocxx-v3/tutorial/
void mongodbExample(unique_ptr<MongodbConnection> mongodbConnection)
{
    // The database will be created if not exists here
    auto database = mongodbConnection->getDatabase();

    // ==== Check if a collection exists (without creating it)
    bool hasCollection = database.has_collection("collection_id");
    auto collection = database["collection_id"];

    // ==== Example insertion ====
    // Create the document (a json object)
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
                                         << "name"
                                         << "billy"
                                         << "num"
                                         << "0102030405"

                                         << "int_array" << bsoncxx::builder::stream::open_array // start json array
                                         << 1 << 2 << 3
                                         << bsoncxx::builder::stream::close_array // end json array

                                         << "object" << bsoncxx::builder::stream::open_document // start json object
                                         << "attr1"
                                         << "gilbert"
                                         << "attr2"
                                         << "savan??"
                                         << bsoncxx::builder::stream::close_document // end json object

                                         << bsoncxx::builder::stream::finalize; // builder.build()

    // insert it
    collection.insert_one(doc_value.view());

    // ==== Example search ====
    auto filter = bsoncxx::builder::stream::document{}
                  << "name"
                  << "billy"
                  << bsoncxx::builder::stream::finalize;
    auto optionalDocument = collection.find_one(filter.view());
    if (optionalDocument)
    {
        cout << bsoncxx::to_json(optionalDocument.value()) << "\n";
    }

    // ==== Example iteration ====
    auto cursor = collection.find({}); // can add filters like above if wanted
    for (auto document : cursor)
    {
        cout << bsoncxx::to_json(document) << "\n";
    }
}