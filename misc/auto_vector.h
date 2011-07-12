#ifndef __AUTO_VECTOR__
#define __AUTO_VECTOR__

#include <vector>

template <typename T>
class AutoVector : public std::vector<T*>
{
 public:    
    ~AutoVector()
    {
        for (unsigned int i(0); i < this->size(); ++i)
            delete this->at(i);
    }
};

#endif
