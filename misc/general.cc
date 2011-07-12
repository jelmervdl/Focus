#include "general.h"
#include "../image/image_package.h"

//unsigned int log_print_importance = 0;

std::vector<std::string> ls(std::string dir)
{
    std::string temp_file = get_temporary_filename();
    std::vector<std::string> filenames;
    
    std::string command = std::string("ls ") + dir + " >" + temp_file;
    std::cout << "command: " << command << std::endl;
    system(command.c_str());
    
    std::ifstream results_file(temp_file.c_str());
    std::string all_text;
    
    {   //skip first line
        std::string line;
        getline(results_file, line);
        all_text += line + " ";
    }

    while (true)
    {
        std::string line;
        getline(results_file, line);
        if (results_file.eof())
            break;
        {
            if (line.size())
            all_text += line + " ";
        }
    }
    
    
    while (true)
    {
        size_t pos = all_text.find(" ");
        if (pos == std::string::npos)
            break;

        std::string line = strip(all_text.substr(0, pos + 1));

        all_text = all_text.substr(pos + 1);
        
        if (line.size())
        {
            filenames.push_back(line);
        }
    }
    
    return filenames;
}

void show_image(PamImage image, std::string save_file)
{
    std::string filename = get_temporary_filename();
    image.save(filename);
    if (!save_file.size())
        save_file = get_temporary_filename() + ".eps";
    
    std::string command_convert(std::string("convert ") + filename + " " + save_file);
    std::string command_open(std::string("open ") + save_file);
    system(command_convert.c_str());
    //system(command_open.c_str());
}

void show_image(std::vector<float> const &v, std::string save_file, unsigned int width, unsigned int height)
{
    if (!width || !height)
    {
        width = sqrt(v.size());
        height = v.size() / width;
    }
    show_image(PamImage(v, width, height), save_file);
}
