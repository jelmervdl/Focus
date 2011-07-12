#ifndef __RECOURCE_CONTEXT_MANAGER__
#define __RECOURCE_CONTEXT_MANAGER__

//singleton class, mapping resource names to filename etc.
//maps from name to resource
//a resource maps from names, or functions to one or several streams to access information

#include <string>
#include <map>
#include <istream>
#include "resource.h"


//TODO make class

class ResourceContextManager {

    std::string d_config_file;

public:
    ResourceContextManager(std::string const &config_file): d_config_file(config_file) {}
    Context get_context(std::string const &context_name);
};

#endif
