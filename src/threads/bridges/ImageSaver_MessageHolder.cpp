#include "ImageSaver_MessageHolder.h"

using namespace std;

ImageSaver_MessageHolder::ImageSaver_MessageHolder()
{}

ImageSaver_MessageHolder::~ImageSaver_MessageHolder()
{}

bool ImageSaver_MessageHolder::canTryGettingMessage()
{
    return !m_messagesQueue.empty();
}

std::unique_ptr<ImageToSave> ImageSaver_MessageHolder::pop()
{
    unique_lock<mutex> lock(m_lock);
    m_condition_variable.wait(lock, [=] { return this->canTryGettingMessage(); });
    auto message = move(m_messagesQueue.front());
    m_messagesQueue.pop();
    return move(message);
}

void ImageSaver_MessageHolder::add(std::unique_ptr<ImageToSave> imageToSave)
{
    unique_lock<mutex> lock(m_lock);
    m_messagesQueue.push(move(imageToSave));
    m_condition_variable.notify_all();
}

void ImageSaver_MessageHolder::flush()
{
    // Send a flush message
    this->add(make_unique<ImageToSave>());
}
