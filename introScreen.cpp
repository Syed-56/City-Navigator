#include "introScreen.h"
#include <iostream>

IntroScreen::IntroScreen() {
    // Load font
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!\n";
    }

    // Load background image
    if (!backgroundTexture.loadFromFile("images/bg-introScreen.jpg")) {
        std::cerr << "Failed to load background image!\n";
    } else {
        background.setTexture(backgroundTexture);
    }

    // Title
    title.setFont(font);
    title.setString("City Navigator");
    title.setCharacterSize(70);
    title.setFillColor(sf::Color::White);

    // Start Button
    startButtonRect.setSize(sf::Vector2f(300, 60));
    startButtonRect.setFillColor(sf::Color(50, 200, 50));
    startButtonText.setFont(font);
    startButtonText.setString("Start");
    startButtonText.setCharacterSize(30);
    startButtonText.setFillColor(sf::Color::White);

    // Exit Button
    exitButtonRect.setSize(sf::Vector2f(300, 60));
    exitButtonRect.setFillColor(sf::Color(200, 50, 50));
    exitButtonText.setFont(font);
    exitButtonText.setString("Exit");
    exitButtonText.setCharacterSize(30);
    exitButtonText.setFillColor(sf::Color::White);
}

void IntroScreen::show(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();

    // Position title
    title.setPosition(windowSize.x / 2.f - title.getLocalBounds().width / 2.f, 100);

    // Position buttons
    startButtonRect.setPosition(windowSize.x / 2.f - startButtonRect.getSize().x / 2.f, 300);
    startButtonText.setPosition(
        startButtonRect.getPosition().x + startButtonRect.getSize().x / 2.f - startButtonText.getLocalBounds().width / 2.f,
        startButtonRect.getPosition().y + startButtonRect.getSize().y / 2.f - startButtonText.getLocalBounds().height
    );

    exitButtonRect.setPosition(windowSize.x / 2.f - exitButtonRect.getSize().x / 2.f, 400);
    exitButtonText.setPosition(
        exitButtonRect.getPosition().x + exitButtonRect.getSize().x / 2.f - exitButtonText.getLocalBounds().width / 2.f,
        exitButtonRect.getPosition().y + exitButtonRect.getSize().y / 2.f - exitButtonText.getLocalBounds().height
    );

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Hover effect for Start
                if (startButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    startButtonRect.setFillColor(sf::Color(80, 255, 80));
                } else {
                    startButtonRect.setFillColor(sf::Color(50, 200, 50));
                }

                // Hover effect for Exit
                if (exitButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    exitButtonRect.setFillColor(sf::Color(255, 80, 80));
                } else {
                    exitButtonRect.setFillColor(sf::Color(200, 50, 50));
                }
            }

            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Start clicked
                if (startButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    std::cout << "Start clicked!\n";
                    return; // Exit intro screen to main game loop
                }

                // Exit clicked
                if (exitButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    window.close();
                }
            }
        }

        window.clear();
        window.draw(background);
        window.draw(title);
        window.draw(startButtonRect);
        window.draw(startButtonText);
        window.draw(exitButtonRect);
        window.draw(exitButtonText);
        window.display();
    }
}
