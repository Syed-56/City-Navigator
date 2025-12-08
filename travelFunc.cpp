#include "travelFunc.h"
#include "createCity.h"
#include <iostream>
#include <sstream>
#include <queue>
#include <algorithm>

std::vector<int> findPathBFS(const Graph& graph, int startId, int endId) {
    std::vector<int> path;
    
    // Validate input
    if (!graph.validIndex(startId) || !graph.validIndex(endId)) {
        return path;
    }
    if (startId == endId) {
        return {startId};
    }
    
    int n = graph.nodes.size();
    std::vector<bool> visited(n, false);
    std::vector<int> parent(n, -1);
    std::queue<int> q;
    
    q.push(startId);
    visited[startId] = true;
    
    // BFS traversal
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        
        // Check if we reached the destination
        if (current == endId) {
            while (current != -1) {
                path.push_back(current);
                current = parent[current];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        for (int neighbor : graph.adj[current]) {
            if (!visited[neighbor] && graph.nodes[neighbor].active) {
                visited[neighbor] = true;
                parent[neighbor] = current;
                q.push(neighbor);
            }
        }
    }
    
    return path;
}

void highlightPath(sf::RenderWindow& window, const Graph& graph, 
                   const std::vector<int>& path, const sf::View& mapView) {
    
    sf::View originalView = window.getView();
    window.setView(mapView);
    
    for (size_t i = 0; i + 1 < path.size(); i++) {
        int from = path[i];
        int to = path[i + 1];
        
        if (!graph.validIndex(from) || !graph.validIndex(to)) continue;
        
        sf::Vertex line[] = {
            sf::Vertex(graph.nodes[from].pos, sf::Color(255, 100, 100)),  // Red
            sf::Vertex(graph.nodes[to].pos, sf::Color(255, 100, 100))
        };
        
        for (int offset = -2; offset <= 2; offset++) {
            line[0].position = graph.nodes[from].pos + sf::Vector2f(offset, offset);
            line[1].position = graph.nodes[to].pos + sf::Vector2f(offset, offset);
            window.draw(line, 2, sf::Lines);
        }
        
        // Draw the main thick line
        line[0].position = graph.nodes[from].pos;
        line[1].position = graph.nodes[to].pos;
        line[0].color = sf::Color(255, 50, 50);  // Bright red
        line[1].color = sf::Color(255, 50, 50);
        window.draw(line, 2, sf::Lines);
    }
    
    for (size_t i = 0; i < path.size(); i++) {
        int nodeId = path[i];
        if (!graph.validIndex(nodeId)) continue;
        
        const auto& node = graph.nodes[nodeId];
        
        sf::CircleShape glow(20.f);
        glow.setOrigin(20.f, 20.f);
        glow.setPosition(node.pos);
        glow.setFillColor(sf::Color(255, 200, 50, 100));  
        window.draw(glow);
        
        sf::CircleShape highlightedNode(15.f);
        highlightedNode.setOrigin(15.f, 15.f);
        highlightedNode.setPosition(node.pos);
        
        if (i == 0) {  
            highlightedNode.setFillColor(sf::Color(100, 255, 100));  
        } else if (i == path.size() - 1) {  
            highlightedNode.setFillColor(sf::Color(255, 100, 100));  
        } else {
            highlightedNode.setFillColor(sf::Color(255, 255, 100)); 
        }
        
        highlightedNode.setOutlineColor(sf::Color::Black);
        highlightedNode.setOutlineThickness(3.f);
        window.draw(highlightedNode);
        
        // Draw node label
        static sf::Font font;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            font.loadFromFile("Arial.ttf");
            fontLoaded = true;
        }
        
        sf::Text label(node.name, font, 14);
        label.setFillColor(sf::Color::Black);
        
        float offsetY = -35.f - (i * 3.f);  
        label.setPosition(node.pos.x - label.getLocalBounds().width / 2.f, 
                         node.pos.y + offsetY);
        
        sf::RectangleShape labelBg(sf::Vector2f(
            label.getLocalBounds().width + 10.f,
            label.getLocalBounds().height + 5.f));
        labelBg.setFillColor(sf::Color(255, 255, 255, 220));
        labelBg.setOutlineColor(sf::Color::Black);
        labelBg.setOutlineThickness(1.f);
        labelBg.setPosition(label.getPosition().x - 5.f, 
                           label.getPosition().y - 3.f);
        
        window.draw(labelBg);
        window.draw(label);
    }
    
    window.setView(originalView);
}

void travel(sf::RenderWindow& window, const Graph& graph,
    bool& showTravelPopup, bool& showErrorPopup,
    std::string& startPoint, std::string& endPoint,
    bool& typingStart, bool& typingEnd,
    std::vector<int>& currentPath, 
    bool& showPath,               
    const sf::View& mapView)      
{
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (!font.loadFromFile("Arial.ttf")) {
            std::cerr << "ERROR: Could not load Arial.ttf\n";
            return;
        }
        fontLoaded = true;
    }

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

    sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePosI, window.getDefaultView());

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

        if (showPath) {
            sf::RectangleShape closePathBtn({120.f, 34.f});
            closePathBtn.setFillColor(sf::Color(240,160,160));
            closePathBtn.setOutlineColor(sf::Color::Black);
            closePathBtn.setOutlineThickness(1.f);
            closePathBtn.setPosition(popupPos.x + popupSize.x/2.f + 60.f, popupPos.y + 140);
            window.draw(closePathBtn);

            sf::Text closePathText("Clear Path", font, 16);
            closePathText.setFillColor(sf::Color::Black);
            closePathText.setPosition(closePathBtn.getPosition().x + 10, closePathBtn.getPosition().y + 5);
            window.draw(closePathText);
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            sf::Vector2f m = window.mapPixelToCoords(mp, window.getDefaultView());
            
            sf::FloatRect startBoxRect(popupPos.x + 130, popupPos.y + 25, 200.f, 30.f);
            sf::FloatRect endBoxRect(popupPos.x + 130, popupPos.y + 75, 200.f, 30.f);
            sf::FloatRect submitBtnRect(popupPos.x + popupSize.x/2.f - 50.f, popupPos.y + 140, 100.f, 34.f);
            sf::FloatRect closePathBtnRect(popupPos.x + popupSize.x/2.f + 60.f, popupPos.y + 140, 120.f, 34.f);
            
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
                    // Find path
                    auto startIt = graph.name_to_id.find(startPoint);
                    auto endIt = graph.name_to_id.find(endPoint);
                    
                    if (startIt == graph.name_to_id.end() || endIt == graph.name_to_id.end()) {
                        showErrorPopup = true;
                    }
                    
                    int startId = startIt->second;
                    int endId = endIt->second;
                    
                    currentPath = findPathBFS(graph, startId, endId);
                    
                    if (currentPath.empty()) {
                        showErrorPopup = true;
                    } else {
                        // Path found - show it
                        showPath = true;
                        std::cout << "Path found from " << startPoint << " to " << endPoint << ":\n";
                        for (size_t i = 0; i < currentPath.size(); i++) {
                            std::cout << graph.nodes[currentPath[i]].name;
                            if (i < currentPath.size() - 1) std::cout << " -> ";
                        }
                        std::cout << "\n";
                    }
                }
            }
            else if (showPath && closePathBtnRect.contains(m)) {
                // Clear the current path
                currentPath.clear();
                showPath = false;
            }
        }
    }

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

        if (!currentPath.empty() && currentPath.size() == 0) {
            sf::Text noPathText("No path exists between these locations", font, 14);
            noPathText.setFillColor(sf::Color::Black);
            noPathText.setPosition(errorPos.x + 20, errorPos.y + 50);
            window.draw(noPathText);
        }

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

    // Path Highlight
    if (showPath && !currentPath.empty()) {
        highlightPath(window, graph, currentPath, mapView);
        
        sf::Vector2f infoSize(600.f, 80.f);
        sf::Vector2f infoPos(10.f, window.getSize().y - infoSize.y - 10.f);
        
        sf::RectangleShape infoBg(infoSize);
        infoBg.setFillColor(sf::Color(230, 230, 250, 220));
        infoBg.setOutlineColor(sf::Color(100, 100, 200));
        infoBg.setOutlineThickness(2.f);
        infoBg.setPosition(infoPos);
        window.draw(infoBg);
        
        std::string pathInfo = "Path: " + graph.nodes[currentPath[0]].name + 
                              " -> " + graph.nodes[currentPath.back()].name +
                              " (" + std::to_string(currentPath.size()) + " nodes)";
        
        sf::Text infoText(pathInfo, font, 14);
        infoText.setFillColor(sf::Color::Black);
        infoText.setPosition(infoPos.x + 10.f, infoPos.y + 10.f);
        window.draw(infoText);
        
        sf::Text hintText("Click 'Clear Path' in travel popup to remove", font, 12);
        hintText.setFillColor(sf::Color(80, 80, 80));
        hintText.setPosition(infoPos.x + 10.f, infoPos.y + 40.f);
        window.draw(hintText);
    }
}