#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

struct sampleLocation {
    std::string name;       // e.g., "Educators"
    std::string type;       // e.g., "School" (for texture mapping)
    float x, y;
    std::vector<int> neighbors;
    sf::Sprite sprite;      // Sprite for icon
};

class SampleCity {
public:
    SampleCity();

    void handleEvent(sf::Event& event, bool& returnToMenu);
    void draw(sf::RenderWindow& window);
    void update(sf::RenderWindow& window); // Add update method for hover

    // Graph utilities
    const std::vector<int>& getNeighbors(int index) const;
    int findLocationByName(const std::string& name) const;

private:
    std::vector<sampleLocation> locations;
    std::map<int, std::vector<int>> adjacencyList;

    sf::RectangleShape closeButton;
    sf::Text closeText;
    sf::Font font;
    std::vector<int> currentPath;
    bool showPath = false;
    
    // Hover functionality
    int hoveredLocation;
    sf::Text hoverText;
    sf::RectangleShape hoverBackground;
    // Travel button
    sf::RectangleShape travelButton;
    sf::Text travelText;
    
    // Travel popup states
    bool showTravelPopup = false;
    bool showErrorPopup = false;
    std::string startPoint = "";
    std::string endPoint = "";
    bool typingStart = false;
    bool typingEnd = false;
};

// Global texture map
extern std::map<std::string, sf::Texture> buildingTextures;
void loadBuildingTextures(); // Call this before creating SampleCity