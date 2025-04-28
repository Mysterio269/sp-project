#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
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


Text mathRevivalText;
Text StartGameText, SettingsText, ExitText,LeaderboardText;
Text GameOverText, ScoreText, RestartText;
Font defgamefont; // default game font

Texture equationSpriteSheet;
Texture MainMenuButtons_Texture,MainMenuBackground_Texture,Map_Texture;
Texture swordspritesheet;
Sprite MainMenuButtons, MainMenuBackground,Map;

View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;

RectangleShape StartButton(Vector2f(490, 110)),SettingsButton(Vector2f(490, 110)),LeaderboardButton(Vector2f(490, 110)),
               ExitButton(Vector2f(490, 110)), MathRevivlaButton(Vector2f(250, 50)) ,restartButton(Vector2f(250, 50));
RectangleShape equationAnsCellBox;
RectangleShape gameOverOverlay; // red color in gameover background
FloatRect StartButtonBounds,SettingsButtonBounds,LeaderboardButtonBounds,ExitButtonBounds;
RectangleShape menuCursor;

int selectedMenuButtonIndex = 0; // 0 for Start, 1 for Settings, 2 for Leaderboard, 3 for Exit
int randIndex;// equations elements random index


Sound MainMenuMusic, GameOverSound;
SoundBuffer MainMenuMusic_source, GameOverSound_source;
bool gameOverSoundPlayed = false;

// RenderWindow window(VideoMode(1280, 800), "Vampire Survivors :The path to the legendary formula");
sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
RenderWindow window(desktopMode, "Vampire Survivors :The path to the legendary formula", sf::Style::Fullscreen);
//fullscreen fix

float shootingtime = 0.f;
float shootingrate = 3;
float deltaTime;
float totalGameTime = 0.f;
float menuInputDelay = 0.f;
const float MENU_INPUT_COOLDOWN = 0.2f; // Time in seconds between allowed inputs

void GetRandIndex(int &randomIndex);
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
struct MathEquation {
        Sprite sprite;
    Text userAnsText;
};
MathEquation SuvivalEquation;
int EquationsAns[8] = {3,4,4,2,7,4,1,6};
    string userInput = "";
    bool MathRevivalON;

int main()
{
    SuvivalEquation.userAnsText.setFont(defgamefont);
    SuvivalEquation.userAnsText.setCharacterSize(30);
    SuvivalEquation.userAnsText.setFillColor(sf::Color::White);
    srand(time(nullptr));
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
            //
            if (event.type == sf::Event::TextEntered)
            {
                if (gamestate == gameover)
                {
                    
                    if (event.text.unicode == 8)// isKeyPressed::Backspace
                    {
                        if (!userInput.empty())
                            userInput.pop_back(); 
                    }
                    else if (event.text.unicode >= '0' && event.text.unicode <= '9')
                    {
                        if (userInput.size() < 9) 
                        {
                            userInput += static_cast<char>(event.text.unicode);
                        }
                    }
                    else if (event.text.unicode == 13)// iskeypressed::Enter
                    {
                        if (!userInput.empty())
                        { 
                            if (stoi(userInput) == EquationsAns[randIndex]) // correct answer
                            {
                                gamestate = gameloop;
                                GameOverSound.stop();
                                MathRevivalON = false;
                            }
                            else
                            {
                                gamestate = mainmenu;
                                MainMenuMusic.play();
                                MathRevivalON = false;

                            }
                            userInput = "";
                        }
                    }
                }
            }
            SuvivalEquation.userAnsText.setString(userInput);
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
   
}

void MapInit() {
    Map_Texture.loadFromFile("Assets\\mapfinal.png");
    Map_Texture.setRepeated(true);
    Map.setTextureRect(IntRect(0,0,20000,20000));
    Map.setTexture(Map_Texture);
    Map.setPosition(-10000, -10000);
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
    RestartText.setString("return to Main Menu");
    RestartText.setCharacterSize(20);
    RestartText.setFillColor(Color::White);

    mathRevivalText.setFont(defgamefont);
    mathRevivalText.setString("Math Revival?");
    mathRevivalText.setCharacterSize(25);
    mathRevivalText.setFillColor(Color::White);

    equationSpriteSheet.loadFromFile("Assets\\equationSpriteSheet.png");
    SuvivalEquation.sprite.setTexture(equationSpriteSheet);
    SuvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));
    SuvivalEquation.sprite.setScale(0.3/1.5, 0.45/1.5);

    equationAnsCellBox.setFillColor(Color::Black);
    equationAnsCellBox.setSize(Vector2f(200, 50));
    equationAnsCellBox.setOutlineColor(Color::Red);
    equationAnsCellBox.setOutlineThickness(5);

    restartButton.setFillColor(Color(100, 0, 0));
    restartButton.setOutlineColor(Color::Yellow);
    restartButton.setOutlineThickness(5);

    MathRevivlaButton.setFillColor(Color(100, 0, 0));
    MathRevivlaButton.setOutlineColor(Color::Yellow);
    MathRevivlaButton.setOutlineThickness(5);



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

void GetRandIndex(int &randomIndex)
{randomIndex = rand() % 6;}


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

        GameOverSound.stop();
        MainMenuMusic.stop();
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

            GetRandIndex(randIndex);
            SuvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));

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
        
        if (Mouse::isButtonPressed(Mouse::Left) && restartButton.getGlobalBounds().contains(mouseScreenpos.x,mouseScreenpos.y))/// change to keyboard
        {
            GameOverSound.stop();
            gamestate = mainmenu;
            view.setCenter(10000, 9800); // Center view back on main menu
            MainMenuMusic.play();
            //reset gameover sound so it works next time
            gameOverSoundPlayed = false;
            selectedMenuButtonIndex = 0;
        }
        if (Mouse::isButtonPressed(Mouse::Left) /*&& MathRevivlaButton.getGlobalBounds().contains(mouseScreenpos.x, mouseScreenpos.y)*/) //// change to keyboard
        {
            MathRevivalON = true;
        }
    }
    if (gamestate != gameover)
    {
        userInput = ""; // delete the last user input   
        SuvivalEquation.userAnsText.setString("");
        if(gameOverSoundPlayed)
        {
         GameOverSound.stop();
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
        ScoreText.setPosition(viewCenter.x - ScoreText.getGlobalBounds().width / 2.f, viewCenter.y+10);

        RestartText.setPosition(viewCenter.x - RestartText.getGlobalBounds().width / 2.f, viewCenter.y + 100.f);
        restartButton.setPosition(viewCenter.x - restartButton.getGlobalBounds().width / 2.f, viewCenter.y + 90);//

        MathRevivlaButton.setPosition(viewCenter.x - MathRevivlaButton.getGlobalBounds().width / 2.f, viewCenter.y + 175.f);//
        mathRevivalText.setPosition(viewCenter.x - mathRevivalText.getGlobalBounds().width / 2.f, viewCenter.y + 185.f);//

        SuvivalEquation.sprite.setPosition(viewCenter.x - SuvivalEquation.sprite.getGlobalBounds().width / 2.f , viewCenter.y + 100.f);
        equationAnsCellBox.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f , viewCenter.y + 160.f);
        SuvivalEquation.userAnsText.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f +5, viewCenter.y + 163.f);//

        window.draw(GameOverText);
        window.draw(ScoreText);

        if (MathRevivalON)
        {
             window.draw(SuvivalEquation.sprite);
             window.draw(equationAnsCellBox);  
             window.draw(SuvivalEquation.userAnsText);
        } 
        else
        {
            window.draw(restartButton);
            window.draw(RestartText);
            window.draw(MathRevivlaButton);
            window.draw(mathRevivalText);
        }

       

    }

    window.display(); // Display sprites on screen
}
