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
Text StartGameText, SettingsText, ExitText,LeaderboardText;
Font defgamefont; // default game font
Texture MainMenuButtons_Texture,MainMenuBackground_Texture;
Sprite MainMenuButtons, MainMenuBackground;
View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;
RectangleShape StartButton(Vector2f(490, 110)),SettingsButton(Vector2f(490, 110)),LeaderboardButton(Vector2f(490, 110)),
               ExitButton(Vector2f(490, 110));
FloatRect StartButtonBounds,SettingsButtonBounds,LeaderboardButtonBounds,ExitButtonBounds;
Sound MainMenuMusic;
SoundBuffer MainMenuMusic_source;

RenderWindow window(VideoMode(1280, 800), "Vampire Survivors Olympus");

void Update();
void Start();
void Draw();
void MainMenuButtonCheck()
{
    if (Mouse::isButtonPressed(Mouse::Left) && StartButtonBounds.contains(mouseWorldpos))
    {
        gamestate = gameloop;
        MainMenuMusic.stop();
    }
    if (Mouse::isButtonPressed(Mouse::Left) && LeaderboardButtonBounds.contains(mouseWorldpos))
    {
        gamestate = leaderboard;
        MainMenuMusic.stop();
    }
    if (Mouse::isButtonPressed(Mouse::Left) && SettingsButtonBounds.contains(mouseWorldpos))
    {
        gamestate = settings;
        MainMenuMusic.stop();
    }
    if (Mouse::isButtonPressed(Mouse::Left) && ExitButtonBounds.contains(mouseWorldpos))
    {
        window.close();
        MainMenuMusic.stop();
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
void MainmenuInit() {
    //Main menu initializations
    //please avoid editing this section
    StartButton.setPosition(9853, 9818);
    StartButton.setScale(0.6, 0.65);
    StartButtonBounds = StartButton.getGlobalBounds();
    StartGameText.setFont(defgamefont);
    StartGameText.setString("Start game");
    StartGameText.setPosition(9915, 9830);

    SettingsButton.setPosition(9853, 9910);
    SettingsButton.setFillColor(Color::Red);
    SettingsButton.setScale(0.6, 0.65);
    SettingsButtonBounds = SettingsButton.getGlobalBounds();
    SettingsText.setFont(defgamefont);
    SettingsText.setString("Settings");
    SettingsText.setPosition(9930, 9923);

    LeaderboardButton.setPosition(9853, 10002);
    LeaderboardButton.setFillColor(Color::Red);
    LeaderboardButton.setScale(0.6, 0.65);
    LeaderboardButtonBounds = LeaderboardButton.getGlobalBounds();
    LeaderboardText.setFont(defgamefont);
    LeaderboardText.setString("Leaderboard");
    LeaderboardText.setPosition(9900, 10015);

    ExitButton.setPosition(9853, 10094);
    ExitButton.setFillColor(Color::Red);
    ExitButton.setScale(0.6, 0.65);
    ExitButtonBounds = ExitButton.getGlobalBounds();
    ExitText.setFont(defgamefont);
    ExitText.setString("Exit");
    ExitText.setPosition(9963, 10107);

    MainMenuButtons_Texture.loadFromFile("Assets\\mainmenu.png");
    MainMenuButtons.setTexture(MainMenuButtons_Texture);
    MainMenuButtons.setOrigin(281, 325);
    MainMenuButtons.setPosition(10000, 10000);
    MainMenuButtons.setScale(0.6, 0.65);

    MainMenuBackground_Texture.loadFromFile("Assets\\MainBackground.png");
    MainMenuBackground.setTexture(MainMenuBackground_Texture);
    MainMenuBackground.setScale(0.84, 1.12);
    MainMenuBackground.setPosition(9340, 9300);

    MainMenuMusic_source.loadFromFile("Assets\\MainMenuMusic.ogg");
    MainMenuMusic.setBuffer(MainMenuMusic_source);
    MainMenuMusic.play();
    MainMenuMusic.setLoop(true);
}
void Start()
{
    // code here is only executed at the start of the program
    // initializations of everything
    window.setFramerateLimit(60);

    //Game font initialization
    defgamefont.loadFromFile("VampireZone.ttf");

    MainmenuInit();
   

    view.setCenter(10000, 9800);
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
        cout << "we are in game phase ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            MainMenuMusic.play();
        }
        if (Keyboard::isKeyPressed(Keyboard::Q))
        {
            gamestate = gameover;
        }
    }
    if (gamestate == settings)
    {
        // settings menu update
        cout << "we are in settings menu ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            MainMenuMusic.play();
        }
    }
    if (gamestate == leaderboard)
    {
        // settings menu update
        cout << "we are in leaderboard menu ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            MainMenuMusic.play();
        }
    }
    if (gamestate == gameover)
    {
        // gameover screen update
        cout << "The player is dead ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            MainMenuMusic.play();
        }
    }
}
void Draw()
{
    // code here is executed every frame since the start of the program

    window.clear(); // clear every pixel on the screen

    // Draw your sprites here

    if (gamestate == mainmenu)
    {
        ////Debugging
        //window.draw(StartButton);
        //window.draw(SettingsButton);
        //window.draw(LeaderboardButton);
        //window.draw(ExitButton);
         

        // main menu draw
        window.draw(MainMenuBackground);
        window.draw(MainMenuButtons);
        window.draw(StartGameText);
        window.draw(SettingsText);
        window.draw(LeaderboardText);
        window.draw(ExitText);

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
