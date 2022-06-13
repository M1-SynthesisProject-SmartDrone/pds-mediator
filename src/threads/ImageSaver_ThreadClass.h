#ifndef __IMAGESAVER_THREADCLASS_H__
#define __IMAGESAVER_THREADCLASS_H__

#include <queue>

#include "Abstract_ThreadClass.h"
#include "config/ConfigParams.h"
#include "mongodb/MongodbConnection.h"
#include "threads/bridges/ImageSaver_MessageHolder.h"

class ImageSaver_ThreadClass : public Abstract_ThreadClass
{
private:
    /**
     * Number of images to save before saving all at once
     */
    static constexpr uint8_t SAVE_BATCH_SIZE = 10;

    MongodbConnection m_mongoConnection;
    mongocxx::options::bulk_write m_bulkOptions;

    std::shared_ptr<ImageSaver_MessageHolder> m_imageMessageHolder;
    std::queue<std::unique_ptr<ImageToSave>> m_imageQueue;

    // ==== PRIVATE METHODS ====
    bool isSaveNeeded();
    void saveImages();
public:
    ImageSaver_ThreadClass(MongoDbParams mongoDbParams, std::shared_ptr<ImageSaver_MessageHolder> imageMessageHolder);
    ~ImageSaver_ThreadClass();

    void run();
};

#endif // __IMAGESAVER_THREADCLASS_H__