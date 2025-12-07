#include "sampleCity.h"
#include <iostream>
#include "travelFunc.h"
#include <math.h>

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

SampleCity::SampleCity() {
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

    // Travel button initialization
    const float btnWidth = 120.f, btnHeight = 40.f;
    const float travelBtnX = 100.f;  // Position on left side
    const float travelBtnY = 650.f;  // Position near bottom
    
    travelButton.setSize(sf::Vector2f(btnWidth, btnHeight));
    travelButton.setPosition(travelBtnX, travelBtnY);
    travelButton.setFillColor(sf::Color(70, 130, 180));
    travelButton.setOutlineThickness(2);
    travelButton.setOutlineColor(sf::Color::Black);

    travelText.setFont(font);
    travelText.setString("Travel");
    travelText.setCharacterSize(18);
    travelText.setFillColor(sf::Color::White);
    travelText.setPosition(travelBtnX + 28, travelBtnY + 8);
        // ==== ORGANIZED CITY MAP (OPTION A) ====

    // Predefined building types
    std::vector<std::string> types = {
        "Park", "School", "Market", "Office", "Hospital", "Apartment"
    };

    // Clear old locations just to be safe
    locations.clear();
    adjacencyList.clear();

    // Helper lambda to add locations easily
    auto addLocation = [&](std::string name, std::string type, int x, int y) {
        sampleLocation loc;
        loc.name = name;
        loc.type = type;
        loc.x = x;
        loc.y = y;

        loc.sprite.setTexture(buildingTextures[type]);
        loc.sprite.setPosition(x - 16, y - 16);

        locations.push_back(loc);
    };

    // ------- DISTRICT 1: PARKS (LEFT) -------
    addLocation("Greenwood Park", "Park", 150, 150);
    addLocation("Maple Garden",  "Park", 150, 260);
    addLocation("Playground Park", "Park", 150, 370);
    addLocation("Sunset Park", "Park", 150, 480);
    addLocation("Rose Park", "Park", 150, 590);

    // ------- DISTRICT 2: SCHOOLS (TOP-LEFT) -------
    addLocation("City School A", "School", 350, 120);
    addLocation("City School B", "School", 450, 120);
    addLocation("Bright Future School", "School", 550, 120);
    addLocation("Educators Academy", "School", 650, 120);
    addLocation("Beaconhouse", "School", 750, 120);

    // ------- DISTRICT 3: MARKETS (CENTER) -------
    addLocation("Central Market", "Market", 450, 300);
    addLocation("Fresh Store", "Market", 550, 300);
    addLocation("Daily Mart", "Market", 650, 300);
    addLocation("Super Grocery", "Market", 750, 300);
    addLocation("Trade Center", "Market", 850, 300);

    // ------- DISTRICT 4: OFFICES (CENTER-RIGHT) -------
    addLocation("Tech Hub", "Office", 950, 350);
    addLocation("Finance Tower", "Office", 950, 450);
    addLocation("Business Plaza", "Office", 950, 550);
    addLocation("Corporate Center", "Office", 950, 650);

    // ------- DISTRICT 5: HOSPITALS (TOP-RIGHT) -------
    addLocation("General Hospital", "Hospital", 1100, 150);
    addLocation("City Emergency", "Hospital", 1100, 250);
    addLocation("Heart Care Center", "Hospital", 1100, 350);
    addLocation("Children Hospital", "Hospital", 1100, 450);
    addLocation("Medical Diagnostics", "Hospital", 1100, 550);

    // ------- DISTRICT 6: APARTMENTS (BOTTOM AREA) -------
    addLocation("Skyline Apartments", "Apartment", 500, 400);
    addLocation("Sunrise Apartments", "Apartment", 600, 400);
    addLocation("Lifestyle Residency", "Apartment", 700, 400);
    addLocation("Green Villas", "Apartment", 800, 400);
    addLocation("Golden Heights", "Apartment", 900, 400);

    // ------- Add more apartments to reach 50 -------
    addLocation("City Homes A", "Apartment", 300, 500);
    addLocation("City Homes B", "Apartment", 400, 500);
    addLocation("City Homes C", "Apartment", 500, 500);
    addLocation("City Homes D", "Apartment", 600, 500);
    addLocation("City Homes E", "Apartment", 700, 500);

    addLocation("Elite Residency A", "Apartment", 300, 600);
    addLocation("Elite Residency B", "Apartment", 400, 600);
    addLocation("Elite Residency C", "Apartment", 500, 600);
    addLocation("Elite Residency D", "Apartment", 600, 600);
    addLocation("Elite Residency E", "Apartment", 700, 600);

    // ------- Automatic Adjacency Connections -------
    for (int i = 0; i < locations.size(); i++) {
        // Connect each location with nearby ones (<200 px distance)
        for (int j = 0; j < locations.size(); j++) {
            if (i == j) continue;

            float dx = locations[i].x - locations[j].x;
            float dy = locations[i].y - locations[j].y;
            float dist = sqrt(dx*dx + dy*dy);

            if (dist < 220)  // threshold for "road"
                locations[i].neighbors.push_back(j);
        }
    }

    // Build adjacencyList
    for (int i = 0; i < locations.size(); i++) {
        adjacencyList[i] = locations[i].neighbors;
    }

}

void SampleCity::handleEvent(sf::Event& event, bool& returnToMenu) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
        
        // Close button
        if (closeButton.getGlobalBounds().contains(mouse)) {
            returnToMenu = true;
        }
        
        // Travel button
        else if (travelButton.getGlobalBounds().contains(mouse)) {
            showTravelPopup = true;
            showErrorPopup = false;
            typingStart = typingEnd = false;
        }
        
        // Travel popup handling
        else if (showTravelPopup) {
            // Calculate popup position (centered)
            sf::Vector2f popupSize(400.f, 200.f);
            sf::Vector2f popupPos(400.f, 300.f); // Adjust based on your window size
            
            // Define rectangles
            sf::FloatRect startBoxRect(popupPos.x + 130, popupPos.y + 25, 200.f, 30.f);
            sf::FloatRect endBoxRect(popupPos.x + 130, popupPos.y + 75, 200.f, 30.f);
            sf::FloatRect submitBtnRect(popupPos.x + popupSize.x/2.f - 50.f, popupPos.y + 140, 100.f, 34.f);
            
            // Check for input box focus
            if (startBoxRect.contains(mouse)) {
                typingStart = true;
                typingEnd = false;
            }
            else if (endBoxRect.contains(mouse)) {
                typingStart = false;
                typingEnd = true;
            }
            else if (submitBtnRect.contains(mouse)) {
                // Validate input
                if (startPoint.empty() || endPoint.empty() || 
                    startPoint == endPoint ||
                    findLocationByName(startPoint) == -1 ||
                    findLocationByName(endPoint) == -1) {
                    showErrorPopup = true;
                } else {
                    // Path finding logic here
                    std::cout << "Finding route: " << startPoint << " -> " << endPoint << "\n";
                    showTravelPopup = false;
                    // You might want to show a path popup here
                }
            }
        }
        
        // Error popup close button
        else if (showErrorPopup) {
            sf::Vector2f errorSize(300.f, 120.f);
            sf::Vector2f errorPos(350.f, 300.f); // Adjust based on your window size
            sf::FloatRect closeBtnRect(errorPos.x + errorSize.x/2 - 40.f, errorPos.y + 70.f, 80.f, 30.f);
            
            if (closeBtnRect.contains(mouse)) {
                showErrorPopup = false;
            }
        }
    }
    
    // Handle keyboard text input for travel popup
    if (showTravelPopup && event.type == sf::Event::TextEntered) {
        if (typingStart) {
            if (event.text.unicode == '\b') { // Backspace
                if (!startPoint.empty()) startPoint.pop_back();
            }
            else if (event.text.unicode < 128) {
                startPoint += static_cast<char>(event.text.unicode);
            }
        }
        else if (typingEnd) {
            if (event.text.unicode == '\b') { // Backspace
                if (!endPoint.empty()) endPoint.pop_back();
            }
            else if (event.text.unicode < 128) {
                endPoint += static_cast<char>(event.text.unicode);
            }
        }
    }
    
    // Escape key to close popups
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        if (showTravelPopup) showTravelPopup = false;
        if (showErrorPopup) showErrorPopup = false;
        typingStart = typingEnd = false;
    }
}

void SampleCity::update(sf::RenderWindow& window) {
    // Check mouse position for hover
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    hoveredLocation = -1;

    for (size_t i = 0; i < locations.size(); i++) {
        sf::FloatRect bounds = locations[i].sprite.getGlobalBounds();
        if (bounds.contains((sf::Vector2f)mousePos)) {
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

    // Draw close button
    window.draw(closeButton);
    window.draw(closeText);
    
    // Draw travel button (only if no popups are open)
    if (!showTravelPopup && !showErrorPopup) {
        window.draw(travelButton);
        window.draw(travelText);
    }

    if (hoveredLocation != -1) {
        sf::Text label;
        label.setFont(font);
        label.setString(locations[hoveredLocation].name);
        label.setCharacterSize(10);
        label.setFillColor(sf::Color::Black);
    
        // Position text on the LEFT of the icon
        float offsetX = -20; // space to the left
        float offsetY = -30; // above the icon
    
        // Calculate width to shift background correctly
        sf::FloatRect bounds = label.getLocalBounds();
        label.setPosition(
            locations[hoveredLocation].x + offsetX - bounds.width, // shift left by text width
            locations[hoveredLocation].y + offsetY
        );
    
        // Create background box
        sf::RectangleShape bg;
        bg.setSize(sf::Vector2f(bounds.width + 10, bounds.height + 10));
        bg.setFillColor(sf::Color(255, 255, 255, 220)); // white, slightly transparent
        bg.setPosition(label.getPosition().x - 5, label.getPosition().y - 5);
        bg.setOutlineColor(sf::Color::Black);
        bg.setOutlineThickness(1);
    
        // Draw background then text
        window.draw(bg);
        window.draw(label);
    }
    
    // Create a Graph object from locations for the travel function
    Graph graph;
    
    // Add all locations as nodes to the graph
    for (size_t i = 0; i < locations.size(); i++) {
        const auto& loc = locations[i];
        Location graphLoc;
        graphLoc.id = i;
        graphLoc.name = loc.name;
        graphLoc.type = loc.type;
        graphLoc.pos = sf::Vector2f(loc.x, loc.y);
        graphLoc.active = true;
        graph.addNode(graphLoc);
    }
    
    // Add edges (connections) to the graph based on neighbors
    for (size_t i = 0; i < locations.size(); i++) {
        for (int neighbor : locations[i].neighbors) {
            // Add edge in both directions
            graph.addEdge(i, neighbor);
        }
    }
    
    // Call the travel function to draw popups
    travel(window, graph, showTravelPopup, showErrorPopup, 
        startPoint, endPoint, typingStart, typingEnd,
        currentPath, showPath, window.getDefaultView());
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