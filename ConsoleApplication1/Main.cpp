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
enum Gamestate
{
    mainmenu,
    gameloop,
    settings,
    gameover,
    leaderboard
};
Gamestate gamestate = mainmenu;
Text StartGameText, SettingsText, ExitText;
Font defgamefont; // default game font
Texture MainMenuBackground_Texture;
Sprite MainMenuBackGround;
View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;
RectangleShape StartButton(Vector2f(490, 110));
FloatRect StartButtonBounds;

RenderWindow window(VideoMode(1280, 800), "Vampire Survivors Olympus");

void Update();
void Start();
void Draw();
void MainMenuButtonCheck()
{
    if (Mouse::isButtonPressed(Mouse::Left) && StartButtonBounds.contains(mouseWorldpos))
    {
        gamestate = gameloop;
    }
}
float deltaTime;
struct player
{
    RectangleShape collider; // Sprite collider
    Sprite shape;
    Texture playerspreadsheet;
    int health, xp;
    float speed;
    Vector2f velocity;
    bool isDead;
    void update()
    {
        if (health <= 0)
            isDead = true;
        collider.setPosition(shape.getPosition());
        collider.setOrigin(shape.getOrigin());
    }
} player1;
struct enemy
{
    RectangleShape attackBox,collider;
    Vector2f velocity;
    Sprite shape;
    Texture enemyspreadsheet;
    float speed;
    int health,damage;
    bool isAttacking, isDead;
    void update()
    {
        if (health <= 0)
            isDead = true;
    }
} enemy1;
int main()
{
    Start();
    Clock clock;
    while (window.isOpen())
    {
        deltaTime = clock.restart().asSeconds();
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
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
    // initializations of everything
    window.setFramerateLimit(60);

    defgamefont.loadFromFile("VampireZone.ttf");
    StartGameText.setFont(defgamefont);
    StartGameText.setString("Start game");
    StartGameText.setPosition(9910, 9830);

    StartButton.setPosition(9853, 9818);
    StartButton.setScale(0.6, 0.65);
    StartButtonBounds = StartButton.getGlobalBounds();

    MainMenuBackground_Texture.loadFromFile("Assets\\mainmenu.png");
    MainMenuBackGround.setTexture(MainMenuBackground_Texture);
    MainMenuBackGround.setOrigin(281, 325);
    MainMenuBackGround.setPosition(10000, 10000);
    MainMenuBackGround.setScale(0.6, 0.65);

    view.setCenter(10000, 10000);
    window.setView(view);
}

void Update()
{
    // code here is executed every frame since the start of the program
    mouseScreenpos = Mouse::getPosition(window);
    mouseWorldpos = window.mapPixelToCoords(mouseScreenpos);
    if (gamestate == mainmenu)
    {
        // main menu update

        MainMenuButtonCheck();
    }
    if (gamestate == gameloop)
    {
        // gameloop update
        cout << "we are in game phase";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
        }
    }
    if (gamestate == settings)
    {
        // settings menu update
    }
    if (gamestate == gameover)
    {
        // gameover screen update
    }
}
void Draw()
{
    // code here is executed every frame since the start of the program

    window.clear(); // clear every pixel on the screen

    // Draw your sprites here

    if (gamestate == mainmenu)
    {
        // main menu draw
        window.draw(MainMenuBackGround);
        // window.draw(StartButton);
        window.draw(StartGameText);
    }
    if (gamestate == gameloop)
    {
        // gameloop draw
    }
    if (gamestate == settings)
    {
        // settings menu draw
    }
    if (gamestate == gameover)
    {
        // gameover screen draw
    }

    window.display(); // Display sprites on screen
}
