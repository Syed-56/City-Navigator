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
