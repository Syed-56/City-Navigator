//use this to compile:
//g++ main.cpp introScreen.cpp -o City-Navigator.exe -I"C:/SFML-2.6.1/include" -L"C:/SFML-2.6.1/lib" -lsfml-graphics -lsfml-window -lsfml-system

#include <SFML/Graphics.hpp>
#include "introScreen.h"

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(1200, 720), "City Navigator", sf::Style::Titlebar | sf::Style::Close);
    IntroScreen intro(window);      // Create intro screen object
    intro.show(window);     // Show intro screen
    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.display();
    }

    
    return 0;
}