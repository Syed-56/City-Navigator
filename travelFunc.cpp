#include "travelFunc.h"
#include "createCity.h"
#include <iostream>
#include <sstream>

void travel(sf::RenderWindow& window, const Graph& graph,
    bool& showTravelPopup, bool& showErrorPopup,
    std::string& startPoint, std::string& endPoint,
    bool& typingStart, bool& typingEnd)
{
    // ---------- Load Font ----------
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("Arial.ttf")) {
            std::cerr << "ERROR: Could not load Arial.ttf\n";
            return;
        }
        fontLoaded = true;
    }

    // ---------- Helper lambdas ----------
    auto hasNode = [&](const std::string& name){
        return graph.name_to_id.find(name) != graph.name_to_id.end();
    };
    
    auto hasNeighbours = [&](const std::string& name){
        auto it = graph.name_to_id.find(name);
        if(it == graph.name_to_id.end()) return false;
        int id = it->second;
        if(!graph.validIndex(id)) return false;
        return !graph.adj[id].empty();
    };

    // ---------- Mouse position (in screen coordinates) ----------
    sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePosI, window.getDefaultView());

    // ---------- Travel Popup ----------
    if (showTravelPopup) {
        sf::Vector2f popupSize(400.f, 200.f);
        sf::Vector2f popupPos((window.getSize().x - popupSize.x)/2.f,
                            (window.getSize().y - popupSize.y)/2.f);
        
        sf::RectangleShape popupBg(popupSize);
        popupBg.setFillColor(sf::Color(230,230,230));
        popupBg.setOutlineColor(sf::Color::Black);
        popupBg.setOutlineThickness(2.f);
        popupBg.setPosition(popupPos);
        window.draw(popupBg);

        // Labels
        sf::Text startLabel("Start Point:", font, 16);
        startLabel.setFillColor(sf::Color::Black);
        startLabel.setPosition(popupPos.x + 20, popupPos.y + 30);
        window.draw(startLabel);

        sf::Text endLabel("End Point:", font, 16);
        endLabel.setFillColor(sf::Color::Black);
        endLabel.setPosition(popupPos.x + 20, popupPos.y + 80);
        window.draw(endLabel);

        // Input boxes
        sf::RectangleShape startBox({200.f, 30.f});
        startBox.setPosition(popupPos.x + 130, popupPos.y + 25);
        startBox.setFillColor(typingStart ? sf::Color(255,255,210) : sf::Color::White);
        startBox.setOutlineColor(sf::Color::Black);
        startBox.setOutlineThickness(1.f);
        window.draw(startBox);

        sf::RectangleShape endBox({200.f, 30.f});
        endBox.setPosition(popupPos.x + 130, popupPos.y + 75);
        endBox.setFillColor(typingEnd ? sf::Color(255,255,210) : sf::Color::White);
        endBox.setOutlineColor(sf::Color::Black);
        endBox.setOutlineThickness(1.f);
        window.draw(endBox);

        // Input Text
        sf::Text startText(startPoint, font, 16);
        startText.setFillColor(sf::Color::Black);
        startText.setPosition(startBox.getPosition().x + 5, startBox.getPosition().y + 5);
        window.draw(startText);

        sf::Text endText(endPoint, font, 16);
        endText.setFillColor(sf::Color::Black);
        endText.setPosition(endBox.getPosition().x + 5, endBox.getPosition().y + 5);
        window.draw(endText);

        // Submit Button
        sf::RectangleShape submitBtn({100.f, 34.f});
        submitBtn.setFillColor(sf::Color(160,240,160));
        submitBtn.setOutlineColor(sf::Color::Black);
        submitBtn.setOutlineThickness(1.f);
        submitBtn.setPosition(popupPos.x + popupSize.x/2.f - 50.f, popupPos.y + 140);
        window.draw(submitBtn);

        sf::Text submitText("Submit", font, 16);
        submitText.setFillColor(sf::Color::Black);
        submitText.setPosition(submitBtn.getPosition().x + 15, submitBtn.getPosition().y + 5);
        window.draw(submitText);

        // Handle mouse clicks for the popup
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            sf::Vector2f m = window.mapPixelToCoords(mp, window.getDefaultView());
            
            // Check for input box focus
            sf::FloatRect startBoxRect(popupPos.x + 130, popupPos.y + 25, 200.f, 30.f);
            sf::FloatRect endBoxRect(popupPos.x + 130, popupPos.y + 75, 200.f, 30.f);
            sf::FloatRect submitBtnRect(popupPos.x + popupSize.x/2.f - 50.f, popupPos.y + 140, 100.f, 34.f);
            
            if (startBoxRect.contains(m)) {
                typingStart = true;
                typingEnd = false;
            }
            else if (endBoxRect.contains(m)) {
                typingStart = false;
                typingEnd = true;
            }
            else if (submitBtnRect.contains(m)) {
                // Validate input
                if (startPoint.empty() || endPoint.empty()) {
                    showErrorPopup = true;
                }
                else if (!hasNode(startPoint) || !hasNode(endPoint)) {
                    showErrorPopup = true;
                }
                else if (!hasNeighbours(startPoint) || !hasNeighbours(endPoint)) {
                    showErrorPopup = true;
                }
                else if (startPoint == endPoint) {
                    showErrorPopup = true;
                }
                else {
                    showTravelPopup = false;
                    std::cout << "Route: " << startPoint << " -> " << endPoint << "\n";
                    startPoint = "";
                    endPoint = "";
                }
            }
        }
    }

    // ---------- Error Popup ----------
    if (showErrorPopup) {
        sf::Vector2f errorSize(300.f,120.f);
        sf::Vector2f errorPos((window.getSize().x-errorSize.x)/2.f,
                            (window.getSize().y-errorSize.y)/2.f);
        
        sf::RectangleShape errorBg(errorSize);
        errorBg.setFillColor(sf::Color(250,180,180));
        errorBg.setOutlineColor(sf::Color::Black);
        errorBg.setOutlineThickness(2.f);
        errorBg.setPosition(errorPos);
        window.draw(errorBg);

        sf::Text errorText("Error: Invalid Locations", font, 16);
        errorText.setFillColor(sf::Color::Black);
        errorText.setPosition(errorPos.x + 20, errorPos.y + 30);
        window.draw(errorText);

        // Close Button
        sf::RectangleShape closeBtn({80.f,30.f});
        closeBtn.setFillColor(sf::Color(200,100,100));
        closeBtn.setOutlineColor(sf::Color::Black);
        closeBtn.setOutlineThickness(1.f);
        closeBtn.setPosition(errorPos.x + errorSize.x/2 - 40.f, errorPos.y + 70.f);
        window.draw(closeBtn);

        sf::Text closeText("Close", font, 16);
        closeText.setFillColor(sf::Color::White);
        closeText.setPosition(closeBtn.getPosition().x + 18, closeBtn.getPosition().y + 5);
        window.draw(closeText);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            sf::Vector2f m = window.mapPixelToCoords(mp, window.getDefaultView());
            
            sf::FloatRect closeBtnRect(errorPos.x + errorSize.x/2 - 40.f, errorPos.y + 70.f, 80.f, 30.f);
            if (closeBtnRect.contains(m)) {
                showErrorPopup = false;
            }
        }
    }
}