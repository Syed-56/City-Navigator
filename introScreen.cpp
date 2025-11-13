#include "introScreen.h"
#include <iostream>

IntroScreen::IntroScreen(sf::RenderWindow& window) {
    // Load font
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!\n";
    }

    // Load background image
    if (!backgroundTexture.loadFromFile("images/bg-introScreen.png")) {
        std::cerr << "Failed to load background image!\n";
    } else {
        background.setTexture(backgroundTexture);
    }

    sf::Vector2u windowSize = window.getSize();

    // Scale background to fit window
    sf::Vector2u imageSize = backgroundTexture.getSize();
    float scaleX = float(windowSize.x) / imageSize.x;
    float scaleY = float(windowSize.y) / imageSize.y;
    background.setScale(scaleX, scaleY);

    // Position title
    title.setPosition(windowSize.x / 2.f - title.getLocalBounds().width / 2.f, 100);
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

    showPopup = false;

    sf::Vector2f popupSize(window.getSize().x / 2.f, window.getSize().y / 2.f);
    popupRect.setSize(popupSize);
    popupRect.setOrigin(popupSize.x / 2.f, popupSize.y / 2.f);
    popupRect.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    popupRect.setFillColor(sf::Color(0, 0, 0, 180)); // translucent black
    popupRect.setOutlineThickness(3);
    popupRect.setOutlineColor(sf::Color::White);

    closeButtonRect.setSize(sf::Vector2f(30, 30));
    closeButtonRect.setFillColor(sf::Color(200, 50, 50));
    closeButtonRect.setOrigin(closeButtonRect.getSize().x / 2.f, closeButtonRect.getSize().y / 2.f);
    closeButtonRect.setPosition(popupRect.getPosition().x + popupRect.getSize().x / 2.f - 20,
                                popupRect.getPosition().y - popupRect.getSize().y / 2.f + 20);

    closeButtonText.setFont(font);
    closeButtonText.setString("X");
    closeButtonText.setCharacterSize(20);
    closeButtonText.setFillColor(sf::Color::White);
    closeButtonText.setPosition(
        closeButtonRect.getPosition().x - closeButtonText.getLocalBounds().width / 2.f,
        closeButtonRect.getPosition().y - closeButtonText.getLocalBounds().height
    );

    sf::Vector2f popupButtonSize(200, 50);
float popupCenterX = popupRect.getPosition().x;
float popupCenterY = popupRect.getPosition().y;

// Create New City Button
createCityButtonRect.setSize(popupButtonSize);
createCityButtonRect.setFillColor(sf::Color(50, 150, 250)); // blue
createCityButtonRect.setOrigin(popupButtonSize.x / 2.f, popupButtonSize.y / 2.f);
createCityButtonRect.setPosition(popupCenterX, popupCenterY - 30); // above center

createCityButtonText.setFont(font);
createCityButtonText.setString("Create New City");
createCityButtonText.setCharacterSize(20);
createCityButtonText.setFillColor(sf::Color::White);
createCityButtonText.setPosition(
    createCityButtonRect.getPosition().x - createCityButtonText.getLocalBounds().width / 2.f,
    createCityButtonRect.getPosition().y - createCityButtonText.getLocalBounds().height
);

// Navigate City Button
    navigateCityButtonRect.setSize(popupButtonSize);
    navigateCityButtonRect.setFillColor(sf::Color(50, 200, 50)); // green
    navigateCityButtonRect.setOrigin(popupButtonSize.x / 2.f, popupButtonSize.y / 2.f);
    navigateCityButtonRect.setPosition(popupCenterX, popupCenterY + 40); // below center

    navigateCityButtonText.setFont(font);
    navigateCityButtonText.setString("Navigate City");
    navigateCityButtonText.setCharacterSize(20);
    navigateCityButtonText.setFillColor(sf::Color::White);
    navigateCityButtonText.setPosition(
        navigateCityButtonRect.getPosition().x - navigateCityButtonText.getLocalBounds().width / 2.f,
        navigateCityButtonRect.getPosition().y - navigateCityButtonText.getLocalBounds().height
    );
}

void IntroScreen::show(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();

    title.setPosition(windowSize.x / 2.f - title.getLocalBounds().width / 2.f, 100);

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
                if (startButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    startButtonRect.setFillColor(sf::Color(80, 255, 80));
                else
                    startButtonRect.setFillColor(sf::Color(50, 200, 50));

                if (exitButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    exitButtonRect.setFillColor(sf::Color(255, 80, 80));
                else
                    exitButtonRect.setFillColor(sf::Color(200, 50, 50));
            }

            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            
                if (showPopup) {
                    // Check close button
                    if (closeButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        showPopup = false; // close popup
                    }
                    // Create New City
                    if (createCityButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "Create New City clicked!\n";
                        // TODO: Add logic for creating new city
                    }

                    // Navigate City
                    if (navigateCityButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "Navigate City clicked!\n";
                        // TODO: Add logic for navigating city
                    }
                } else {
                    // Start clicked
                    if (startButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "Start clicked!\n";
                        showPopup = true; // show popup
                    }
            
                    // Exit clicked
                    if (exitButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        window.close();
                    }
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

        if (showPopup) {
            // Optional dim overlay
            sf::RectangleShape overlay(sf::Vector2f(windowSize.x, windowSize.y));
            overlay.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(overlay);
        
            // Draw popup
            window.draw(popupRect);
        
            // Draw close button on top
            window.draw(closeButtonRect);
            window.draw(closeButtonText);

            // Draw popup buttons
            window.draw(createCityButtonRect);
            window.draw(createCityButtonText);

            window.draw(navigateCityButtonRect);
            window.draw(navigateCityButtonText);
        }
        
        window.display();
    }
}