#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace sf;


sf::RenderWindow window(sf::VideoMode(1280, 800), "Vampire Survivors Olympus");
sf::CircleShape shape(100.f);
void Update();
void Start();
void Draw();
float deltaTime;
int main()
{
    Start();
    Clock clock;
    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }
        }
        Update();
        Draw();
    }
    return 0;
}
void Start()
{
    // code here is only executed at the start of the program
    window.setFramerateLimit(60);
}
void Update()
{
    // code here is executed every frame since the start of the program

}
void Draw()
{
    // code here is executed every frame since the start of the program

    window.clear(); //clear every pixel on the screen

    //Draw your sprites here

    window.draw(shape);

    
    window.display(); //Display sprites on screen
}
