#include "context.h"
#include "log.h"

class GlobalContext : public Context
{
  static GlobalContext *s_global_context;
  
public:
  static GlobalContext &instance()
  {
    if (!s_global_context)
      init();
    return *s_global_context;
  }

  template <typename T>
  T get(std::string name)
  {
    return string_to_t<T>((*this)[name]);
  }
  
  template <typename T>
  void set(std::string name, T value)
  {
    (*this)[name] = t_to_string(value);
  }
  
private:
    
  static void init()
  {
    s_global_context = new GlobalContext();
  }
  
};


