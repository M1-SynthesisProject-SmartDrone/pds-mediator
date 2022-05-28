#ifndef __MEMORY_UTILS_H__
#define __MEMORY_UTILS_H__

#include <memory>

namespace memoryUtils
{

    /**
     * Cast the unique pointer to a specific type. 
     * This invalidates the old pointer.
     */
    template<typename TYPE_TO, typename TYPE_FROM>
    std::unique_ptr<TYPE_TO> static_unique_pointer_cast(std::unique_ptr<TYPE_FROM>&& old)
    {
        return std::unique_ptr<TYPE_TO>{static_cast<TYPE_TO*>(old.release())};
    }

}

#endif // __MEMORY_UTILS_H__