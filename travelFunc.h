#ifndef TRAVELFUNC_H
#define TRAVELFUNC_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "createCity.h"

void travel(sf::RenderWindow& window, const Graph& graph,
    bool& showTravelPopup, bool& showErrorPopup,
    std::string& startPoint, std::string& endPoint,
    bool& typingStart, bool& typingEnd);
#endif