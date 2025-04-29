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
#include <random>
using namespace std;
using namespace sf;
enum Gamestate
{
    mainmenu,
    gameloop,
    settings,
    gameover,
    leaderboard,
    credits
};
enum animationstate
{
    attacking, walking, idle, ultimate
};
enum playerDirection
{
    toright,toleft
};
enum monstertype
{
    Beast,
    Werewolf
};
Gamestate gamestate = mainmenu;
Text StartGameText, SettingsText, ExitText,LeaderboardText, CreditsText,volumeText,settingsmenuText;
Text DEV_T, TEAMNAME, NAMES, prof, teamname;
Text GameOverText, ScoreText, RestartText;
Font defgamefont; // default game font

Texture MainMenuButtons_Texture,MainMenuBackground_Texture,Map_Texture,healthbar_Texture, credits_Texture, credits_background
        ,volume_up_Texture,volume_down_Texture;

Text mathRevivalText;
Texture equationSpriteSheet;
Texture swordspritesheet;
Sprite MainMenuButtons, MainMenuBackground,Map,healthbar, creditsbutton, creditback, volume_up, volume_down,settingsBackground;
View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;

RectangleShape StartButton(Vector2f(490, 110)),SettingsButton(Vector2f(490, 110)),LeaderboardButton(Vector2f(490, 110)),
               ExitButton(Vector2f(490, 110)), creditsButton(Vector2f(490, 110)),MathRevivlaButton(Vector2f(250, 50)) ,restartButton(Vector2f(250, 50));
RectangleShape equationAnsCellBox;
RectangleShape gameOverOverlay; // red color in gameover background
FloatRect StartButtonBounds,SettingsButtonBounds,LeaderboardButtonBounds,ExitButtonBounds, creditsButtonBounds,volumeUpBounds,
          volumeDownBounds;
RectangleShape menuCursor;
Text nametext;
Listener GameVolume;
int selectedMenuButtonIndex = 0; // 0 for Start, 1 for Settings, 2 for Leaderboard, 3 for Exit
float volumebarcontroller;
int randIndex;// equations elements random index

Sound MainMenuMusic, GameOverSound,GameloopMusic;
SoundBuffer MainMenuMusic_source, GameOverSound_source,GameloopMusic_source;
bool gameOverSoundPlayed = false;

sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
RenderWindow window(desktopMode, "Vampire Survivors :The path to the legendary formula", sf::Style::Fullscreen);
//fullscreen fix
int mobCounter = 4;
float spawnTime = 3.f;
float healthRatio;
float shootingtime = 0.f;
float shootingrate = 3;
float deltaTime;
float totalGameTime = 0.f;
float menuInputDelay = 0.f;
const float MENU_INPUT_COOLDOWN = 0.2f; // Time in seconds between allowed inputs
float soundcontroller = 100;
RectangleShape volumebar[10];
void creditsInit();
void unitVector();
void generalCollision();
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
    bool startattack = false;


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
            if (Keyboard::isKeyPressed(Keyboard::E)){
                AnimationState = attacking;
                if (startattack == false) {
                    columnIndex = 0;
                    startattack =  true;
                }
            }
            collider.setPosition(sprite.getPosition());
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
                startattack = false;
                break;
            case idle:
                rowIndex = 0;
                columnIndex = columnIndex % 4;
                sprite.setTextureRect(IntRect(columnIndex * 111 + 30, rowIndex * 130 - 5, 120, 148));
                startattack = false;
                break;
            }
        }
    }
} shanoa;

struct enemy
{
    RectangleShape attackBox,collider;
    Vector2f velocity;
    monstertype MonsterType;
    Sprite shape;
    Texture enemyspreadsheet;
    animationstate AnimationState;
    float speed;
    int columnindex, rowindex;
    int health,damage;
    bool isAttacking, isDead;
    void update()
    {
        if (health <= 0)
            isDead = true;
    }
} beast,werewolf;

vector<enemy> enemies;

struct sword {
    Sprite shape;
    RectangleShape collider;
    Vector2f velocity;
    float speed;
    float deletiontimer = 0;

    void update()
    {
        shape.move(velocity * deltaTime);
        collider.setPosition(shape.getPosition());
        collider.setOrigin(shape.getOrigin());
        shape.setOrigin(16, 16);
        collider.setOrigin(15, 7.5);
        deletiontimer += deltaTime;
    }
    
};
    vector<sword> swords; 

string names[11] = {
    "Developers :",
    "Mahmoud Abd Elmegeed",
    "Ahmed Hossam",
    "Fares Mohamed",
    "Youssef Mahmoud",
    "Youssef Wael",
    "Youssef Tamer",
    "Amr Sameh",
    " ",
    "Voice Actor (Math Revival) :",
    "Prof : Mohamed Ibrahem"
};
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
        newSword.speed = 10000 * deltaTime;
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
    }

}

void MainmenuInit() {
    //Main menu initializations
    //please avoid editing this section
    GameVolume.setGlobalVolume(soundcontroller);
    MainMenuMusic.setVolume(30);

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

    ExitButton.setPosition(9853, 10186);
    ExitButton.setFillColor(Color::Red);
    ExitButton.setScale(0.6, 0.65);
    ExitButtonBounds = ExitButton.getGlobalBounds();
    ExitText.setFont(defgamefont);
    ExitText.setString("Exit");
    ExitText.setPosition(9970, 10199);

    creditsButton.setPosition(9853, 10094); 
    creditsButton.setFillColor(Color::Red);
    creditsButton.setScale(0.6, 0.65);
    CreditsText.setPosition(9940, 10107);
    creditsButtonBounds = creditsButton.getGlobalBounds();
    CreditsText.setFont(defgamefont);
    CreditsText.setString("Credits");

    MainMenuButtons_Texture.loadFromFile("Assets\\mainmenu.png");
    MainMenuButtons.setTexture(MainMenuButtons_Texture);
    MainMenuButtons.setOrigin(281, 325);
    MainMenuButtons.setPosition(10000, 10000);
    MainMenuButtons.setScale(0.6, 0.65);

    credits_Texture.loadFromFile("Assets\\creditsbutton.png");
    creditsbutton.setTexture(credits_Texture);
    creditsButton.setOrigin(490 / 2, 110 / 2);
    creditsbutton.setPosition(9832, 10165);
    creditsbutton.setScale(0.6, 0.65);

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

    credits_background.loadFromFile("Assets\\MainBackground2.png");
    creditback.setTexture(credits_background);
    creditback.setScale(0.84, 1.12);
    creditback.setPosition(9340, 9300);


    GameloopMusic_source.loadFromFile("Assets\\gameloopost.ogg");
    GameloopMusic.setBuffer(GameloopMusic_source);
    GameloopMusic.setVolume(5);
    GameloopMusic.setLoop(true);

    defgamefont.loadFromFile("VampireZone.ttf");
    swordspritesheet.loadFromFile("Assets\\SWORDS.png");
    healthbar_Texture.loadFromFile("Assets\\shanoahealthbar.png");
}

void creditsInit()
{
    DEV_T.setPosition(9800, 9330);
    DEV_T.setCharacterSize(50);
    DEV_T.setFillColor(Color::White);
    DEV_T.setFont(defgamefont);
    DEV_T.setString("Development_Team");

    TEAMNAME.setPosition(9530, 9450);
    TEAMNAME.setCharacterSize(50);
    TEAMNAME.setFillColor(Color::White);
    TEAMNAME.setFont(defgamefont);
    TEAMNAME.setString("Team Name :");

    teamname.setPosition(9850, 9450);
    teamname.setCharacterSize(50);
    teamname.setFillColor(Color::Red);
    teamname.setFont(defgamefont);
    teamname.setString("Runtime Horror");


}

void CharacterInit() {
    shanoa.texture.loadFromFile("Assets\\shanoa.png");
    shanoa.sprite.setTexture(shanoa.texture);
    shanoa.speed = 200;
    shanoa.sprite.setPosition(0, 0);
    shanoa.collider.setSize(Vector2f(60, 125));
    shanoa.collider.setOrigin(shanoa.collider.getLocalBounds().width/2,shanoa.collider.getLocalBounds().height/2);
    shanoa.collider.setFillColor(Color::Blue);
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
Vector2f unitVector(Vector2f vector) {
    float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
    Vector2f unit = Vector2f(vector.x / magnitude, vector.y / magnitude);
    return unit;
}

void playertargeting(enemy& enemy1)
{
    enemy1.velocity = shanoa.sprite.getPosition() - enemy1.shape.getPosition();
    enemy1.velocity = unitVector(enemy1.velocity) * enemy1.speed;
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
    healthbar.setPosition(shanoa.sprite.getPosition().x - 500, shanoa.sprite.getPosition().y + 285);
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

void MainMenuButtonCheck()
{
    if (StartButtonBounds.contains(mouseWorldpos))
    {
        selectedMenuButtonIndex = 0;
        if(Mouse::isButtonPressed(Mouse::Left))
        {
            gamestate = gameloop;
            MainMenuMusic.stop();
            GameloopMusic.play();
        }
    }
    if (LeaderboardButtonBounds.contains(mouseWorldpos)) {
        selectedMenuButtonIndex = 2;
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            gamestate = leaderboard;
        }
    }
    if (SettingsButtonBounds.contains(mouseWorldpos)) {
        selectedMenuButtonIndex = 1;
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            gamestate = settings;
        }
    }
    if (ExitButtonBounds.contains(mouseWorldpos))
    {
        selectedMenuButtonIndex = 4;
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            window.close();
        }
    }
    if (creditsButtonBounds.contains(mouseWorldpos))
    {
        selectedMenuButtonIndex = 3;
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            gamestate = credits;
        }
        
    }
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
                selectedMenuButtonIndex = 4; 
            }
            else if (selectedMenuButtonIndex > 4) { 
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
                GameloopMusic.play();

                //RESETTING after death for next game
                shanoa.health = 120; // <--replace 100 with your actual starting health
                shanoa.isDead = false;
                totalGameTime = 0.f;
                swords.clear(); // Clear any old swords when starting a NEW game

                //RESET MONSTERS HERE WHEN IMPLEMENTED

            }
            else if (selectedMenuButtonIndex == 1) { // settings
                gamestate = settings;
            }
            else if (selectedMenuButtonIndex == 2) { // leaderboard
                gamestate = leaderboard;
            }
            else if (selectedMenuButtonIndex == 3) { // credits
                gamestate = credits;
            }
            else if (selectedMenuButtonIndex == 4) { // exit
                window.close();
            }

            menuInputDelay = 0.f;
        }
    }
}

void SettingsMenuInit() {
    volume_up_Texture.loadFromFile("Assets\\volume_up.png");
    volume_up.setTexture(volume_up_Texture);
    volume_up.setPosition(1100, 20280);
    volume_up.setScale(-0.63, 0.7);
    volumeUpBounds = volume_up.getGlobalBounds();

    volume_down_Texture.loadFromFile("Assets\\volume_down.png");
    volume_down.setTexture(volume_down_Texture);
    volume_down.setPosition(1000, 20280);
    volume_down.setScale(-0.63, 0.7);
    volumeDownBounds = volume_down.getGlobalBounds();

    settingsBackground.setTexture(credits_background);
    settingsBackground.setColor(Color(60, 60, 60));
    settingsBackground.setPosition(0, 20000);

    volumeText.setString("Volume");
    volumeText.setFont(defgamefont);
    volumeText.setPosition(520, 20300);
    volumeText.setScale(1.2, 1.2);

    settingsmenuText.setFont(defgamefont);
    settingsmenuText.setString("Settings");
    settingsmenuText.setPosition(680, 20100);
    settingsmenuText.setScale(1.8, 1.8);

    {//volume bar init
        for (int i = 0;i < 10;i++) {
            volumebar[i].setSize(Vector2f(15, 30));
            volumebar[i].setFillColor(Color::White);
            volumebar[i].setPosition(690 + (i * 20), 20307);
       }
    }
}
void generalCollision(RectangleShape& objectTOBeMovedCollider, RectangleShape& Wall, Sprite& Object) {
    if (objectTOBeMovedCollider.getGlobalBounds().intersects(Wall.getGlobalBounds())) {
        FloatRect objectBound = objectTOBeMovedCollider.getGlobalBounds();
        FloatRect intersection;
        FloatRect wall = Wall.getGlobalBounds();
        if (objectBound.intersects(wall)) {
            objectBound.intersects(wall, intersection);
            //left or right
            if (intersection.height > intersection.width) {
                // left coll
                if (objectBound.left > wall.left) {
                    cout << "l";
                    Object.move(intersection.width, 0);
                }
                else { // right
                    Object.move(-intersection.width, 0);
                    cout << "r";
                }
            }
            else if (intersection.height < intersection.width) {

                if (objectBound.top > wall.top) {
                    Object.move(0, intersection.height);
                    cout << "Lower";
                }
                else {
                    Object.move(0, -intersection.height);
                    cout << "upper";

                }

            }
        } // upper/lower 
    }
}



void BeastInit() {
    beast.MonsterType = Beast;
    beast.enemyspreadsheet.loadFromFile("Assets\\beastTexture.png");
    beast.shape.setTexture(beast.enemyspreadsheet);
    beast.shape.setScale(1.8,2.5);
    beast.health = 200;
    beast.speed = 150;
    beast.velocity = Vector2f(beast.speed, beast.speed);
    beast.damage = 15;
    beast.attackBox.setSize(Vector2f(150, 150));
    beast.shape.setPosition(-200, -200);
    beast.shape.setOrigin(64,32);
    beast.AnimationState = walking;
    beast.collider.setFillColor(Color::Yellow);
    beast.collider.setSize(Vector2f(60, 120));
    beast.collider.setOrigin(beast.collider.getLocalBounds().width / 2, beast.collider.getLocalBounds().height / 2);
    beast.attackBox.setFillColor(Color::Red);
    beast.attackBox.setOrigin(beast.attackBox.getLocalBounds().width / 2, beast.attackBox.getLocalBounds().height / 2);
}
void AttackDetection(RectangleShape &playerCollider, RectangleShape &enemyCollider , enemy &theEnemy) {
    if (playerCollider.getGlobalBounds().intersects(enemyCollider.getGlobalBounds())) {
        theEnemy.AnimationState = attacking;
    }
    else
        theEnemy.AnimationState = walking;
}

void beastUpdate() {
    beast.collider.setPosition(beast.shape.getPosition());
    beast.attackBox.setPosition(beast.shape.getPosition());
    if (beast.AnimationState == walking) {
        beast.rowindex = 1;
        beast.columnindex =(beast.columnindex + 1) % 4;
    }
    else if (beast.AnimationState == attacking) {
        beast.rowindex = 2;
        beast.columnindex = (beast.columnindex + 1) % 8;
    }
    beast.shape.setTextureRect(IntRect(128 * beast.columnindex, 64 * beast.rowindex, 128, 64));
    playertargeting(beast);
    beast.shape.move(beast.velocity * deltaTime);
    if (beast.shape.getPosition().x < shanoa.sprite.getPosition().x)
        beast.shape.setScale(1.8, 2.5);
    else
        beast.shape.setScale(-1.8, 2.5);
    AttackDetection(shanoa.collider, beast.collider, beast);

}


void werewolfInit() {
    werewolf.MonsterType = Werewolf;
    werewolf.shape.setTexture(werewolf.enemyspreadsheet);
    werewolf.enemyspreadsheet.loadFromFile("Assets\\werewolfwhite.png");
    werewolf.shape.setPosition(200, 200);
    werewolf.speed = 175;
    werewolf.health = 150;
    werewolf.damage = 10;
    werewolf.AnimationState = walking;
    werewolf.attackBox.setSize(Vector2f(175, 175));
    werewolf.attackBox.setFillColor(Color::Magenta);
    werewolf.attackBox.setOrigin(werewolf.attackBox.getLocalBounds().width / 2, werewolf.attackBox.getLocalBounds().height / 2);
    werewolf.collider.setSize(Vector2f(128, 128));
    werewolf.collider.setFillColor(Color::Green);
    werewolf.collider.setOrigin(werewolf.collider.getLocalBounds().width / 2, werewolf.collider.getLocalBounds().height / 2);
    werewolf.shape.setOrigin(64,73);

}

void werewolfupdate()
{

    if (werewolf.AnimationState == walking)
    {
        // walking 
        werewolf.rowindex = 1;
        werewolf.shape.setTextureRect(IntRect(werewolf.columnindex * 128, 145.666 * werewolf.rowindex, 128, 146));
        werewolf.columnindex = (werewolf.columnindex + 1) % 8;

    }
    else if (werewolf.AnimationState == attacking) {
        // attacking /
        werewolf.rowindex = 2;
        werewolf.shape.setTextureRect(IntRect(werewolf.columnindex * 128, werewolf.rowindex * 145.666, 128, 146));
        werewolf.columnindex = (werewolf.columnindex + 1) % 6;
    }
    playertargeting(werewolf);
    werewolf.shape.move(werewolf.velocity * deltaTime);
    werewolf.attackBox.setPosition(werewolf.shape.getPosition());
    werewolf.collider.setPosition(werewolf.shape.getPosition());
    if (werewolf.shape.getPosition().x < shanoa.sprite.getPosition().x)
        werewolf.shape.setScale(1, 1);
    else
        werewolf.shape.setScale(-1, 1);
    AttackDetection(shanoa.collider, werewolf.collider, werewolf);
}
void beastGlobalStart(enemy beastMob) {
    beastMob.MonsterType = Beast;
    beastMob.enemyspreadsheet.loadFromFile("Assets\\beastTexture.png");
    beastMob.shape.setTexture(beastMob.enemyspreadsheet);
    beastMob.shape.setScale(1.8, 2.5);
    beastMob.health = 200;
    beastMob.speed = 150;
    beastMob.velocity = Vector2f(beastMob.speed, beastMob.speed);
    beastMob.damage = 15;
    beastMob.attackBox.setSize(Vector2f(150, 150));
    beastMob.shape.setPosition(-200, -200);
    beastMob.shape.setOrigin(64, 32);
    beastMob.AnimationState = walking;
    beastMob.collider.setFillColor(Color::Yellow);
    beastMob.collider.setSize(Vector2f(60, 120));
    beastMob.collider.setOrigin(beastMob.collider.getLocalBounds().width / 2, beastMob.collider.getLocalBounds().height / 2);
    beastMob.attackBox.setFillColor(Color::Red);
    beastMob.attackBox.setOrigin(beastMob.attackBox.getLocalBounds().width / 2, beastMob.attackBox.getLocalBounds().height / 2);
}
void beastMobGlobalUpdate(enemy beastMob) {
    beastMob.collider.setPosition(beastMob.shape.getPosition());
    beastMob.attackBox.setPosition(beastMob.shape.getPosition());
    if (beastMob.AnimationState == walking) {
        beastMob.rowindex = 1;
        beastMob.columnindex = (beastMob.columnindex + 1) % 4;
    }
    else if (beastMob.AnimationState == attacking) {
        beastMob.rowindex = 2;
        beastMob.columnindex = (beastMob.columnindex + 1) % 8;
    }
    beastMob.shape.setTextureRect(IntRect(128 * beastMob.columnindex, 64 * beastMob.rowindex, 128, 64));
    playertargeting(beastMob);
    beastMob.shape.move(beastMob.velocity * deltaTime);
    if (beastMob.shape.getPosition().x < shanoa.sprite.getPosition().x)
        beastMob.shape.setScale(1.8, 2.5);
    else
        beastMob.shape.setScale(-1.8, 2.5);
    AttackDetection(shanoa.collider, beastMob.collider, beastMob);
}

void GetRandIndex(int &randomIndex)
{randomIndex = rand() % 6;}






vector <enemy> mobSpawned;
int spawnTimerDetector = 0, currentMobCounter = 0;
void Spawn() {
    /*spawnTimerDetector += deltaTime;*/
    if (Keyboard::isKeyPressed(Keyboard::P)){
        spawnTimerDetector = 0;
        ++currentMobCounter;
        enemy newBeast;
        beastGlobalStart(newBeast);
        newBeast.shape.setPosition(100, 100);
        mobSpawned.push_back(newBeast);
    }
}
 
   









void swordFullCollision() {
    for (int i = 0; i < swords.size(); ++i) {
        swords[i].update();
        /*------------------enemys-----------------*/
        bool beastDetection = swords[i].collider.getGlobalBounds().intersects(beast.collider.getGlobalBounds());
        bool wereWolfDetection = swords[i].collider.getGlobalBounds().intersects(werewolf.collider.getGlobalBounds());







        /*------------------obstacles-----------------*/





        /*-----------------All Conditions---------------*/
        if (beastDetection or wereWolfDetection) {
            swords.erase(swords.begin() + i);
            break;
        }
    }
}




// For All Game Collision
void globalCollsion() {
    generalCollision(beast.collider, shanoa.collider, beast.shape);
    generalCollision(werewolf.collider, shanoa.collider, werewolf.shape);
    swordFullCollision();
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
    GameOverInit();
    CharacterInit();
    MapInit();
    creditsInit();
    SettingsMenuInit();
    BeastInit();
    werewolfInit();

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
        window.setMouseCursorVisible(true);

        MainMenuInput();
        MainMenuButtonCheck();


        
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
            view.setCenter(0, 20000);
        }
        else if (selectedMenuButtonIndex == 2) {
            selectedButtonPosition = LeaderboardButton.getPosition();
            selectedButtonSize = LeaderboardButton.getSize();
        }
        else if (selectedMenuButtonIndex == 3) {
            selectedButtonPosition = creditsButton.getPosition();
            selectedButtonSize = creditsButton.getSize();
        }
        else if (selectedMenuButtonIndex == 4) {
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

        window.setMouseCursorVisible(false); // get rid of the mouse
        //cout << "we are in game phase ";

        totalGameTime += deltaTime; // measure survival time

        GameOverSound.stop();
        MainMenuMusic.stop();
        shooting();
        for (int i = 0; i < swords.size(); i++)
        {
            swords[i].update();
            if (swords[i].deletiontimer > 10 ) {
                swords.erase(swords.begin() + i);
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            view.setCenter(10000, 9800);
            GameloopMusic.stop();
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

            GameloopMusic.stop();
            gamestate = gameover;
            selectedMenuButtonIndex = 0;
        }

        Spawn();
        for (int i = 0; i < mobSpawned.size(); ++i) {
            beastMobGlobalUpdate(mobSpawned[i]);
        }
        beastUpdate();
        werewolfupdate();
        shanoa.update();
        globalCollsion();
        healthbarhandling();
        view.setCenter(shanoa.sprite.getPosition());
    }

    if (gamestate == settings)
    {
        // settings menu update
        window.setMouseCursorVisible(true);
        volumebarcontroller = soundcontroller / 100;
        view.setCenter(800, 20500);
        volume_down.setColor(Color::White);
        volume_up.setColor(Color::White);
        if (volumeUpBounds.contains(mouseWorldpos)) {
            volume_up.setColor(Color::Yellow);
            if (Mouse::isButtonPressed(Mouse::Left)) {
                soundcontroller += 5;
            }
        }
        if (volumeDownBounds.contains(mouseWorldpos)) {
            volume_down.setColor(Color::Yellow);
            if (Mouse::isButtonPressed(Mouse::Left)) {
                soundcontroller -= 5;
            }
        }
        if (soundcontroller > 100)
            soundcontroller = 100;
        else if (soundcontroller < 0)
            soundcontroller = 0;

        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            selectedMenuButtonIndex = 0;
        }
    }

    if (gamestate == leaderboard)
    {
        // settings menu update
        window.setMouseCursorVisible(true);
        cout << "we are in leaderboard menu ";
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            selectedMenuButtonIndex = 0;
        }
    }

    if (gamestate == gameover)
    {
        // gameover screen update
        window.setMouseCursorVisible(true);
 
        if (Keyboard::isKeyPressed(Keyboard::R))
        
        if (Mouse::isButtonPressed(Mouse::Left) && restartButton.getGlobalBounds().contains(mouseScreenpos.x,mouseScreenpos.y))/// change to keyboard
        {
            GameOverSound.stop();
            gamestate = mainmenu;
            view.setCenter(10000, 9800); // Center view back on main menu
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

    if (gamestate == credits) {
        creditback.setColor(Color(70, 70, 70));
        view.setCenter(10000, 9800);
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
        }
    }

    GameVolume.setGlobalVolume(soundcontroller);
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
        window.draw(creditsbutton);
        window.draw(menuCursor);
        window.draw(StartGameText);
        window.draw(SettingsText);
        window.draw(LeaderboardText);
        window.draw(CreditsText);
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
        window.draw(beast.shape);
        window.draw(werewolf.shape);
        window.draw(healthbar);
        window.draw(shanoa.sprite);
        for (int i = 0; i < mobSpawned.size(); ++i) {
            window.draw(mobSpawned[i].shape);        
        }
    }

    if (gamestate == settings)
    {
        // settings menu draw
        window.draw(settingsBackground);
        window.draw(volume_down);
        window.draw(volume_up);
        window.draw(settingsmenuText);
        window.draw(volumeText);
        for (int i = 0;i < (volumebarcontroller * 10);i++) {
            window.draw(volumebar[i]);
        }
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

    if (gamestate == credits)
    {
        window.draw(creditback);
        window.draw(DEV_T);
        window.draw(TEAMNAME);
        window.draw(teamname);

        for (int i = 0; i < 11; ++i) {
            Text nameText;
            nameText.setFont(defgamefont);
            nameText.setString(names[i]);
            nameText.setCharacterSize(50);
            nameText.setFillColor(Color::White);
            nameText.setPosition(9530, 9530 + i * 60);

            window.draw(nameText);

        }

    }

    window.display(); // Display sprites on screen
}
