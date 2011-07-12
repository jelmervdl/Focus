#ifndef __COLLECTION_MANAGER__
#define __COLLECTION_MANAGER__

#include "resources_package.h"
#include "../misc/misc_package.h"

#include <map>
#include <string>

#include "collection.h"
#include "lfw_collection.h"

class CollectionManager
{
   typedef std::map<std::string, smart_pointer<Collection> > collection_map;
   collection_map d_collection_map;
   
   static CollectionManager* s_collection_manager;
    
 public:
    CollectionManager()
    {
        load();
    }
   
    void load()
    {
        
    }
   
    static void initialize()
    {
        if (!s_collection_manager)
            CollectionManager::s_collection_manager = new CollectionManager();
    }

    static CollectionManager &get_instance()
    {
        CollectionManager::initialize();
        return *s_collection_manager;
    }
   
   smart_pointer<Collection> get_collection(std::string name)
   {
        if (!d_collection_map.count(name))
        {
            throw SimpleException(std::string("that resource does not exist: ") + name);
        }
        return d_collection_map[name];
   }
   
   void add_collection(std::string name, std::string dir, std::string type)
   {
      assert(name.size() && dir.size() && type.size());
      log(type, EXTRA);
      if (type == "lfw")
            d_collection_map[name] = new LFWCollection(name, type, dir);
      else if (type == "normal")
            d_collection_map[name] = new DataElementCollection(name, type, dir);
      else if (type == "old")
            d_collection_map[name] = new DataElementOldCollection(name, type, dir);
      else
            throw SimpleException(std::string("unknown collection type: ") + type);
   }
   
   
};

#endif
