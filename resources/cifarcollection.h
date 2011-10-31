#ifndef __CIFAR_COLLECTION__
#define __CIFAR_COLLECTION__

#include "collection.h"

class CifarCollection : public Collection
{   
    
 public:
    CifarCollection(std::string const &name, std::string const &type, std::string const &dir_name);

    void clear();

    void load();

    bool exists() const;
};

#endif
