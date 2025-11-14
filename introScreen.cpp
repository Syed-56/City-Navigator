#include "introScreen.h"
#include "sampleCity.h"
#include <iostream>
#include <SFML/System.hpp>

IntroScreen::IntroScreen(sf::RenderWindow& window) : 
    showPopup(false), 
    transitionActive(false), 
    navigatingCity(false), 
    city(nullptr) 
{
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

    // Title
    title.setFont(font);
    title.setString("City Navigator");
    title.setCharacterSize(70);
    title.setFillColor(sf::Color::White);
    title.setPosition(windowSize.x / 2.f - title.getLocalBounds().width / 2.f, 100);

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

    // Popup setup
    sf::Vector2f popupSize(window.getSize().x / 2.f, window.getSize().y / 2.f);
    popupRect.setSize(popupSize);
    popupRect.setOrigin(popupSize.x / 2.f, popupSize.y / 2.f);
    popupRect.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    popupRect.setFillColor(sf::Color(0, 0, 0, 180));
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
    createCityButtonRect.setFillColor(sf::Color(50, 150, 250));
    createCityButtonRect.setOrigin(popupButtonSize.x / 2.f, popupButtonSize.y / 2.f);
    createCityButtonRect.setPosition(popupCenterX, popupCenterY - 30);

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
    navigateCityButtonRect.setFillColor(sf::Color(50, 200, 50));
    navigateCityButtonRect.setOrigin(popupButtonSize.x / 2.f, popupButtonSize.y / 2.f);
    navigateCityButtonRect.setPosition(popupCenterX, popupCenterY + 40);

    navigateCityButtonText.setFont(font);
    navigateCityButtonText.setString("Navigate City");
    navigateCityButtonText.setCharacterSize(20);
    navigateCityButtonText.setFillColor(sf::Color::White);
    navigateCityButtonText.setPosition(
        navigateCityButtonRect.getPosition().x - navigateCityButtonText.getLocalBounds().width / 2.f,
        navigateCityButtonRect.getPosition().y - navigateCityButtonText.getLocalBounds().height
    );

    // Position UI elements
    sf::Vector2u updatedWindowSize = window.getSize();
    title.setPosition(updatedWindowSize.x / 2.f - title.getLocalBounds().width / 2.f, 100);
    startButtonRect.setPosition(updatedWindowSize.x / 2.f - startButtonRect.getSize().x / 2.f, 300);
    startButtonText.setPosition(
        startButtonRect.getPosition().x + startButtonRect.getSize().x / 2.f - startButtonText.getLocalBounds().width / 2.f,
        startButtonRect.getPosition().y + startButtonRect.getSize().y / 2.f - startButtonText.getLocalBounds().height
    );
    exitButtonRect.setPosition(updatedWindowSize.x / 2.f - exitButtonRect.getSize().x / 2.f, 400);
    exitButtonText.setPosition(
        exitButtonRect.getPosition().x + exitButtonRect.getSize().x / 2.f - exitButtonText.getLocalBounds().width / 2.f,
        exitButtonRect.getPosition().y + exitButtonRect.getSize().y / 2.f - exitButtonText.getLocalBounds().height
    );
}

IntroScreen::~IntroScreen() {
    if (city) {
        delete city;
    }
}

void IntroScreen::handleEvent(sf::Event& event, sf::RenderWindow& window) {
    if (navigatingCity && city) {
        bool returnToMenu = false;
        city->handleEvent(event, returnToMenu);
        if (returnToMenu) {
            navigatingCity = false;
            delete city;
            city = nullptr;
        }
        return;
    }

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (!showPopup) {
            startButtonRect.setFillColor(
                startButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ?
                sf::Color(80, 255, 80) : sf::Color(50, 200, 50));

            exitButtonRect.setFillColor(
                exitButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ?
                sf::Color(255, 80, 80) : sf::Color(200, 50, 50));
        } else {
            // Handle popup button hovers
            createCityButtonRect.setFillColor(
                createCityButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ?
                sf::Color(80, 180, 255) : sf::Color(50, 150, 250));

            navigateCityButtonRect.setFillColor(
                navigateCityButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ?
                sf::Color(80, 255, 80) : sf::Color(50, 200, 50));

            closeButtonRect.setFillColor(
                closeButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)) ?
                sf::Color(255, 80, 80) : sf::Color(200, 50, 50));
        }
    }

    // Mouse clicks
    else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (showPopup) {
            if (closeButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                showPopup = false;
            else if (createCityButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                std::cout << "Create New City clicked!\n";
            else if (navigateCityButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                std::cout << "Navigate City clicked!\n";
                transitionActive = true;
                transitionClock.restart();
            }
        } else {
            if (startButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                showPopup = true;
            else if (exitButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                window.close();
        }
    }
}

void IntroScreen::update(sf::RenderWindow& window) {
    if (navigatingCity && city) {
        city->update(window);
    }
}


void IntroScreen::draw(sf::RenderWindow& window) {
    if (navigatingCity && city) {
        city->draw(window);
        return;
    }

    // Draw intro screen
    window.draw(background);
    window.draw(title);
    window.draw(startButtonRect);
    window.draw(startButtonText);
    window.draw(exitButtonRect);
    window.draw(exitButtonText);

    if (showPopup) {
        sf::Vector2u windowSize = window.getSize();
        sf::RectangleShape overlay(sf::Vector2f(windowSize.x, windowSize.y));
        overlay.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(overlay);

        window.draw(popupRect);
        window.draw(closeButtonRect);
        window.draw(closeButtonText);
        window.draw(createCityButtonRect);
        window.draw(createCityButtonText);
        window.draw(navigateCityButtonRect);
        window.draw(navigateCityButtonText);
    }

    if (transitionActive) {
        if (transitionClock.getElapsedTime().asSeconds() >= 1.0f) {
            transitionActive = false;
            navigatingCity = true;
            city = new SampleCity();
        } else {
            sf::Vector2u windowSize = window.getSize();
            sf::Text loadingText("Loading City...", font, 40);
            loadingText.setFillColor(sf::Color::White);
            loadingText.setPosition(windowSize.x/2.f - loadingText.getLocalBounds().width/2.f,
                                    windowSize.y/2.f - loadingText.getLocalBounds().height/2.f);
            window.draw(loadingText);
        }
    }
}