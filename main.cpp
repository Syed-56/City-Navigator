//use this to compile:
//g++ main.cpp introScreen.cpp sampleCity.cpp -o City-Navigator.exe -I"C:/SFML-2.6.1/include" -L"C:/SFML-2.6.1/lib" -lsfml-graphics -lsfml-window -lsfml-system

#include <SFML/Graphics.hpp>
#include "introScreen.h"
#include "sampleCity.h"

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(1200, 720), "City Navigator", sf::Style::Titlebar | sf::Style::Close);
    
    // Initialize global textures
    loadBuildingTextures();
    
    IntroScreen intro(window);
    
    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Pass events to intro screen
            intro.handleEvent(event, window);
        }
        intro.update(window);
        window.clear();
        intro.draw(window);
        window.display();
    }
    
    return 0;
}