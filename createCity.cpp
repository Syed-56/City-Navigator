#include "createCity.h"
#include "travelFunc.h"
#include <vector>
#include <utility>

bool loadFontRobust(sf::Font& font) {
    if (font.loadFromFile("arial.ttf")) return true;
    if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) return true;
    return false;
}

bool pointInCircle(const sf::Vector2f& p, const sf::Vector2f& c, float r) {
    float dx = p.x - c.x, dy = p.y - c.y;
    return dx * dx + dy * dy <= r * r;
}

std::string formatIntPair(int x, int y) {
    std::ostringstream ss;
    ss << "(" << x << "," << y << ")";
    return ss.str();
}

// ------------------ Graph methods ------------------

int Graph::addNode(const Location& loc) {
    int id = nodes.size();
    Location copy = loc;
    copy.id = id;
    copy.active = true;
    nodes.push_back(copy);
    if ((int)adj.size() <= id) adj.resize(id + 1);
    name_to_id[copy.name] = id;
    return id;
}

bool Graph::addEdge(int a, int b) {
    if (!validIndex(a) || !validIndex(b)) return false;
    if (a == b) return false;
    // prevent duplicates
    if (std::find(adj[a].begin(), adj[a].end(), b) == adj[a].end()) adj[a].push_back(b);
    if (std::find(adj[b].begin(), adj[b].end(), a) == adj[b].end()) adj[b].push_back(a);
    return true;
}

bool Graph::removeEdge(int a, int b) {
    if (!validIndex(a) || !validIndex(b)) return false;
    auto it = std::find(adj[a].begin(), adj[a].end(), b);
    if (it != adj[a].end()) adj[a].erase(it);
    it = std::find(adj[b].begin(), adj[b].end(), a);
    if (it != adj[b].end()) adj[b].erase(it);
    return true;
}

void Graph::deactivateNode(int id) {
    if (!validIndex(id)) return;
    nodes[id].active = false;
    // remove edges
    for (int nb : adj[id]) {
        auto it = std::find(adj[nb].begin(), adj[nb].end(), id);
        if (it != adj[nb].end()) adj[nb].erase(it);
    }
    adj[id].clear();
}

void Graph::activateNode(const Location& locCopy) {
    
    // If the node already exists (same id) and within range, just reactivate and restore basic info.
    if (locCopy.id >= 0 && locCopy.id < (int)nodes.size()) {
        nodes[locCopy.id] = locCopy;
        nodes[locCopy.id].active = true;
        if ((int)adj.size() <= locCopy.id) adj.resize(locCopy.id + 1);
    }
    else {
        // If adding new beyond current size (shouldn't normally happen), append.
        addNode(locCopy);
    }
    name_to_id[locCopy.name] = locCopy.id;
}

bool Graph::validIndex(int i) const {
    return i >= 0 && i < (int)nodes.size();
}

// ------------------ ActionHistory methods ------------------

void ActionHistory::push(const std::shared_ptr<Action>& act) {
    act->next = head;
    head = act;
}

std::shared_ptr<Action> ActionHistory::pop() {
    if (!head) return nullptr;
    auto top = head;
    head = head->next;
    top->next = nullptr;
    return top;
}

std::shared_ptr<Action> ActionHistory::peek() const {
    return head;
}

std::vector<std::string> ActionHistory::toVector() const {
    std::vector<std::string> out;
    auto cur = head;
    while (cur) {
        out.push_back(cur->description);
        cur = cur->next;
    }
    return out;
}

int ActionHistory::size() const {
    int c = 0;
    for (auto cur = head; cur; cur = cur->next) ++c;
    return c;
}

void ActionHistory::clear() {
    head = nullptr;
}

int hoveredNode = -1;
void createCity(sf::RenderWindow& window) {
    const unsigned int WINDOW_W = 1000, WINDOW_H = 700;                      // top-left corner with 10px padding
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        return;
    }

    if (!loadFontRobust(font)) {
        std::cerr << "ERROR: Could not load a font. Place arial.ttf or DejaVuSans.ttf next to the executable.\n";
        return;
    }

    Graph graph;
    ActionHistory undoStack; // actions to undo (top = most recent)
    ActionHistory redoStack; // actions that were undone (for redo)

    // UI: + button
    sf::CircleShape addBtn(22.f);
    addBtn.setFillColor(sf::Color(170, 190, 255));
    addBtn.setOutlineColor(sf::Color::Black);
    addBtn.setOutlineThickness(2.f);
    sf::Vector2u windowSize = window.getSize(); // get window width & height
    addBtn.setPosition(10.f, window.getSize().y - 2*addBtn.getRadius() - 10.f);
    sf::Text plusText("+", font, 28);
    plusText.setFillColor(sf::Color::Black);
    plusText.setPosition(addBtn.getPosition().x + 12.f, addBtn.getPosition().y - 6.f);

    // ---------- Travel Button ----------
    const float btnWidth = 120.f, btnHeight = 40.f;
    const float travelBtnX = 250.f, travelBtnY = 720.f; // Fixed position to be visible
    sf::RectangleShape travelBtn(sf::Vector2f(btnWidth, btnHeight));
    travelBtn.setPosition(travelBtnX, travelBtnY);
    travelBtn.setFillColor(sf::Color(70, 130, 180));
    travelBtn.setOutlineThickness(2);
    travelBtn.setOutlineColor(sf::Color::Black);

    sf::Text travelText("Travel", font, 18);
    travelText.setFillColor(sf::Color::White);
    travelText.setPosition(travelBtnX + 18, travelBtnY + 6);

    // Popup state
    bool showAddPopup = false;
    bool showConnectPopup = false;

    // One-time rule: after first node added, show a modal asking to add another before connecting
    bool firstNodeNoticePending = false; // triggered immediately after first node add
    bool firstNodeNoticeShownAlready = false; // ensure it happens only once

    // Add popup fields
    std::vector<std::string> buildingTypes = {"Apartment", "School", "Office", "Park", "Hospital"};
    int typeIndex = 0;
    bool typeDropdownOpen = false;
    std::string inputName = "";
    std::string inputX = "00";
    std::string inputY = "00";
    bool typingName = false, typingX = false, typingY = false;
    std::vector<int> currentPath;
    bool showPath = false;

    // Connect popup fields
    int connectA = -1, connectB = -1;
    bool dropAOpen = false, dropBOpen = false;

    // Popup rectangles
    sf::RectangleShape popupBg(sf::Vector2f(420.f, 260.f));
    popupBg.setFillColor(sf::Color(230, 230, 230));
    popupBg.setOutlineColor(sf::Color::Black);
    popupBg.setOutlineThickness(2.f);
    popupBg.setPosition((WINDOW_W - popupBg.getSize().x) / 2.f, (WINDOW_H - popupBg.getSize().y) / 2.f);

    sf::RectangleShape connectBg(sf::Vector2f(420.f, 220.f));
    connectBg.setFillColor(sf::Color(230, 230, 230));
    connectBg.setOutlineColor(sf::Color::Black);
    connectBg.setOutlineThickness(2.f);
    connectBg.setPosition((WINDOW_W - connectBg.getSize().x) / 2.f, (WINDOW_H - connectBg.getSize().y) / 2.f + 20.f);

    // Buttons
    sf::RectangleShape submitBtn(sf::Vector2f(100.f, 34.f));
    submitBtn.setFillColor(sf::Color(160, 240, 160));
    submitBtn.setOutlineColor(sf::Color::Black);
    submitBtn.setOutlineThickness(1.f);

    sf::RectangleShape cancelBtn(sf::Vector2f(100.f, 34.f));
    cancelBtn.setFillColor(sf::Color(240, 160, 160));
    cancelBtn.setOutlineColor(sf::Color::Black);
    cancelBtn.setOutlineThickness(1.f);

    // ---------- Travel popup state ----------
    bool showTravelPopup = false;
    bool showErrorPopup = false;
    std::string startPoint = "";
    std::string endPoint = "";
    bool typingStart = false;
    bool typingEnd = false;

    // Map view / pan
    sf::View mapView = window.getDefaultView();
    bool panning = false;
    sf::Vector2f lastMouse;

    const float gridSize = 50.f;

    // ---------------- Floating overlay panel (right) ----------------
    const float panelW = 320.f;
    const float panelH = (float)WINDOW_H - 80.f;
    // visible x and hidden x (off-screen right)
    float panelXVisible = WINDOW_W - panelW - 20.f;
    float panelXHidden = WINDOW_W + 10.f;
    float panelX = panelXHidden; // start hidden
    float panelY = 40.f;
    bool panelVisible = false;
    bool panelAnimating = false;
    float panelSpeed = 900.f; // pixels per second for sliding

    // UI elements inside panel (using UI coordinates)
    sf::RectangleShape panelBg(sf::Vector2f(panelW, panelH));
    panelBg.setFillColor(sf::Color(40, 40, 40, 220));
    panelBg.setOutlineColor(sf::Color(80, 80, 80));
    panelBg.setOutlineThickness(1.f);
    panelBg.setPosition(panelX, panelY);

    sf::Text panelTitle("History (Actions)", font, 18);
    panelTitle.setFillColor(sf::Color::White);
    panelTitle.setPosition(panelX + 14.f, panelY + 12.f);

    // Panel buttons (Undo, Redo, Clear, Toggle)
    sf::RectangleShape btnUndo(sf::Vector2f(70.f, 28.f));
    sf::RectangleShape btnRedo(sf::Vector2f(70.f, 28.f));
    sf::RectangleShape btnClear(sf::Vector2f(70.f, 28.f));
    sf::RectangleShape btnClose(sf::Vector2f(34.f, 28.f));

    btnUndo.setFillColor(sf::Color(180, 180, 180));
    btnRedo.setFillColor(sf::Color(180, 180, 180));
    btnClear.setFillColor(sf::Color(180, 180, 180));
    btnClose.setFillColor(sf::Color(200, 100, 100));

    // Load textures once, outside the main loop
    std::map<std::string, sf::Texture> nodeTextures;
    for (const auto& type : buildingTypes) {
        sf::Texture tex;
        if (tex.loadFromFile("images/" + type + ".png")) {
            nodeTextures[type] = tex;
        }
    }

    // For timing delta
    sf::Clock clock;
    std::vector<std::pair<float, float>> occupiedLocs;

    // main loop
    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaSeconds = dt.asSeconds();

        sf::Event ev;
        while (window.pollEvent(ev)) {
            bool dropdownClickConsumed = false;
            if (ev.type == sf::Event::Closed) window.close();

            // Escape key to close popups
            if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape) {
                if (showAddPopup) showAddPopup = false;
                if (showConnectPopup) showConnectPopup = false;
                if (showTravelPopup) showTravelPopup = false;
                if (showErrorPopup) showErrorPopup = false;
                typeDropdownOpen = false;
                dropAOpen = dropBOpen = false;
                typingName = typingX = typingY = typingStart = typingEnd = false;
            }

            // Keyboard: text input for add popup
            if (ev.type == sf::Event::TextEntered && showAddPopup) {
                if (typingName) {
                    if (ev.text.unicode == 8) { if (!inputName.empty()) inputName.pop_back(); }
                    else if (ev.text.unicode >= 32 && ev.text.unicode < 128) inputName += (char)ev.text.unicode;
                }
                else if (typingX) {
                    if (ev.text.unicode == 8) { if (!inputX.empty()) inputX.pop_back(); }
                    else if ((ev.text.unicode >= '0' && ev.text.unicode <= '9') || ev.text.unicode == '.' || ev.text.unicode == '-') inputX += (char)ev.text.unicode;
                }
                else if (typingY) {
                    if (ev.text.unicode == 8) { if (!inputY.empty()) inputY.pop_back(); }
                    else if ((ev.text.unicode >= '0' && ev.text.unicode <= '9') || ev.text.unicode == '.' || ev.text.unicode == '-') inputY += (char)ev.text.unicode;
                }
            }

            // Keyboard: text input for travel popup
            if (ev.type == sf::Event::TextEntered && showTravelPopup) {
                if (typingStart) {
                    if (ev.text.unicode == 8) { 
                        if (!startPoint.empty()) startPoint.pop_back(); 
                    }
                    else if (ev.text.unicode >= 32 && ev.text.unicode < 128) {
                        startPoint += static_cast<char>(ev.text.unicode);
                    }
                }
                else if (typingEnd) {
                    if (ev.text.unicode == 8) { 
                        if (!endPoint.empty()) endPoint.pop_back(); 
                    }
                    else if (ev.text.unicode >= 32 && ev.text.unicode < 128) {
                        endPoint += static_cast<char>(ev.text.unicode);
                    }
                }
            }

            // Mouse pressed
            if (ev.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouseScreen((float)ev.mouseButton.x, (float)ev.mouseButton.y);
                sf::Vector2f mouseWorld = window.mapPixelToCoords({ ev.mouseButton.x, ev.mouseButton.y });
            
                // Travel button click check
                sf::Vector2f travelBtnPos(250.f, 720.f);
                sf::FloatRect travelBtnRect(travelBtnPos, sf::Vector2f(120.f, 40.f));
                if (travelBtnRect.contains(mouseScreen) && !showTravelPopup && !showErrorPopup) {
                    showTravelPopup = true;
                    typingStart = true;
                    typingEnd = false;
                    continue;
                }
            
                // Middle -> pan start
                if (ev.mouseButton.button == sf::Mouse::Middle) {
                    panning = true;
                    lastMouse = mouseScreen;
                }

                // Panel UI gets priority when visible or animating
                float currentPanelX = panelX;
                float currentPanelY = panelY;
                sf::FloatRect panelRect(currentPanelX, currentPanelY, panelW, panelH);
                if (panelRect.contains(mouseScreen)) {
                    // compute button positions relative to panel
                    panelBg.setPosition(currentPanelX, currentPanelY);

                    btnUndo.setPosition(currentPanelX + 14.f, currentPanelY + 44.f);
                    btnRedo.setPosition(currentPanelX + 14.f + 80.f, currentPanelY + 44.f);
                    btnClear.setPosition(currentPanelX + 14.f + 160.f, currentPanelY + 44.f);
                    btnClose.setPosition(currentPanelX + panelW - 44.f, currentPanelY + 12.f);

                    // If clicked Undo
                    if (btnUndo.getGlobalBounds().contains(mouseScreen)) {
                        auto act = undoStack.pop();
                        if (act) {
                            if (act->kind == ActKind::AddNode) {
                                int nid = act->nodeSnapshot.id;
                                if (graph.validIndex(nid) && graph.nodes[nid].active) {
                                    graph.deactivateNode(nid);
                                }
                            }
                            else if (act->kind == ActKind::Connect) {
                                graph.removeEdge(act->a, act->b);
                            }
                            redoStack.push(act);
                        }
                    }
                    // Redo
                    else if (btnRedo.getGlobalBounds().contains(mouseScreen)) {
                        auto act = redoStack.pop();
                        if (act) {
                            if (act->kind == ActKind::AddNode) {
                                graph.activateNode(act->nodeSnapshot);
                            }
                            else if (act->kind == ActKind::Connect) {
                                graph.addEdge(act->a, act->b);
                            }
                            undoStack.push(act);
                        }
                    }
                    // Clear history (both stacks)
                    else if (btnClear.getGlobalBounds().contains(mouseScreen)) {
                        undoStack.clear();
                        redoStack.clear();
                    }
                    // Close panel
                    else if (btnClose.getGlobalBounds().contains(mouseScreen)) {
                        return;
                    }
                    else {
                        // Click inside list region: we could implement selecting entries; for now do nothing
                    }

                    continue; // skip other scene clicks
                }

                // If add popup open, handle its UI first
                if (showAddPopup) {
                    sf::Vector2f pb = popupBg.getPosition();
                    sf::FloatRect typeRect(pb.x + 20.f, pb.y + 40.f, 200.f, 30.f);
                    sf::FloatRect nameRect(pb.x + 20.f, pb.y + 90.f, 380.f, 30.f);
                    sf::FloatRect xRect(pb.x + 20.f, pb.y + 140.f, 160.f, 30.f);
                    sf::FloatRect yRect(pb.x + 210.f, pb.y + 140.f, 160.f, 30.f);
                    submitBtn.setPosition(pb.x + 220.f, pb.y + 200.f);
                    cancelBtn.setPosition(pb.x + 60.f, pb.y + 200.f);

                    // Click inside typeRect toggles dropdown
                    if (typeRect.contains(mouseScreen)) {
                        typeDropdownOpen = !typeDropdownOpen;
                    }
                    // Click on name/x/y fields toggles typing
                    else if (nameRect.contains(mouseScreen)) {
                        typingName = true; typingX = typingY = false;
                        typeDropdownOpen = false;
                    }
                    else if (xRect.contains(mouseScreen)) {
                        typingX = true; typingName = typingY = false;
                        typeDropdownOpen = false;
                    }
                    else if (yRect.contains(mouseScreen)) {
                        typingY = true; typingName = typingX = false;
                        typeDropdownOpen = false;
                    }
                    // Click on Submit
                    else if (submitBtn.getGlobalBounds().contains(mouseScreen)) {
                        // Parse coordinates (with fallback)
                        float xx = 200.f, yy = 200.f;
                        try { xx = std::stof(inputX); }
                        catch (...) { xx = 200.f; }
                        try { yy = std::stof(inputY); }
                        catch (...) { yy = 200.f; }

                        // ---- BOUNDS CHECK HERE ----
                        const float MIN_X = 15.f;
                        const float MAX_X = 975.f;      // or ANY custom number
                        const float MIN_Y = 15.f;
                        const float MAX_Y = 700.f;

                        if (xx < MIN_X || xx > MAX_X || yy < MIN_Y || yy > MAX_Y) {
                            // show popup, print message, or ignore silently
                            std::cout << "Coordinates out of bounds — node not added.\n";
                            continue;   // IMPORTANT: stop processing Submit
                        }

                        bool canPlace = true;  // assume we can place

                        for (int i = 0; i < occupiedLocs.size(); i++) {
                            if (abs(occupiedLocs[i].first - xx) < 50 && abs(occupiedLocs[i].second - yy) < 50) {
                                std::cout << "Location too close to an existing one\n";
                                canPlace = false; // prevent placement
                                break;
                            }
                        }

                        // push only if location is valid
                        if (canPlace) {
                            occupiedLocs.push_back({xx, yy});
                            Location loc;
                            loc.id = graph.nodes.size();
                            loc.name = inputName.empty() ? ("Loc" + std::to_string(loc.id)) : inputName;
                            loc.type = buildingTypes[typeIndex];
                            loc.pos = { xx, yy };
                            loc.active = true;
                            int newId = graph.addNode(loc);

                            // Create structured action and push to undo stack
                            auto act = std::make_shared<Action>(ActKind::AddNode);
                            act->nodeSnapshot = loc;         // snapshot so we can restore on redo
                            std::ostringstream ds;
                            ds << "Added: " << loc.type << " '" << loc.name << "' at (" << (int)xx << "," << (int)yy << ")";
                            act->description = ds.str();
                            undoStack.push(act);
                            // New action invalidates redo stack
                            redoStack.clear();

                            // One-time rule: if graph now has exactly 1 node, show the modal and DO NOT open connect popup.
                            if (graph.nodes.size() == 1 && !firstNodeNoticeShownAlready) {
                                firstNodeNoticePending = true; // will be shown below in render
                                firstNodeNoticeShownAlready = true;
                                showConnectPopup = false;
                            }
                            else {
                                // Normal: open connect popup for the newly added node
                                showConnectPopup = true;
                                connectA = newId;
                                connectB = -1;
                            }

                            // Reset add popup fields/state
                            showAddPopup = false;
                            typeDropdownOpen = false;
                            typingName = typingX = typingY = false;
                            inputName = "";
                            inputX = inputY = "200";
                            typeIndex = 0;
                        }

                    }
                    // Cancel
                    else if (cancelBtn.getGlobalBounds().contains(mouseScreen)) {
                        showAddPopup = false;
                        typeDropdownOpen = false;
                        typingName = typingX = typingY = false;
                    }
                    else {
                        // If clicked outside add popup fields: close type dropdown and stop typing
                        typingName = typingX = typingY = false;

                        // If click was inside dropdown area while open, handle selection
                        if (typeDropdownOpen) {
                            // dropdown rect extends under typeRect
                            float itemH = 28.f;
                            sf::FloatRect dropdownRect(typeRect.left, typeRect.top + typeRect.height, typeRect.width, itemH * (float)buildingTypes.size());
                            if (dropdownRect.contains(mouseScreen)) {
                                int idx = (int)((mouseScreen.y - dropdownRect.top) / itemH);
                                if (idx >= 0 && idx < (int)buildingTypes.size()) {
                                    typeIndex = idx;
                                }
                                typeDropdownOpen = false;
                            }
                            else {
                                // clicked outside dropdown -> close it
                                typeDropdownOpen = false;
                            }
                        }
                    }

                    continue; // done processing this mouse press
                } // end showAddPopup handling

                // If connect popup open, handle its UI first
                if (showConnectPopup) {
                    sf::Vector2f cb = connectBg.getPosition();
                    sf::FloatRect aRect(cb.x + 20.f, cb.y + 40.f, 380.f, 30.f);
                    sf::FloatRect bRect(cb.x + 20.f, cb.y + 90.f, 380.f, 30.f);
                    submitBtn.setPosition(cb.x + 220.f, cb.y + 140.f);
                    cancelBtn.setPosition(cb.x + 60.f, cb.y + 140.f);

                    // Click on From/To toggles dropdowns
                    if (aRect.contains(mouseScreen)) {
                        dropAOpen = !dropAOpen;
                        dropBOpen = false;
                    }
                    else if (bRect.contains(mouseScreen)) {
                        dropBOpen = !dropBOpen;
                        dropAOpen = false;
                    }
                    // Submit (Connect)
                    else if (submitBtn.getGlobalBounds().contains(mouseScreen)) {
                        if (connectA >= 0 && connectB >= 0 && connectA != connectB && connectA < (int)graph.nodes.size() && connectB < (int)graph.nodes.size()) {
                            graph.addEdge(connectA, connectB);

                            // push structured action
                            auto act = std::make_shared<Action>(ActKind::Connect);
                            act->a = connectA; act->b = connectB;
                            std::ostringstream ds;
                            ds << "Connected '" << graph.nodes[connectA].name << "' <-> '" << graph.nodes[connectB].name << "'";
                            act->description = ds.str();
                            undoStack.push(act);
                            redoStack.clear();
                        }
                        showConnectPopup = false;
                        dropAOpen = dropBOpen = false;
                    }
                    else if (cancelBtn.getGlobalBounds().contains(mouseScreen)) {
                        showConnectPopup = false;
                        dropAOpen = dropBOpen = false;
                    }
                    else {
                        // if a dropdown is open and click within its area, select item
                        if (dropAOpen) {
                            float itemH = 28.f;
                            sf::FloatRect dropdownRect(aRect.left, aRect.top + aRect.height, aRect.width, itemH * (float)graph.nodes.size());
                            if (dropdownRect.contains(mouseScreen)) {
                                int idx = (int)((mouseScreen.y - dropdownRect.top) / itemH);
                                if (idx >= 0 && idx < (int)graph.nodes.size()) {
                                    connectA = idx;
                                }
                                dropAOpen = false;
                            }
                            else {
                                dropAOpen = false;
                            }
                        }
                        else if (dropBOpen) {
                            float itemH = 28.f;
                            sf::FloatRect dropdownRect(bRect.left, bRect.top + bRect.height, bRect.width, itemH * (float)graph.nodes.size());
                            if (dropdownRect.contains(mouseScreen)) {
                                int idx = (int)((mouseScreen.y - dropdownRect.top) / itemH);
                                if (idx >= 0 && idx < (int)graph.nodes.size()) {
                                    connectB = idx;
                                }
                                dropBOpen = false;
                            }
                            else {
                                dropBOpen = false;
                            }
                        }
                    }

                    continue; // skip other scene clicks
                } // end showConnectPopup handling

                // No popups active:
                // Check + button (screen coords)
                if (addBtn.getGlobalBounds().contains(mouseScreen)) {
                    showAddPopup = true;
                    // default values
                    typeIndex = 0;
                    typeDropdownOpen = false;
                    inputName = "";
                    inputX = "200";
                    inputY = "200";
                    typingName = typingX = typingY = false;
                    continue;
                }

                // Click on a node (world coords) should open connect popup with that node as From
                if (ev.mouseButton.button == sf::Mouse::Left) {
                    for (auto& loc : graph.nodes) {
                        if (!loc.active) continue;
                        if (pointInCircle(mouseWorld, loc.pos, 12.f)) {
                            // If graph size is 1 and firstNodeNotice hasn't been shown before -> show modal instead
                            if (graph.nodes.size() == 1 && !firstNodeNoticeShownAlready) {
                                firstNodeNoticePending = true;
                                firstNodeNoticeShownAlready = true;
                                showConnectPopup = false;
                            }
                            else {
                                showConnectPopup = true;
                                connectA = loc.id;
                                connectB = -1;
                                dropAOpen = dropBOpen = false;
                            }
                            break;
                        }
                    }
                }
            } // end MouseButtonPressed

            // Mouse release
            if (ev.type == sf::Event::MouseButtonReleased) {
                if (ev.mouseButton.button == sf::Mouse::Middle) panning = false;
            }

            // Mouse move for panning
            if (ev.type == sf::Event::MouseMoved) {
                sf::Vector2f curr((float)ev.mouseMove.x, (float)ev.mouseMove.y);
                if (panning) {
                    sf::Vector2f delta = lastMouse - curr;
                    mapView.move(delta);
                    window.setView(mapView);
                    lastMouse = curr;
                }
            }
        } // end event polling

        // ----------------- Panel animation -----------------
        if (panelAnimating) {
            float targetX = panelVisible ? panelXVisible : panelXHidden;
            if (std::abs(panelX - targetX) < 2.f) {
                panelX = targetX;
                panelAnimating = false;
            }
            else {
                float dir = (targetX > panelX) ? 1.f : -1.f;
                panelX += dir * panelSpeed * deltaSeconds;
                // clamp
                if ((dir > 0 && panelX > targetX) || (dir < 0 && panelX < targetX)) panelX = targetX;
            }
        }

        // ----------------- Rendering -----------------
        window.clear(sf::Color(210, 210, 210));
        window.setView(mapView);

        // Draw grid
        sf::Vector2f viewTL = window.mapPixelToCoords({ 0,0 });
        sf::Vector2f viewBR = window.mapPixelToCoords({ (int)WINDOW_W,(int)WINDOW_H });
        sf::Vertex gridLine[2];
        const float MAX_X = 1000.f;
        const float MAX_Y = 700.f;

        // Draw grid vertical lines
        sf::Vertex vline[2];
        for (float x = 0; x <= MAX_X; x += gridSize) {
            vline[0] = sf::Vertex({ x, 0.f }, sf::Color(190,190,190));
            vline[1] = sf::Vertex({ x, MAX_Y }, sf::Color(190,190,190));
            window.draw(vline, 2, sf::Lines);
        }

        // Draw grid horizontal lines
        sf::Vertex hline[2];
        for (float y = 0; y <= MAX_Y; y += gridSize) {
            hline[0] = sf::Vertex({ 0.f, y }, sf::Color(190,190,190));
            hline[1] = sf::Vertex({ MAX_X, y }, sf::Color(190,190,190));
            window.draw(hline, 2, sf::Lines);
        }

        // Draw edges (only between active nodes)
        for (int i = 0; i < (int)graph.nodes.size(); ++i) {
            if (!graph.nodes[i].active) continue;
            for (int j : graph.adj[i]) {
                if (j <= i) continue;
                if (!graph.nodes[j].active) continue;
                sf::Vertex e[2] = {
                    sf::Vertex(graph.nodes[i].pos, sf::Color(60,60,180)),
                    sf::Vertex(graph.nodes[j].pos, sf::Color(60,60,180))
                };
                window.draw(e, 2, sf::Lines);
            }
        }
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
        
        hoveredNode = -1;
        for (const auto& loc : graph.nodes) {
            if (!loc.active) continue;
            if (pointInCircle(mouseWorld, loc.pos, 12.f)) {
                hoveredNode = loc.id;
                break;
            }
        }
        // Draw nodes
        for (const auto& loc : graph.nodes) {
            if (!loc.active) continue;

            // Check if we have a texture for this type
            auto it = nodeTextures.find(loc.type);
            if (it != nodeTextures.end()) {
                sf::Sprite sprite;
                sprite.setTexture(it->second);
                sprite.setOrigin(it->second.getSize().x / 2.f, it->second.getSize().y / 2.f);
                sprite.setPosition(loc.pos);
                window.draw(sprite);
            } else {
                // fallback to circle if no texture found
                sf::CircleShape node(12.f);
                node.setOrigin(12.f, 12.f);
                node.setPosition(loc.pos);
                node.setOutlineColor(sf::Color::Black);
                node.setOutlineThickness(2.f);
                node.setFillColor(sf::Color(170, 170, 170));
                window.draw(node);
            }
        }

        
        if (hoveredNode != -1) {
            const auto& loc = graph.nodes[hoveredNode];
        
            sf::Text label;
            label.setFont(font);
            label.setString(loc.name);
            label.setCharacterSize(14);
            label.setFillColor(sf::Color::Black);
        
            float offsetX = -20.f;
            float offsetY = -30.f;
        
            sf::FloatRect bounds = label.getLocalBounds();
            label.setPosition(loc.pos.x + offsetX - bounds.width, loc.pos.y + offsetY);
        
            // Background box
            sf::RectangleShape bg(sf::Vector2f(bounds.width + 10, bounds.height + 10));
            bg.setFillColor(sf::Color(255, 255, 255, 220));
            bg.setOutlineColor(sf::Color::Black);
            bg.setOutlineThickness(1.f);
            bg.setPosition(label.getPosition().x - 5, label.getPosition().y - 5);
        
            window.draw(bg);
            window.draw(label);
        }
        
        
        // Reset to UI view
        window.setView(window.getDefaultView());

        // Draw + button
        window.draw(addBtn);
        window.draw(plusText);

        // Instructions
        sf::Text instr("Click '+' to add", font, 24);
        instr.setFillColor(sf::Color::Black);
        float padding = 10.f; // spacing between buttons
        instr.setPosition(
            addBtn.getPosition().x + 2*addBtn.getRadius() + padding, // right of addBtn
            addBtn.getPosition().y // same vertical position
        );
        window.draw(instr);

        // Draw travel button (only when popup is not open)
        if (!showTravelPopup && !showErrorPopup) {
            window.draw(travelBtn);
            window.draw(travelText);
        }

        // ---------------- Add popup UI ----------------
        sf::Vector2f pb = popupBg.getPosition();
        if (showAddPopup) {
            window.draw(popupBg);

            sf::Text title("Add Location", font, 20);
            title.setFillColor(sf::Color::Black);
            title.setPosition(pb.x + 20.f, pb.y + 8.f);
            window.draw(title);

            // Type row (styled dropdown)
            sf::RectangleShape typeRect(sf::Vector2f(200.f, 30.f));
            typeRect.setPosition(pb.x + 20.f, pb.y + 40.f);
            typeRect.setFillColor(sf::Color::White);
            typeRect.setOutlineColor(sf::Color::Black);
            typeRect.setOutlineThickness(1.f);
            window.draw(typeRect);

            // Arrow indicator
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setPoint(0, sf::Vector2f(0, 0));
            arrow.setPoint(1, sf::Vector2f(10, 0));
            arrow.setPoint(2, sf::Vector2f(5, 6));
            arrow.setFillColor(sf::Color::Black);
            arrow.setPosition(typeRect.getPosition().x + typeRect.getSize().x - 18.f, typeRect.getPosition().y + 10.f);
            window.draw(arrow);

            sf::Text typeText(("Type: " + buildingTypes[typeIndex]), font, 16);
            typeText.setFillColor(sf::Color::Black);
            typeText.setPosition(pb.x + 26.f, pb.y + 44.f);
            window.draw(typeText);

            // Name
            sf::RectangleShape nameRect(sf::Vector2f(380.f, 30.f));
            nameRect.setPosition(pb.x + 20.f, pb.y + 90.f);
            nameRect.setFillColor(typingName ? sf::Color(255, 255, 210) : sf::Color::White);
            nameRect.setOutlineColor(sf::Color::Black);
            nameRect.setOutlineThickness(1.f);
            window.draw(nameRect);

            sf::Text nameLabel("Name:", font, 16);
            nameLabel.setFillColor(sf::Color::Black);
            nameLabel.setPosition(pb.x + 26.f, pb.y + 92.f);
            window.draw(nameLabel);

            sf::Text nameInput(inputName.empty() ? "<enter name>" : inputName, font, 16);
            nameInput.setFillColor(sf::Color::Black);
            nameInput.setPosition(pb.x + 110.f, pb.y + 92.f);
            window.draw(nameInput);

            // X
            sf::RectangleShape xRect(sf::Vector2f(160.f, 30.f));
            xRect.setPosition(pb.x + 20.f, pb.y + 140.f);
            xRect.setFillColor(typingX ? sf::Color(255, 255, 210) : sf::Color::White);
            xRect.setOutlineColor(sf::Color::Black);
            xRect.setOutlineThickness(1.f);
            window.draw(xRect);

            sf::Text xLabel(("X: " + inputX), font, 16);
            xLabel.setFillColor(sf::Color::Black);
            xLabel.setPosition(pb.x + 26.f, pb.y + 142.f);
            window.draw(xLabel);

            // Y
            sf::RectangleShape yRect(sf::Vector2f(160.f, 30.f));
            yRect.setPosition(pb.x + 210.f, pb.y + 140.f);
            yRect.setFillColor(typingY ? sf::Color(255, 255, 210) : sf::Color::White);
            yRect.setOutlineColor(sf::Color::Black);
            yRect.setOutlineThickness(1.f);
            window.draw(yRect);

            sf::Text yLabel(("Y: " + inputY), font, 16);
            yLabel.setFillColor(sf::Color::Black);
            yLabel.setPosition(pb.x + 216.f, pb.y + 142.f);
            window.draw(yLabel);

            // Buttons
            submitBtn.setPosition(pb.x + 220.f, pb.y + 200.f);
            cancelBtn.setPosition(pb.x + 60.f, pb.y + 200.f);
            window.draw(submitBtn);
            window.draw(cancelBtn);
            sf::Text sT("Submit", font, 16); sT.setFillColor(sf::Color::Black); sT.setPosition(submitBtn.getPosition().x + 14.f, submitBtn.getPosition().y + 6.f); window.draw(sT);
            sf::Text cT("Cancel", font, 16); cT.setFillColor(sf::Color::Black); cT.setPosition(cancelBtn.getPosition().x + 20.f, cancelBtn.getPosition().y + 6.f); window.draw(cT);
        } // end showAddPopup
        
        static std::string selectedType = "";  // persists across frames
        static int dropDownIndex = 0;
        static bool keyPressed = false;
        if (typeDropdownOpen) {
            float itemH = 28.f;
            sf::Vector2f dropdownPos(pb.x + 20.f, pb.y + 50.f);
            sf::RectangleShape typeRect(sf::Vector2f(200.f, 30.f));
            typeRect.setPosition(dropdownPos);
        
            // Background for dropdown items
            sf::RectangleShape dropBg(sf::Vector2f(typeRect.getSize().x, itemH * (float)buildingTypes.size()));
            dropBg.setPosition(dropdownPos.x, dropdownPos.y + typeRect.getSize().y);
            dropBg.setFillColor(sf::Color(245, 245, 245));
            dropBg.setOutlineColor(sf::Color::Black);
            dropBg.setOutlineThickness(1.f);
            window.draw(dropBg);
        
            // --- Handle keyboard input ---
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !keyPressed) {
                dropDownIndex++;
                if (dropDownIndex >= buildingTypes.size()) dropDownIndex = buildingTypes.size() - 1;
                keyPressed = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !keyPressed) {
                dropDownIndex--;
                if (dropDownIndex < 0) dropDownIndex = 0;
                keyPressed = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !keyPressed) {
                selectedType = buildingTypes[dropDownIndex]; // select option
                typeDropdownOpen = false; // close dropdown
                keyPressed = true;
            }
            else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
                     !sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
                     !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                keyPressed = false; // reset flag when no key pressed
            }
        
            // --- Draw dropdown items ---
            for (int i = 0; i < (int)buildingTypes.size(); ++i) {
                sf::RectangleShape itemRect(sf::Vector2f(typeRect.getSize().x, itemH));
                itemRect.setPosition(dropBg.getPosition().x, dropBg.getPosition().y + i * itemH);
        
                // Highlight if keyboard selected
                if (i == dropDownIndex) {
                    itemRect.setFillColor(sf::Color(200, 220, 255));
                } else {
                    itemRect.setFillColor(sf::Color::White);
                }
        
                window.draw(itemRect);
        
                sf::Text it(buildingTypes[i], font, 16);
                it.setFillColor(sf::Color::Black);
                it.setPosition(itemRect.getPosition().x + 8.f, itemRect.getPosition().y + 4.f);
                window.draw(it);
            }
            typeIndex = dropDownIndex;
        }
        

        // ---------------- Connect popup UI ----------------
        if (showConnectPopup) {
            window.draw(connectBg);
            sf::Vector2f cb = connectBg.getPosition();

            sf::Text title("Create Connection", font, 20);
            title.setFillColor(sf::Color::Black);
            title.setPosition(cb.x + 20.f, cb.y + 8.f);
            window.draw(title);

            // From box (dropdown)
            sf::RectangleShape aRect(sf::Vector2f(380.f, 30.f));
            aRect.setPosition(cb.x + 20.f, cb.y + 40.f);
            aRect.setFillColor(dropAOpen ? sf::Color(255, 255, 210) : sf::Color::White);
            aRect.setOutlineColor(sf::Color::Black);
            aRect.setOutlineThickness(1.f);
            window.draw(aRect);

            std::string fromLabel = "From: ";
            if (connectA >= 0 && connectA < (int)graph.nodes.size()) fromLabel += graph.nodes[connectA].name;
            else fromLabel += "<choose>";
            sf::Text aText(fromLabel, font, 16);
            aText.setFillColor(sf::Color::Black);
            aText.setPosition(cb.x + 26.f, cb.y + 42.f);
            window.draw(aText);

            // To box
            sf::RectangleShape bRect(sf::Vector2f(380.f, 30.f));
            bRect.setPosition(cb.x + 20.f, cb.y + 90.f);
            bRect.setFillColor(dropBOpen ? sf::Color(255, 255, 210) : sf::Color::White);
            bRect.setOutlineColor(sf::Color::Black);
            bRect.setOutlineThickness(1.f);
            window.draw(bRect);

            std::string toLabel = "To: ";
            if (connectB >= 0 && connectB < (int)graph.nodes.size()) toLabel += graph.nodes[connectB].name;
            else toLabel += "<choose>";
            sf::Text bText(toLabel, font, 16);
            bText.setFillColor(sf::Color::Black);
            bText.setPosition(cb.x + 26.f, cb.y + 92.f);
            window.draw(bText);

            

            // Buttons
            submitBtn.setPosition(cb.x + 220.f, cb.y + 140.f);
            cancelBtn.setPosition(cb.x + 60.f, cb.y + 140.f);
            window.draw(submitBtn);
            window.draw(cancelBtn);
            sf::Text sT("Connect", font, 16); sT.setFillColor(sf::Color::Black); sT.setPosition(submitBtn.getPosition().x + 10.f, submitBtn.getPosition().y + 6.f); window.draw(sT);
            sf::Text cT("Cancel", font, 16); cT.setFillColor(sf::Color::Black); cT.setPosition(cancelBtn.getPosition().x + 20.f, cancelBtn.getPosition().y + 6.f); window.draw(cT);

            // hint
            sf::Text hint("Click From/To to open dropdowns and pick locations", font, 12);
            hint.setFillColor(sf::Color(80, 80, 80));
            hint.setPosition(cb.x + 20.f, cb.y + 180.f);
            window.draw(hint);
            // If dropdown A open, draw list
            if (dropAOpen) {
                float itemH = 28.f;
                sf::RectangleShape dropBg(sf::Vector2f(aRect.getSize().x, itemH * std::max(1, (int)graph.nodes.size())));
                dropBg.setPosition(aRect.getPosition().x, aRect.getPosition().y + aRect.getSize().y);
                dropBg.setFillColor(sf::Color(245, 245, 245));
                dropBg.setOutlineColor(sf::Color::Black);
                dropBg.setOutlineThickness(1.f);
                window.draw(dropBg);

                for (int i = 0; i < (int)graph.nodes.size(); ++i) {
                    if (!graph.nodes[i].active) continue;
                    sf::RectangleShape item(sf::Vector2f(aRect.getSize().x, itemH));
                    item.setPosition(dropBg.getPosition().x, dropBg.getPosition().y + i * itemH);
                    // hover highlight
                    sf::Vector2i mi = sf::Mouse::getPosition(window);
                    sf::Vector2f m((float)mi.x, (float)mi.y);
                    if (sf::FloatRect(item.getPosition(), item.getSize()).contains(m)) item.setFillColor(sf::Color(200, 220, 255));
                    else item.setFillColor(sf::Color::White);
                    window.draw(item);

                    sf::Text it(graph.nodes[i].name, font, 15);
                    it.setFillColor(sf::Color::Black);
                    it.setPosition(item.getPosition().x + 8.f, item.getPosition().y + 4.f);
                    window.draw(it);
                }
            }

            if (dropBOpen) {
                float itemH = 28.f;
                sf::RectangleShape dropBg(sf::Vector2f(bRect.getSize().x, itemH * std::max(1, (int)graph.nodes.size())));
                dropBg.setPosition(bRect.getPosition().x, bRect.getPosition().y + bRect.getSize().y);
                dropBg.setFillColor(sf::Color(245, 245, 245));
                dropBg.setOutlineColor(sf::Color::Black);
                dropBg.setOutlineThickness(1.f);
                window.draw(dropBg);

                for (int i = 0; i < (int)graph.nodes.size(); ++i) {
                    if (!graph.nodes[i].active) continue;
                    sf::RectangleShape item(sf::Vector2f(bRect.getSize().x, itemH));
                    item.setPosition(dropBg.getPosition().x, dropBg.getPosition().y + i * itemH);
                    // hover highlight
                    sf::Vector2i mi = sf::Mouse::getPosition(window);
                    sf::Vector2f m((float)mi.x, (float)mi.y);
                    if (sf::FloatRect(item.getPosition(), item.getSize()).contains(m)) item.setFillColor(sf::Color(200, 220, 255));
                    else item.setFillColor(sf::Color::White);
                    window.draw(item);

                    sf::Text it(graph.nodes[i].name, font, 15);
                    it.setFillColor(sf::Color::Black);
                    it.setPosition(item.getPosition().x + 8.f, item.getPosition().y + 4.f);
                    window.draw(it);
                }
            }
        } // end connect popup

        // ---------------- First-node modal (one-time) ----------------
        if (firstNodeNoticePending) {
            // draw a centered modal rectangle
            float mw = 480.f, mh = 140.f;
            sf::RectangleShape modalBg(sf::Vector2f(mw, mh));
            modalBg.setFillColor(sf::Color(245, 245, 245));
            modalBg.setOutlineColor(sf::Color::Black);
            modalBg.setOutlineThickness(2.f);
            modalBg.setPosition((WINDOW_W - mw) / 2.f, (WINDOW_H - mh) / 2.f - 20.f);
            window.draw(modalBg);

            sf::Text mt("Notice", font, 20);
            mt.setFillColor(sf::Color::Black);
            mt.setPosition(modalBg.getPosition().x + 18.f, modalBg.getPosition().y + 10.f);
            window.draw(mt);

            sf::Text msg("You have added your first location.\nPlease add at least one more location before creating connections.", font, 16);
            msg.setFillColor(sf::Color::Black);
            msg.setPosition(modalBg.getPosition().x + 18.f, modalBg.getPosition().y + 44.f);
            window.draw(msg);

            // OK button
            sf::RectangleShape okBtn(sf::Vector2f(100.f, 34.f));
            okBtn.setFillColor(sf::Color(180, 240, 180));
            okBtn.setOutlineColor(sf::Color::Black);
            okBtn.setOutlineThickness(1.f);
            okBtn.setPosition(modalBg.getPosition().x + (mw - 100.f) / 2.f, modalBg.getPosition().y + mh - 48.f);
            window.draw(okBtn);
            sf::Text okT("OK", font, 16); okT.setFillColor(sf::Color::Black); okT.setPosition(okBtn.getPosition().x + 36.f, okBtn.getPosition().y + 6.f); window.draw(okT);

            // detect click on OK (use immediate mouse state)
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mp = sf::Mouse::getPosition(window);
                sf::Vector2f m((float)mp.x, (float)mp.y);
                if (sf::FloatRect(okBtn.getPosition(), okBtn.getSize()).contains(m)) {
                    firstNodeNoticePending = false;
                }
            }
        }

        // ---------------- Travel popup ----------------
        travel(window, graph, showTravelPopup, showErrorPopup, 
            startPoint, endPoint, typingStart, typingEnd,
            currentPath, showPath, mapView);

        // ---------------- Floating history panel rendering ----------------
        // Reposition panel background
        panelBg.setPosition(panelX, panelY);
        window.draw(panelBg);

        // Title
        panelTitle.setPosition(panelX + 14.f, panelY + 12.f);
        window.draw(panelTitle);

        // Buttons inside panel (Undo/Redo/Clear/Close)
        btnUndo.setPosition(panelX + 14.f, panelY + 44.f);
        btnRedo.setPosition(panelX + 14.f + 80.f, panelY + 44.f);
        btnClear.setPosition(panelX + 14.f + 160.f, panelY + 44.f);
        btnClose.setPosition(panelX + panelW - 44.f, panelY + 12.f);

        window.draw(btnUndo);
        window.draw(btnRedo);
        window.draw(btnClear);
        window.draw(btnClose);

        sf::Text tu("Undo", font, 13); tu.setPosition(btnUndo.getPosition().x + 6.f, btnUndo.getPosition().y + 6.f);
        sf::Text tr("Redo", font, 13); tr.setPosition(btnRedo.getPosition().x + 6.f, btnRedo.getPosition().y + 6.f);
        sf::Text tc("Clear", font, 13); tc.setPosition(btnClear.getPosition().x + 14.f, btnClear.getPosition().y + 6.f);
        sf::Text tclose("X", font, 14); tclose.setPosition(btnClose.getPosition().x + 10.f, btnClose.getPosition().y + 6.f);

        tu.setFillColor(sf::Color::Black); tr.setFillColor(sf::Color::Black); tc.setFillColor(sf::Color::Black); tclose.setFillColor(sf::Color::White);
        window.draw(tu); window.draw(tr); window.draw(tc); window.draw(tclose);

        // Show counts for stacks
        std::ostringstream sUndoCount; sUndoCount << "Undo: " << undoStack.size();
        std::ostringstream sRedoCount; sRedoCount << "Redo: " << redoStack.size();
        sf::Text sund(sUndoCount.str(), font, 12); sund.setPosition(panelX + 14.f, panelY + 80.f); sund.setFillColor(sf::Color::White);
        sf::Text sred(sRedoCount.str(), font, 12); sred.setPosition(panelX + 120.f, panelY + 80.f); sred.setFillColor(sf::Color::White);
        window.draw(sund); window.draw(sred);

        // Draw history list (top = recent)
        auto hv = undoStack.toVector();
        float listTop = panelY + 104.f;
        float yp = listTop;
        const float lineH = 18.f;
        for (int i = 0; i < (int)hv.size() && (yp + lineH) < (panelY + panelH - 16.f); ++i) {
            sf::Text t(hv[i], font, 13);
            t.setFillColor(sf::Color::White);
            t.setPosition(panelX + 14.f, yp);
            window.draw(t);
            yp += lineH;
        }
        // If no history
        if (hv.empty()) {
            sf::Text t("(no actions yet)", font, 13);
            t.setFillColor(sf::Color(200, 200, 200));
            t.setPosition(panelX + 14.f, listTop);
            window.draw(t);
        }

        window.display();
    }
}