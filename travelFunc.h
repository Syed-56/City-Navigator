#ifndef TRAVELFUNC_H
#define TRAVELFUNC_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "createCity.h"

// Function to perform BFS pathfinding
std::vector<int> findPathBFS(const Graph& graph, int startId, int endId);

// Function to highlight the path on the graph
void highlightPath(sf::RenderWindow& window, const Graph& graph, 
                   const std::vector<int>& path, const sf::View& mapView);

// Main travel function
void travel(sf::RenderWindow& window, const Graph& graph,
    bool& showTravelPopup, bool& showErrorPopup,
    std::string& startPoint, std::string& endPoint,
    bool& typingStart, bool& typingEnd,
    std::vector<int>& currentPath, // Added: store current path
    bool& showPath,               // Added: flag to show/highlight path
    const sf::View& mapView);     // Added: need map view for drawing

#endif