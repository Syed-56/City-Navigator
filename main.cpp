#include <SFML/Graphics.hpp>

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test - Working!");
    
    // Create a circle shape
    sf::CircleShape circle(100.f);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(300, 200); // Center it roughly
    
    // Create a rectangle shape
    sf::RectangleShape rectangle(sf::Vector2f(120.f, 60.f));
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setPosition(340, 400);
    
    // Create text (if you have font)
    sf::Font font;
    sf::Text text;
    
    // Try to load a font (optional - will still work without it)
    if (font.loadFromFile("arial.ttf")) {
        text.setFont(font);
        text.setString("SFML 2.6.1 is Working!");
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(250, 50);
    }
    
    // Main game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window if close button clicked
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Close window if Escape key pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }
        
        // Clear screen
        window.clear(sf::Color::Black);
        
        // Draw everything
        window.draw(circle);
        window.draw(rectangle);
        
        // Draw text if font was loaded
        if (font.loadFromFile("arial.ttf")) {
            window.draw(text);
        }
        
        // Update the window
        window.display();
    }
    
    return 0;
}