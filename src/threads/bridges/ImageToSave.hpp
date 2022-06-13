#ifndef __IMAGETOSAVE_H__
#define __IMAGETOSAVE_H__

#include <vector>

struct ImageToSave
{
    std::string collectionId = "";
    int positionId = -1;
    std::vector<uint8_t> imageData;
    int width = 640;
    int height = 480;
    bool flush = true;

    ImageToSave() {};

    ImageToSave(std::string collectionId, int positionId, std::vector<uint8_t> imageData) :
        collectionId(collectionId),
        positionId(positionId),
        // More efficient copy of the vector
        imageData(imageData.begin(), imageData.end())
    {
        this->flush = false;
    }
};

#endif // __IMAGETOSAVE_H__