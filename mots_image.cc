#include "misc/misc_package.h"
#include "systems/systems_package.h"
#include "resources/resources_package.h"
#include "image/image_package.h"
#include "datastructs/datastructs_package.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <ios>
#include <time.h>

using namespace std;

Context context;

void create_selection_file(std::string filename, unsigned int size, float portion)
{
  vector<bool> selection(size);
  fill(selection.begin(), selection.end(), false);
  fill(selection.begin(), selection.begin() + selection.size() * portion, true);
  random_shuffle(selection.begin(), selection.end());

  ofstream outfile(filename.c_str());
  byte_write_vector(outfile, selection);
}

void normalize_collection(SmartVector<DataElement> *collection_ptr)
{
  SmartVector<DataElement> &collection(*collection_ptr);

  for (size_t i(0); i < collection.size(); ++i)
	collection[i]->data = normalize_std(collection[i]->data, 0.3);
}

SmartVector<DataElement> get_normalized_collection(std::string dir_name)
{
  string resource_dir_name(dir_name);
  string resource_name("annotated");
  string resource_type("normal");

  CollectionManager cm = CollectionManager::get_instance();

  cm.add_collection(resource_name, resource_dir_name, resource_type);
  log("collection added");
  smart_pointer<Collection> collection_ptr = cm.get_collection(resource_name);
  log("got pointer");

  SmartVector<DataElement> collection = collection_ptr->collection();
  //normalize_collection(&collection);
  return collection;
}

void do_stuff()
{
  string resource_dir_name(context.get<string>("dir"));

  string save_file(context.get<string>("save_file"));


  log("start loading");
  SmartVector<DataElement> collection(get_normalized_collection(resource_dir_name));

  int index = rand() % collection.size();
  DataElement element = *collection[index];
  size_t width = element.get_meta<size_t>("width");
  cout << t_to_string<size_t>(width * element.get_meta<float>("eye_x")) << " " << t_to_string<size_t>(width * element.get_meta<float>("eye_y")) <<  endl;
  cout << t_to_string<size_t>(width * element.get_meta<float>("eye_right_x")) << " " << t_to_string<size_t>(width * element.get_meta<float>("eye_right_y")) <<  endl;
  cout << t_to_string<size_t>(width * element.get_meta<float>("nose_x")) << " " << t_to_string<size_t>(width * element.get_meta<float>("nose_y")) <<  endl;
  cout << t_to_string<size_t>(width * element.get_meta<float>("mouth_x")) << " " << t_to_string<size_t>(width * element.get_meta<float>("mouth_y")) <<  endl;

  PamImage image(element.data, element.get_meta<size_t>("width"), element.get_meta<size_t>("height"));
  image.save(save_file);
}

int main(int argc, char **argv)
{
  srand(time(0));
  cout.precision(4);

  if (argc % 2 == 0)
	{
	  cerr << "need even number of arguments (name value pairs)" << endl;
	  return 1;
	}

  for (int i(1); i < argc; i += 2) //read name value pairs in argument
	{
	  context[argv[i]] = argv[i + 1];
	}

  do_stuff();

  return 0;
}
