#ifndef TRAVELFUNC_H
#define TRAVELFUNC_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "createCity.h"

std::vector<int> findPathBFS(const Graph& graph, int startId, int endId);

void highlightPath(sf::RenderWindow& window, const Graph& graph, 
                   const std::vector<int>& path, const sf::View& mapView);

void travel(sf::RenderWindow& window, const Graph& graph,
    bool& showTravelPopup, bool& showErrorPopup,
    std::string& startPoint, std::string& endPoint,
    bool& typingStart, bool& typingEnd,
    std::vector<int>& currentPath, 
    bool& showPath,               
    const sf::View& mapView);     

#endif