//to compile:
// g++ main.cpp introScreen.cpp sampleCity.cpp createCity.cpp travelFunc.cpp -o City-Navigator.exe -I"C:/SFML-2.6.1/include" -L"C:/SFML-2.6.1/lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "introScreen.h"
#include "sampleCity.h"
#include <iostream>

int main()
{
    // Get desktop resolution
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // Create fullscreen window
    sf::RenderWindow window(desktop, "City Navigator", sf::Style::Fullscreen);

    // ============ MUSIC SETUP ============
    sf::Music backgroundMusic;
    
    // Try to load background music
    if (backgroundMusic.openFromFile("song.mp3")) {
        // Set music properties
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.f);
        backgroundMusic.play();
        std::cout << "Background music started.\n";
    } 
    else {
        std::cerr << "Warning: Could not load song.mp3\n";
    }
    // ====================================

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
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }

            intro.handleEvent(event, window);
        }

        intro.update(window);

        window.clear();
        intro.draw(window);
        window.display();
    }

    return 0;
}