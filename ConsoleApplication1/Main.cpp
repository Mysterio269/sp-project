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
enum animationstate
{
    attacking, walking, idle, ultimate
};
enum playerDirection
{
    toright,toleft
};
Gamestate gamestate = mainmenu;

Text StartGameText, SettingsText, ExitText,LeaderboardText;
Text GameOverText, ScoreText, RestartText;
Font defgamefont; // default game font

Texture MainMenuButtons_Texture,MainMenuBackground_Texture,Map_Texture,healthbar_Texture;
Texture swordspritesheet;
Sprite MainMenuButtons, MainMenuBackground,Map,healthbar;
View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;

RectangleShape StartButton(Vector2f(490, 110)),SettingsButton(Vector2f(490, 110)),LeaderboardButton(Vector2f(490, 110)),
               ExitButton(Vector2f(490, 110));
RectangleShape gameOverOverlay; // red color in gameover background
FloatRect StartButtonBounds,SettingsButtonBounds,LeaderboardButtonBounds,ExitButtonBounds;
RectangleShape menuCursor;

int selectedMenuButtonIndex = 0; // 0 for Start, 1 for Settings, 2 for Leaderboard, 3 for Exit


Sound MainMenuMusic, GameOverSound;
SoundBuffer MainMenuMusic_source, GameOverSound_source;
bool gameOverSoundPlayed = false;

sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
RenderWindow window(desktopMode, "Vampire Survivors :The path to the legendary formula", sf::Style::Fullscreen);
//fullscreen fix

float healthRatio;
float shootingtime = 0.f;
float shootingrate = 3;
float deltaTime;
float totalGameTime = 0.f;
float menuInputDelay = 0.f;
const float MENU_INPUT_COOLDOWN = 0.2f; // Time in seconds between allowed inputs


void MainMenuInput();
void Update();
void Start();
void Draw();



struct character
{
    RectangleShape collider; // Sprite collider
    Sprite sprite;
    Texture texture;
    Texture playerspreadsheet;
    float health = 120, xp;
    float Maxhp = 120;
    float speed;
    Vector2f velocity;
    bool isDead;
    animationstate AnimationState ;
    playerDirection spriteDirection;
    int columnIndex = 0;
    int rowIndex = 0;
    int animationdelaytimer = 0;


    void update()
    {
        //spriteDirection = toright;
        AnimationState = idle;
        animationdelaytimer++;
        if (animationdelaytimer > 3) {
            columnIndex++;
            animationdelaytimer = 0;
        }

        if (health <= 0)
            isDead = true;

        //movement
        {
            if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) {
                sprite.move(-speed *deltaTime, 0);
                sprite.setScale(-1, 1.5);
                spriteDirection = toleft;
                AnimationState = walking;
            }
            if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) {
                sprite.move(speed * deltaTime, 0);
                sprite.setScale(1, 1.5);
                spriteDirection = toright;
                AnimationState = walking;

            }
            if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) {
                sprite.move(0, speed * deltaTime);
                AnimationState = walking;

            }
            if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) {
                sprite.move(0, -speed * deltaTime);
                AnimationState = walking;

            }
            if (Keyboard::isKeyPressed(Keyboard::E)) {
                AnimationState = attacking;
            }
        }

        //animation
        {
            switch (AnimationState) {
            case attacking:
                rowIndex = 2;
                columnIndex %= 6;
                if(columnIndex == 0)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130 +13, 120, 148));
                else if(columnIndex == 1)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130 +18, 120, 148));
                else if (columnIndex == 2)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130 + 18, 160, 148));
                else if (columnIndex == 3)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 90, rowIndex * 130 + 18, 160, 148));
                else if (columnIndex == 4)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 135, rowIndex * 130 + 11, 160, 148));
                else if (columnIndex == 5)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 195, rowIndex * 130  + 8, 160, 140));
                break;
            case walking:
                rowIndex = 1;
                columnIndex %= 8;
                sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130, 120, 148));
                break;
            case idle:
                rowIndex = 0;
                columnIndex = columnIndex % 4;
                sprite.setTextureRect(IntRect(columnIndex * 111 + 30, rowIndex * 130 - 5, 120, 148));
                break;
            }
        }
    }
} shanoa;

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

struct sword {
    Sprite shape;
    RectangleShape collider;
    Vector2f velocity;
    float speed;

    void update(float deltaTime)
    {
        shape.move(velocity * deltaTime);
        collider.setPosition(shape.getPosition());
        collider.setOrigin(shape.getOrigin());
        shape.setOrigin(16, 16);
        collider.setOrigin(15, 7.5);
    }
    
};
    vector<sword> swords; 



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

void shooting()
{
    shootingtime += deltaTime;
    
    if (shootingtime>=shootingrate)
    {
        shootingtime = 0;
        float erasuretimer = 0;
        sword newSword;
        newSword.speed = 2500 * deltaTime;
        newSword.shape.setTexture(swordspritesheet);
        newSword.shape.setTextureRect(IntRect(1 * 32, 2 * 32, 32, 32));
        newSword.shape.setScale(2, 2);
        newSword.shape.setPosition(shanoa.sprite.getPosition());// init
        newSword.collider.setSize(Vector2f(30, 15));// init

   

        if (shanoa.spriteDirection == toleft)
        {
            newSword.velocity = Vector2f(-1.f, 0.f) * newSword.speed;
            newSword.shape.setRotation(225);
        }
        else  
        {
            newSword.velocity = Vector2f(1.f, 0.f) * newSword.speed;
            newSword.shape.setRotation(45);
        }
        swords.push_back(newSword);
        /*erasuretimer += deltaTime; //failed algorithm :'(
        if (erasuretimer > 5) {
            swords.erase(swords.begin() +1);
            erasuretimer = 0;
        }*/
    }

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

    menuCursor.setFillColor(Color::Transparent);
    menuCursor.setOutlineColor(Color::Yellow);
    menuCursor.setOutlineThickness(4);


    selectedMenuButtonIndex = 0; 


    MainMenuMusic_source.loadFromFile("Assets\\MainMenuMusic.ogg");
    MainMenuMusic.setBuffer(MainMenuMusic_source);
    MainMenuMusic.play();
    MainMenuMusic.setLoop(true);
}

void CharacterInit() {
    shanoa.texture.loadFromFile("Assets\\shanoa.png");
    shanoa.sprite.setTexture(shanoa.texture);
    shanoa.speed = 200;
    shanoa.sprite.setPosition(0, 0);
    shanoa.collider.setPosition(shanoa.sprite.getPosition());
    shanoa.collider.setOrigin(shanoa.sprite.getOrigin());
    shanoa.sprite.setOrigin(66, 74);
    shanoa.sprite.setScale(1, 1.5);
    shanoa.AnimationState = idle;
    healthbar.setTexture(healthbar_Texture);
    healthbar.setScale(0.84, 1.2);
}

void MapInit() {
    Map_Texture.loadFromFile("Assets\\mapfinal.png");
    Map_Texture.setRepeated(true);
    Map.setTextureRect(IntRect(0,0,20000,20000));
    Map.setTexture(Map_Texture);
    Map.setPosition(-10000, -10000);
}


void healthbarhandling() {
    healthRatio = shanoa.health / shanoa.Maxhp;
    if (healthRatio > 0.9) {
        healthbar.setTextureRect(IntRect(0, 0, 445, 177));
    }
    else if (healthRatio > 0.8) {
        healthbar.setTextureRect(IntRect(444.4+ 5, 0, 445, 177));
    }
    else if (healthRatio > 0.7) {
        healthbar.setTextureRect(IntRect(888.8 + 10, 0, 445, 177));
    }
    else if (healthRatio > 0.6) {
        healthbar.setTextureRect(IntRect(1333.2 + 22, 0, 445, 177));
    }
    else if (healthRatio > 0.5) {
        healthbar.setTextureRect(IntRect(1777.6 + 27, 0, 445, 177));
    }
    else if (healthRatio > 0.4) {
        healthbar.setTextureRect(IntRect(2222 + 32, 0, 445, 177));
    }
    else if (healthRatio > 0.3) {
        healthbar.setTextureRect(IntRect(2666.4 + 30, 0, 445, 177));
    }
    else if (healthRatio > 0.2) {
        healthbar.setTextureRect(IntRect(3110.8 + 35, 0, 445, 177));
    }
    else if (healthRatio > 0.1) {
        healthbar.setTextureRect(IntRect(3552.2 + 42, 0, 445, 177));
    }
    else if(shanoa.health <= 0){
        gamestate = gameover;
    }
    healthbar.setPosition(shanoa.sprite.getPosition().x - 500, shanoa.sprite.getPosition().y - 500);
  }

void GameOverInit()
{
    GameOverText.setFont(defgamefont);
    GameOverText.setString("GAME OVER");
    GameOverText.setCharacterSize(80);
    GameOverText.setFillColor(Color::Red);

    ScoreText.setFont(defgamefont);
    ScoreText.setString("Time: 00:00");
    ScoreText.setCharacterSize(40); 
    ScoreText.setFillColor(Color::White);

    RestartText.setFont(defgamefont);
    RestartText.setString("Press R to return to Main Menu");
    RestartText.setCharacterSize(30);
    RestartText.setFillColor(Color::White);


    gameOverOverlay.setSize(view.getSize()); // Set size based on the view's size
    // Set its origin to the center, so position(view.getCenter()) works correctly
    gameOverOverlay.setOrigin(view.getSize().x / 2.f, view.getSize().y / 2.f);

    // The fourth parameter (100) is the alpha channel (transparency), from 0 (fully transparent) to 255 (fully opaque)
    gameOverOverlay.setFillColor(Color(100, 0, 0, 120));

    // REMEMBER TO REPLACE GAMEOVER SOUND WITH NEW ONE
    GameOverSound_source.loadFromFile("Assets\\MainMenuMusic.ogg");
    GameOverSound.setBuffer(GameOverSound_source);

    gameOverSoundPlayed = false;

}

void MainMenuInput()
{
    menuInputDelay += deltaTime; //input delay
    if (menuInputDelay >= MENU_INPUT_COOLDOWN)
    {
        bool moved = false;
        if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down))
        {
            selectedMenuButtonIndex++;
            moved = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up))
        {
            selectedMenuButtonIndex--;
            moved = true;
        }

        if (moved)
        {
            // handling out of bounds
            if (selectedMenuButtonIndex < 0) {
                selectedMenuButtonIndex = 3; 
            }
            else if (selectedMenuButtonIndex > 3) { 
                selectedMenuButtonIndex = 0;
            }

            // delay for next move
            menuInputDelay = 0.f;
        }

        if (Keyboard::isKeyPressed(Keyboard::Enter))
        {
            if (selectedMenuButtonIndex == 0) { // start Game
                gamestate = gameloop;
                shanoa.sprite.setPosition(0, 0);
                MainMenuMusic.stop();

                //RESETTING after death for next game
                shanoa.health = 100; // <--replace 100 with your actual starting health
                shanoa.isDead = false;
                totalGameTime = 0.f;
                swords.clear(); // Clear any old swords when starting a NEW game

                //RESET MONSTERS HERE WHEN IMPLEMENTED

            }
            else if (selectedMenuButtonIndex == 1) { // settings
                gamestate = settings;
                MainMenuMusic.stop();
            }
            else if (selectedMenuButtonIndex == 2) { // leaderboard
                gamestate = leaderboard;
                MainMenuMusic.stop();
            }
            else if (selectedMenuButtonIndex == 3) { // exit
                window.close();
                MainMenuMusic.stop();
            }

            menuInputDelay = 0.f;
        }
    }
}


void Start()
{
    // code here is only executed at the start of the program
    // initializations of everything

    window.setFramerateLimit(30);

    window.setMouseCursorVisible(false); // get rid of the mouse

    //Game font initialization
    MapInit();
    defgamefont.loadFromFile("VampireZone.ttf");
    swordspritesheet.loadFromFile("Assets\\SWORDS.png");
    healthbar_Texture.loadFromFile("Assets\\shanoahealthbar.png");

    MainmenuInit();
    GameOverInit();
    CharacterInit();
    MapInit();

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


        MainMenuInput();
        
        // changing cursor based on button it's on
        Vector2f selectedButtonPosition;
        Vector2f selectedButtonSize;

        if (selectedMenuButtonIndex == 0) {
            selectedButtonPosition = StartButton.getPosition();
            selectedButtonSize = StartButton.getSize();
        }
        else if (selectedMenuButtonIndex == 1) {
            selectedButtonPosition = SettingsButton.getPosition();
            selectedButtonSize = SettingsButton.getSize();
        }
        else if (selectedMenuButtonIndex == 2) {
            selectedButtonPosition = LeaderboardButton.getPosition();
            selectedButtonSize = LeaderboardButton.getSize();
        }
        else if (selectedMenuButtonIndex == 3) {
            selectedButtonPosition = ExitButton.getPosition();
            selectedButtonSize = ExitButton.getSize();
        }

        menuCursor.setPosition(selectedButtonPosition.x, selectedButtonPosition.y);
        menuCursor.setSize(Vector2f(selectedButtonSize.x-200, selectedButtonSize.y-45));

        view.setCenter(10000, 9800);

    }
    if (gamestate == gameloop)
    {
        // gameloop update


        //cout << "we are in game phase ";

        totalGameTime += deltaTime; // measure survival time
        shooting();
        for (int i = 0; i < swords.size(); i++)
        {
            swords[i].update(deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            view.setCenter(10000, 9800);
            MainMenuMusic.play();
        }
        if (Keyboard::isKeyPressed(Keyboard::Q) || shanoa.isDead)
        {
            int minutes = static_cast<int>(totalGameTime) / 60; // time calculations for final score
            int seconds = static_cast<int>(totalGameTime) % 60;

            stringstream ss; // time formatting
            ss << "Time: " << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
            ScoreText.setString(ss.str());

            if (!gameOverSoundPlayed)
            {
                //only play if it hasn't been played since the last reset
                GameOverSound.play();
                gameOverSoundPlayed = true; //set the flag so it doesn't play again immediately
            }

            gamestate = gameover;
            selectedMenuButtonIndex = 0;
        }
        shanoa.update();
        healthbarhandling();
        view.setCenter(shanoa.sprite.getPosition());
    }
    if (gamestate == settings)
    {
        // settings menu update
        cout << "we are in settings menu ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            MainMenuMusic.play();
            selectedMenuButtonIndex = 0;
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
            selectedMenuButtonIndex = 0;
        }
    }
    if (gamestate == gameover)
    {
        // gameover screen update
 
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            GameOverSound.stop();

            gamestate = mainmenu;
            view.setCenter(10000, 9800); // Center view back on main menu
            MainMenuMusic.play();
            //reset gameover sound so it works next time
            gameOverSoundPlayed = false;
            selectedMenuButtonIndex = 0;
        }
    }

    window.setView(view);
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

        window.draw(menuCursor);
        window.draw(StartGameText);
        window.draw(SettingsText);
        window.draw(LeaderboardText);
        window.draw(ExitText);

    }
    if (gamestate == gameloop)
    {
        // gameloop draw
        window.draw(Map);
        for (int i = 0; i < swords.size(); i++)
        {
          /*  window.draw(swords[i].collider);*/
            window.draw(swords[i].shape);
        }
        window.draw(healthbar);
        window.draw(shanoa.sprite);
    }
    if (gamestate == settings)
    {
        // settings menu draw


    }
    if (gamestate == gameover)
    {
        // gameover screen draw
        window.draw(Map);

        for (int i = 0; i < swords.size(); i++)
        {
            window.draw(swords[i].shape);
        }
        window.draw(shanoa.sprite);

        //DRAW ENEMIES HERE AFTER IMPLEMENTING

        gameOverOverlay.setPosition(view.getCenter());
        window.draw(gameOverOverlay);

        Vector2f viewCenter = view.getCenter();
        // Center horizontally by subtracting half of the text's width
        GameOverText.setPosition(viewCenter.x - GameOverText.getGlobalBounds().width / 2.f, viewCenter.y - 100.f);
        ScoreText.setPosition(viewCenter.x - ScoreText.getGlobalBounds().width / 2.f, viewCenter.y - 10.f);
        RestartText.setPosition(viewCenter.x - RestartText.getGlobalBounds().width / 2.f, viewCenter.y + 50.f);

        window.draw(GameOverText);
        window.draw(ScoreText);
        window.draw(RestartText);

    }

    window.display(); // Display sprites on screen
}
