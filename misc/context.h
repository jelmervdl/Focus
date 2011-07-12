#ifndef __CONTEXT__
#define __CONTEXT__

#include <string>
#include <map>
#include <iostream>
#include "bytewrite.h"

class Context : public std::map<std::string, std::string>
{
 public:
  
  Context(){}
  Context(std::string name, std::string value)
    {
      (*this)[name] = value;
    }

    void print(std::ostream &out = std::cout)
    {
        for (Context::iterator it = this->begin(); it != this->end(); ++it)
            out << it->first << " " << it->second << std::endl;
    }
    
    template <typename T>
    T get(std::string name)
    {
        if (!this->count(name))
        {
            std::cerr << "context does not have: " << name << " in its map" << std::endl;
            exit(1);
        }
        return string_to_t<T>((*this)[name]);
    }

    bool has(std::string name)
    {
        return this->count(name);
    }
};

#endif
