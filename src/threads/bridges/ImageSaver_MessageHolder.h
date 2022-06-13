#ifndef __IMAGESAVER_MESSAGEHOLDER_H__
#define __IMAGESAVER_MESSAGEHOLDER_H__

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "ImageToSave.hpp"

/**
 * Thread-safe class containing a queue of images to save to database.
 * @author Aldric Vitali Silvestre
 */
class ImageSaver_MessageHolder
{
private:
    std::mutex m_lock;
    std::condition_variable m_condition_variable;
    std::queue<std::unique_ptr<ImageToSave>> m_messagesQueue;

    bool canTryGettingMessage();

public:
    std::string currentCollectionId = "NONE";

    ImageSaver_MessageHolder();
    ~ImageSaver_MessageHolder();

    std::unique_ptr<ImageToSave> pop();
    void add(std::unique_ptr<ImageToSave> imageToSave);

    /**
     * ask to save all remaining images in queue, if any
     */
    void flush();
};

#endif // __IMAGESAVER_MESSAGEHOLDER_H__