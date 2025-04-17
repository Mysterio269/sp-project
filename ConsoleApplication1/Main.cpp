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
sf::CircleShape circle(100.f);
void Update();
void Start();
void Draw();
float deltaTime;
struct player
{
    RectangleShape realCollision ;// real sprite Bounds
    Sprite shape;
    Texture walk;
    Texture idle;
    int health,xp;
    float speed;    
    Vector2f velocity;
    bool isDead;
    void update() {
        if (health <= 0)
            isDead = true;
        realCollision.setPosition(shape.getPosition());
        realCollision.setOrigin(shape.getOrigin());
    }
}player1;
struct enemy {
    RectangleShape attackBox;
    Vector2f velocity;
    Sprite shape;
    Texture walk, attack, dead;
    float speed;
    int health;
    bool isAttacking, isDead;
    void update() {
        if (health <= 0)
            isDead = true;
    }
}enemy1;
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

    window.draw(circle);

    
    window.display(); //Display sprites on screen
}
