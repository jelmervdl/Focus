#ifndef __PRESENTER_H_
#define __PRESENTER_H_

#include "../misc/misc_package.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>



class Presenter
{
    sf::RenderWindow d_app;
    sf::Image d_image;
    sf::Sprite d_sprite;
 
    size_t d_app_width, d_app_height;
    
public:
    
    Presenter(std::string name, size_t app_width = 600, size_t app_height = 600)
    :
        d_app(sf::VideoMode(app_width, app_height), name),
        d_app_width(app_width),
        d_app_height(app_height)
    {
        d_sprite.SetImage(d_image);
    }
    
    
    void image_from_vector(std::vector<float> const &data)
    {
        uint32_t const n_pixels = data.size();
        
        uint32_t image_width(0), image_height(0);
        image_width = sqrt(n_pixels);
        image_height = n_pixels /  image_width;
    
        sf::Color* color_data = new sf::Color[n_pixels];
        for (uint32_t i(0); i < n_pixels; ++i)
        {
            char c = static_cast<char>((data[i]) * 255);
            color_data[i] = sf::Color(c, c, c);
        }
        
        d_image.Create(image_width, image_height, sf::Color(0, 0, 0));
        d_image.LoadFromPixels(image_width, image_height, reinterpret_cast<sf::Uint8*>(color_data));
        
        d_sprite.SetSubRect(sf::IntRect(0, 0, image_width, image_height));
        d_sprite.Resize(d_app_width, d_app_height);
        d_sprite.SetImage(d_image);
    }
    
    void show(std::vector<float> &data)
    {
        image_from_vector(normalize_vector(data, 0.0, 1.0));
        
        d_app.SetActive();
        d_app.Clear();
        d_app.Draw(d_sprite);
        d_app.Display();
    }
};



#endif