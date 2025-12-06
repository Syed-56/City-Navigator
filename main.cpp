//to compile:
// g++ main.cpp introScreen.cpp sampleCity.cpp -o City-Navigator.exe -I"C:/SFML-2.6.1/include" -L"C:/SFML-2.6.1/lib" -lsfml-graphics -lsfml-window -lsfml-system (type .cpp file names whom we are coding here we did introscreen and samplecity and main will always be compiled)
#include <SFML/Graphics.hpp>
#include "introScreen.h"
#include "sampleCity.h"

int main()
{
    // Get desktop resolution
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // Create fullscreen window
    sf::RenderWindow window(desktop, "City Navigator", sf::Style::Fullscreen);

    // Initialize global textures
    loadBuildingTextures();

    // Initialize IntroScreen
    IntroScreen intro(window);

    // Main loop
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
