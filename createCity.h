#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <string>
#include <cmath>
#include <sstream>
#include <chrono>
#include "sampleCity.h"


// ------------------ Data structures ------------------

struct Location {
    int id;
    std::string name;
    std::string type;
    sf::Vector2f pos;
    bool active = true; // inactive when undone (soft deletion)
};

class Graph {
public:
    std::vector<Location> nodes;
    std::vector<std::vector<int>> adj; // adjacency lists (store neighbor ids)
    std::unordered_map<std::string, int> name_to_id;

    int addNode(const Location& loc);
    bool addEdge(int a, int b);
    bool removeEdge(int a, int b);
    void deactivateNode(int id);

    void activateNode(const Location& locCopy);

    bool validIndex(int i) const;
};

// ------------------ Action + History (stack) ------------------

enum class ActKind { AddNode, Connect };

struct Action {
    ActKind kind;
    std::string description;      // human readable
    // payload
    Location nodeSnapshot;        // used for AddNode (snapshot of the node)
    int a = -1, b = -1;           // used for Connect (node ids)
    std::shared_ptr<Action> next; // for linked-list stack
    Action(ActKind k) : kind(k), description(""), nodeSnapshot({}), a(-1), b(-1), next(nullptr) {}
};

class ActionHistory {
public:
    std::shared_ptr<Action> head;
    void push(const std::shared_ptr<Action>& act);
    // pop and return the top action; returns nullptr if empty
    std::shared_ptr<Action> pop();
    std::shared_ptr<Action> peek() const;
    std::vector<std::string> toVector() const;
    int size() const;
    void clear();
};

bool loadFontRobust(sf::Font& font);
bool pointInCircle(const sf::Vector2f& p, const sf::Vector2f& c, float r);
std::string formatIntPair(int x, int y);
void createCity(sf::RenderWindow& window);