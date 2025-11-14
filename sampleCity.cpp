#include "sampleCity.h"
#include <iostream>

// Global texture map
std::map<std::string, sf::Texture> buildingTextures;

void loadBuildingTextures() {
    std::vector<std::pair<std::string, std::string>> files = {
        {"Park", "images/park.png"},
        {"School", "images/school.png"},
        {"Office", "images/office.png"},
        {"Apartment", "images/apartment.png"},
        {"Market", "images/market.png"},
        {"Hospital", "images/hospital.png"}
    };

    for (auto &[type, path] : files) {
        sf::Texture tex;
        if (!tex.loadFromFile(path))
            std::cerr << "Failed to load " << path << "\n";
        else
            buildingTextures[type] = tex;
    }
}

SampleCity::SampleCity() : hoveredLocation(-1) {
    // Load font
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!\n";
    }

    // Close button
    closeButton.setSize(sf::Vector2f(40, 40));
    closeButton.setPosition(10, 10);
    closeButton.setFillColor(sf::Color(200, 50, 50));

    closeText.setFont(font);
    closeText.setString("X");
    closeText.setCharacterSize(28);
    closeText.setFillColor(sf::Color::White);
    closeText.setPosition(20, 12);

    // Hover text setup
    hoverText.setFont(font);
    hoverText.setCharacterSize(16);
    hoverText.setFillColor(sf::Color::White);
    
    hoverBackground.setFillColor(sf::Color(0, 0, 0, 180));
    hoverBackground.setOutlineThickness(1);
    hoverBackground.setOutlineColor(sf::Color::White);

    // Locations - using custom names
    Location park{"Central Park", "Park", 100, 100, {1, 2}};
    park.sprite.setTexture(buildingTextures[park.type]);
    park.sprite.setPosition(park.x - 16, park.y - 16);
    locations.push_back(park);

    Location school{"Educators Academy", "School", 300, 100, {0, 2}};
    school.sprite.setTexture(buildingTextures[school.type]);
    school.sprite.setPosition(school.x - 16, school.y - 16);
    locations.push_back(school);

    Location market{"Fresh Market", "Market", 200, 300, {0, 1}};
    market.sprite.setTexture(buildingTextures[market.type]);
    market.sprite.setPosition(market.x - 16, market.y - 16);
    locations.push_back(market);

    Location hospital{"City General Hospital", "Hospital", 500, 200, {4}};
    hospital.sprite.setTexture(buildingTextures[hospital.type]);
    hospital.sprite.setPosition(hospital.x - 16, hospital.y - 16);
    locations.push_back(hospital);

    Location apartments{"Skyline Apartments", "Apartment", 400, 400, {3}};
    apartments.sprite.setTexture(buildingTextures[apartments.type]);
    apartments.sprite.setPosition(apartments.x - 16, apartments.y - 16);
    locations.push_back(apartments);

    // Build adjacency list
    for (size_t i = 0; i < locations.size(); i++) {
        adjacencyList[i] = locations[i].neighbors;
    }
}

void SampleCity::handleEvent(sf::Event& event, bool& returnToMenu) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
        if (closeButton.getGlobalBounds().contains(mouse)) {
            returnToMenu = true; // go back to intro screen
        }
    }
}

void SampleCity::update(sf::RenderWindow& window) {
    // Check mouse position for hover
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    hoveredLocation = -1;
    
    for (size_t i = 0; i < locations.size(); i++) {
        // Check if mouse is over the sprite (with some padding)
        sf::FloatRect bounds = locations[i].sprite.getGlobalBounds();
        if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            hoveredLocation = i;
            break;
        }
    }
}

void SampleCity::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(170, 255, 170)); // light green background

    // Draw roads
    for (size_t i = 0; i < locations.size(); i++) {
        for (int neighbor : locations[i].neighbors) {
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(locations[i].x, locations[i].y), sf::Color::Black),
                sf::Vertex(sf::Vector2f(locations[neighbor].x, locations[neighbor].y), sf::Color::Black)
            };
            window.draw(line, 2, sf::Lines);
        }
    }

    // Draw locations
    for (auto &loc : locations) {
        window.draw(loc.sprite);
    }

    // Draw hover text if hovering over a location
    if (hoveredLocation != -1) {
        const Location& loc = locations[hoveredLocation];
        
        // Set hover text
        hoverText.setString(loc.name);
        
        // Position text above the sprite
        sf::FloatRect textBounds = hoverText.getLocalBounds();
        float textX = loc.x - textBounds.width / 2;
        float textY = loc.y - 40; // Position above the icon
        
        hoverText.setPosition(textX, textY);
        
        // Create background for text
        float padding = 5.0f;
        hoverBackground.setSize(sf::Vector2f(textBounds.width + padding * 2, textBounds.height + padding * 2));
        hoverBackground.setPosition(textX - padding, textY - padding);
        
        window.draw(hoverBackground);
        window.draw(hoverText);
    }

    // Draw close button
    window.draw(closeButton);
    window.draw(closeText);
}

// Graph utilities
const std::vector<int>& SampleCity::getNeighbors(int index) const {
    return adjacencyList.at(index);
}

int SampleCity::findLocationByName(const std::string& name) const {
    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i].name == name) return i;
    }
    return -1; // not found
}