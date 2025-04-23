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
Font defgamefont; // default game font
Texture MainMenuButtons_Texture,MainMenuBackground_Texture,Map_Texture;
Texture swordspritesheet;
Sprite MainMenuButtons, MainMenuBackground,Map;
View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;
RectangleShape StartButton(Vector2f(490, 110)),SettingsButton(Vector2f(490, 110)),LeaderboardButton(Vector2f(490, 110)),
               ExitButton(Vector2f(490, 110));
FloatRect StartButtonBounds,SettingsButtonBounds,LeaderboardButtonBounds,ExitButtonBounds;
Sound MainMenuMusic;
SoundBuffer MainMenuMusic_source;

RenderWindow window(VideoMode(1280, 800), "Vampire Survivors :The path to the legendary formula");
float shootingtime = 0;
float shootingrate = 3;

void Update();
void Start();
void Draw();
void MainMenuButtonCheck()
{
    if (Mouse::isButtonPressed(Mouse::Left) && StartButtonBounds.contains(mouseWorldpos))
    {
        gamestate = gameloop;
        view.setCenter(0, 0);
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
struct character
{
    RectangleShape collider; // Sprite collider
    Sprite sprite;
    Texture texture;
    Texture playerspreadsheet;
    int health, xp;
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
            if (Keyboard::isKeyPressed(Keyboard::A)) {
                sprite.move(-speed *deltaTime, 0);
                sprite.setScale(-1, 1.5);
                spriteDirection = toleft;
                AnimationState = walking;
            }
            if (Keyboard::isKeyPressed(Keyboard::D)) {
                sprite.move(speed * deltaTime, 0);
                sprite.setScale(1, 1.5);
                spriteDirection = toright;
                AnimationState = walking;

            }
            if (Keyboard::isKeyPressed(Keyboard::S)) {
                sprite.move(0, speed * deltaTime);
                AnimationState = walking;

            }
            if (Keyboard::isKeyPressed(Keyboard::W)) {
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
                if (columnIndex < 4)
                    sprite.setTextureRect(IntRect(columnIndex * 132, rowIndex * 143, 125, 148));
                else
                    sprite.setTextureRect(IntRect(columnIndex * 132 + 25, rowIndex * 143, 125, 148));
                break;
            case walking:
                rowIndex = 1;
                columnIndex %= 8;
                sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130, 120, 148));
                break;
            case idle:
                rowIndex = 0;
                columnIndex = columnIndex % 4;
                sprite.setTextureRect(IntRect(columnIndex * 111 + 30, rowIndex * 130, 120, 148));
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
        sword newSword;
        newSword.speed = 500;
        newSword.shape.setTexture(swordspritesheet);
        newSword.shape.setTextureRect(IntRect(1 * 32, 2 * 32, 32, 32));
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
   
}

void MapInit() {
    Map_Texture.loadFromFile("Assets\\mapfinal.png");
    Map_Texture.setRepeated(true);
    Map.setTextureRect(IntRect(0,0,20000,20000));
    Map.setTexture(Map_Texture);
    Map.setPosition(-10000, -10000);
}

void gameStateHandle()
{
    if (gamestate == mainmenu)
    {
        // main menu update

        MainMenuButtonCheck();
        view.setCenter(10000, 9800);

    }
    if (gamestate == gameloop)
    {
        // gameloop update
        //cout << "we are in game phase ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            view.setCenter(10000, 9800);
            MainMenuMusic.play();
            CharacterInit();
        }
        if (Keyboard::isKeyPressed(Keyboard::Q))
        {
            gamestate = gameover;
        }
        view.setCenter(shanoa.sprite.getPosition());
        shanoa.update();
    }
    if (gamestate == settings)
    {
        // settings menu update
        cout << "we are in settings menu ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            MainMenuMusic.play();
            /*CharacterInit();*/ //not sure delete it or leave it
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
            /*CharacterInit();*/ //not sure delete it or leave it
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
            CharacterInit();
        }
    }
}


void Start()
{
    // code here is only executed at the start of the program
    // initializations of everything

    window.setFramerateLimit(30);

    //Game font initialization
    MapInit();
    defgamefont.loadFromFile("VampireZone.ttf");
    swordspritesheet.loadFromFile("Assets\\SWORDS.png");

    MainmenuInit();
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
    gameStateHandle();
    window.setView(view);
    if (gamestate == gameloop)
    {
        shooting();
        for (int i = 0; i < swords.size(); i++)
        {
            swords[i].update(deltaTime);
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
        window.draw(Map);

        for (int i = 0; i < swords.size(); i++)
        {
          /*  window.draw(swords[i].collider);*/
            window.draw(swords[i].shape);
        }
        window.draw(shanoa.sprite);
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
