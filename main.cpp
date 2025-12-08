//to compile:
// g++ main.cpp introScreen.cpp sampleCity.cpp createCity.cpp travelFunc.cpp -o City-Navigator.exe -I"C:/SFML-2.6.1/include" -L"C:/SFML-2.6.1/lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "introScreen.h"
#include "sampleCity.h"
#include <iostream>

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "City Navigator", sf::Style::Fullscreen);
    sf::Music backgroundMusic;
    
    if (backgroundMusic.openFromFile("song.mp3")) {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.f);
        backgroundMusic.play();
        std::cout << "Background music started.\n";
    } 
    else {
        std::cerr << "Warning: Could not load song.mp3\n";
    }
   
    loadBuildingTextures();
    IntroScreen intro(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            intro.handleEvent(event, window);
        }

        intro.update(window);

        window.clear();
        intro.draw(window);
        window.display();
    }

    return 0;
}