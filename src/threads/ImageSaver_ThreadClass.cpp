
#include "ImageSaver_ThreadClass.h"

#include <chrono>
#include <loguru/loguru.hpp>

using namespace std;

ImageSaver_ThreadClass::ImageSaver_ThreadClass(MongoDbParams mongoDbParams,
    std::shared_ptr<ImageSaver_MessageHolder> imageMessageHolder) :
    Abstract_ThreadClass("image_saver", 1000, 1000),
    m_mongoConnection(mongoDbParams)
{
    m_imageMessageHolder = imageMessageHolder;

    // Init values for the bulk operation to be faster (not necessarily useful though)
    mongocxx::write_concern writeConcern;
    writeConcern.acknowledge_level(mongocxx::write_concern::level::k_default);
    m_bulkOptions.ordered(false);
    m_bulkOptions.write_concern(writeConcern);
}

ImageSaver_ThreadClass::~ImageSaver_ThreadClass()
{}

void ImageSaver_ThreadClass::run()
{
    initRun();

    while (isRunFlag())
    {
        auto message = m_imageMessageHolder->pop();
        bool mustSave = false;
        if (message->flush)
        {
            mustSave = true;
        }
        else
        {
            m_imageQueue.push(move(message));
            mustSave = m_imageQueue.size() >= SAVE_BATCH_SIZE;
        }
        
        if (mustSave)
        {
            auto start = chrono::steady_clock::now();
            saveImages();
            auto end = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            LOG_F(INFO, "Batch saved on %ldms", elapsed);
        }
    }
}

void ImageSaver_ThreadClass::saveImages()
{
    if (m_imageQueue.empty())
    {
        LOG_F(WARNING, "No image to save, exit saveImages method");
        return;
    }
    LOG_F(INFO, "Save %ld images", m_imageQueue.size());
    auto database = m_mongoConnection.getDatabase();
    auto collection = database[m_imageMessageHolder->currentCollectionId];

    auto bulk = collection.create_bulk_write(m_bulkOptions);
    while (!m_imageQueue.empty())
    {
        auto imageToSave = move(m_imageQueue.front());

        bsoncxx::builder::basic::array arrrayBuilder{};

        auto &image = imageToSave->imageData;
        bsoncxx::array::view a{ image.data(), image.size() };
        for (auto const& value : image)
        {
            arrrayBuilder.append(value);
        }
        auto array = arrrayBuilder.view();
        
        // Create the document (a json object)
        auto builder = bsoncxx::builder::stream::document{};
        auto docValue = builder
            << "id_pos" << imageToSave->positionId
            << "width" << imageToSave->width
            << "height" << imageToSave->height
            << "image" << array
            << bsoncxx::builder::stream::finalize;
        
        mongocxx::model::insert_one insertOp(docValue.view());
        bulk.append(insertOp);

        m_imageQueue.pop();
    }
    bulk.execute();
}
