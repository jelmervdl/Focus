#ifndef __CONDITIONS_H__
#define __CONDITIONS_H__

#include <map>
#include <string>
#include <iostream>

class Conditions : public std::map<std::string, bool>
{
  bool d_ok;

 public:
 Conditions(): d_ok(true){}

  void add_condition(std::string name)
  {
    (*this)[name] = false;
    d_ok = check_ok();
  }

  void ok(std::string name, bool status)
  {
    (*this)[name] = status;
    d_ok = check_ok();
  }

  bool ok(std::string name)
  {
    return (*this)[name];
  }

  bool check_ok()
  {
    typedef std::map<std::string, bool>::const_iterator map_it;
    map_it it(this->begin()), it_end(this->end());
    for (; it != it_end; ++it)
      if (!it->second)
        return false;
    return true;
  }

  void print_not_ok()
  {
    std::cerr << "not ok: [";
    typedef std::map<std::string, bool>::const_iterator map_it;
    map_it it(this->begin()), it_end(this->end());
    for (; it != it_end; ++it)
      if (!it->second)
        std::cerr << it->first << " ";
    std::cerr << "]" << std::endl;
  }

  bool ok()
  {
    if (!d_ok)
        print_not_ok();
    return d_ok;
  }

};

#endif
