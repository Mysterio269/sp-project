#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include <map>
using namespace std;
using namespace sf;

enum Gamestate
{
    mainmenu,
    gameloop,
    settings,
    gameover,
    leaderboard,
    credits,
    nameinput,
    paused
};

enum CrystalState
{
    IDLE, REPEL, ATTRACT, ABSORB
};

enum animationstate
{
    attacking, walking, idle, ultimate
};

enum playerDirection
{
    toright, toleft
};

enum monstertype
{
    Beast,
    Werewolf,
    Zombie,
    Bat
};

float deltaTime;
Texture MainMenuButtons_Texture, MainMenuBackground_Texture, Map_Texture, healthbar_Texture, credits_Texture, credits_background
, volume_up_Texture, volume_down_Texture;
Texture equationSpriteSheet;
Texture swordspritesheet;
Texture beasttexture, zombieTexture, batTexture, werewolfTexture;
Texture BlueXP, GreenXP, RedXP;


int BlueXPcValue = 10, GreenXPcValue = 30, RedXPcValue = 50;

multimap<float, std::string> leaderboardEntriesMap; // Key: -score (float), Value: playerName (string)

void PauseMenuInit();
void handleNameInput(sf::Event& event);
void SaveLeaderboard();
void LoadLeaderboard();
void MapInit();
void creditsInit();
void generalCollision(RectangleShape& objectTOBeMovedCollider, RectangleShape& Wall, Sprite& Object);
void GetRandIndex(int& randomIndex);
void MainMenuInput();
void Update();
void Start();
void Draw();

Vector2f unitVector(Vector2f vector) {
    float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
    if (magnitude == 0) {
        return Vector2f(0, 0); // Or handle as needed, perhaps return the zero vector
    }
    Vector2f unit = Vector2f(vector.x / magnitude, vector.y / magnitude);
    return unit;
}

struct character
{
    RectangleShape collider; // Sprite collider
    Sprite sprite;
    Texture texture;
    Texture playerspreadsheet;
    float health = 120, xp;
    float Maxhp = 120;
    float speed;
    int level = 1;
    Vector2f velocity;
    bool isDead;
    animationstate AnimationState;
    playerDirection spriteDirection;
    int columnIndex = 0;
    int rowIndex = 0;
    int animationdelaytimer = 0;
    bool startattack = false;


    // *** Add members for revival tracking and score calculation ***
    bool hasRevived = false;
    float timeAtFirstDeath = 0.f; // totalGameTime when the player first died


    void update()
    {
        //spriteDirection = toright;
        AnimationState = idle;
        animationdelaytimer++;
        if (animationdelaytimer > 2) {
            columnIndex++;
            animationdelaytimer = 0;
        }

        if (health <= 0)
            isDead = true;

        //movement
        {
            if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) {
                sprite.move(-speed * deltaTime, 0);
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
                if (startattack == false) {
                    columnIndex = 0;
                    startattack = true;
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
                if (columnIndex == 0)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130 + 13, 120, 148));
                else if (columnIndex == 1)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130 + 18, 120, 148));
                else if (columnIndex == 2)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130 + 18, 160, 148));
                else if (columnIndex == 3)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 90, rowIndex * 130 + 18, 160, 148));
                else if (columnIndex == 4)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 135, rowIndex * 130 + 11, 160, 148));
                else if (columnIndex == 5)
                    sprite.setTextureRect(IntRect(columnIndex * 98 + 195, rowIndex * 130 + 8, 160, 140));
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

struct XPc
{
    Sprite sprite;
    bool isCollected = false;
    bool CrystalsRemove = false;
    int xpValue;
    float lifetime = 35.0f;  // Crystal will exist for 35 seconds 
    CrystalState state = IDLE;

    // Animation parameters
    float repelDistance = 150.0f;  // Maximum distance to repel
    float repelSpeed = 300.0f;     // Speed of repelling
    float attractRange = 150.0f;   // Range at which crystals start getting attracted
    float attractSpeed = 190.0f;   // Base speed for attraction
    float absorbSpeed = 550.0f;    // Speed during final absorption
    float absorbDistance = 75.0f;  // Distance at which absorption animation starts

    // Timing
    float repelTimer = 0.0f;
    float maxRepelTime = 1.1f;     // Time it takes to complete repel animation

    // Movement vector
    Vector2f direction;

    XPc(float x, float y, Texture& tex, int xp) {
        sprite.setTexture(tex);
        sprite.setPosition(x, y);
        sprite.setScale(0.2f, 0.4f);

        // Center the origin for proper positioning
        sprite.setOrigin(tex.getSize().x / 2.0f, tex.getSize().y / 2.0f);

        sprite.setTextureRect(IntRect(0, 0, tex.getSize().x, tex.getSize().y));
        xpValue = xp;

    }

    void update(float deltaTime, const Vector2f& playerPosition)
    {
        // Update lifetime regardless of state
        lifetime -= deltaTime;
        if (lifetime <= 0.0f) {
            CrystalsRemove = true;
            return;
        }

        // Calculate distance to player
        Vector2f toPlayer = playerPosition - sprite.getPosition();
        float mag = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        // Normalize direction to player
        Vector2f normalizedToPlayer = toPlayer;
        if (mag > 0) {
            normalizedToPlayer.x /= mag;
            normalizedToPlayer.y /= mag;
        }

        // Animation state machine
        switch (state) {
        case IDLE:
            state = REPEL;
            break;

        case REPEL:
            // Move away from player initially
            repelTimer += deltaTime;

            if (repelTimer < maxRepelTime)
            {
                float currentRepelSpeed = repelSpeed * (1.0f - repelTimer / maxRepelTime); // Slow down as it reaches max distance
                sprite.move(direction * currentRepelSpeed * deltaTime);
            }
            else
            {
                state = (mag <= attractRange) ? ATTRACT : IDLE;
            }
            break;

        case ATTRACT:
            // Move toward player with increasing speed based on distance
            if (mag <= absorbDistance)
            {
                // When very close, transition to final absorption
                state = ABSORB;
            }
            else
            {
                float speedFactor = 1.0f + (attractRange - mag) / attractRange * 2.0f;
                sprite.move(normalizedToPlayer * attractSpeed * speedFactor * deltaTime);

            }
            break;

        case ABSORB:
            // Final quick absorption movement
            sprite.move(normalizedToPlayer * absorbSpeed * deltaTime);

            // Shrink effect
            float currentScale = sprite.getScale().x - 0.6f * deltaTime;
            if (currentScale <= 0.05f)
            {
                isCollected = true;
            }
            else
            {
                sprite.setScale(currentScale, currentScale * 2.0f);
            }
            break;
        }

        if (state == IDLE && mag <= attractRange)
        {
            state = ATTRACT;
        }
    }

    FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

struct ENEMY
{
    RectangleShape attackBox, collider;
    Vector2f velocity;
    monstertype MonsterType;
    Sprite shape;
    Texture enemyspreadsheet;
    animationstate AnimationState;
    float speed, animationdelaytimer;
    int columnindex = 0, rowindex;
    int health, damage;
    bool isAttacking = false, isDead = false, hasDroppedXP = false;
    virtual void start() {
        //virtual start to be edited afterwards in the code
    }
    virtual void update() {}
    virtual ~ENEMY() {}
};
vector<XPc> Crystals;

struct BEAST :public ENEMY
{
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds())) {
            AnimationState = attacking;
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Beast;
        shape.setTexture(beasttexture);
        shape.setScale(1.8, 2.5);
        health = 250;
        speed = 100;
        damage = 25;
        attackBox.setSize(Vector2f(130, 1));
        shape.setPosition(-200, -200);
        shape.setOrigin(64, 32);
        AnimationState = walking;
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(60, 120));
        collider.setOrigin(collider.getLocalBounds().width / 2, collider.getLocalBounds().height / 2);
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        AttackDetection();
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 1;
            columnindex = columnindex % 4;
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 2;
            columnindex = columnindex % 8;
        }
        shape.setTextureRect(IntRect(128 * columnindex + 10, 64 * rowindex, 128, 64));
        playertargeting();
        if (shape.getPosition().x > shanoa.sprite.getPosition().x) {
            shape.setScale(-1.8, 2.5);
        }
        else {
            shape.setScale(1.8, 2.5);
        }
        if (health <= 0) {
            isDead = true;
            health = 0;
            shape.setScale(0, 0);
        }
        if (isDead && hasDroppedXP == false)
        {
            Vector2f position = shape.getPosition();

            // Randomly decide: 5% chance for Red crystal
            int randValue = (rand() + 1) % 100;
            if (randValue < 5)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue));

            else if (randValue > 5 && randValue < 15)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue));

            hasDroppedXP = true;
        }
    }
}beast;

struct ZOMBIE :public ENEMY
{
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds())) {
            AnimationState = attacking;
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Zombie;
        shape.setTexture(zombieTexture);
        shape.setScale(2.52, 3.5);
        health = 100;
        speed = 120;
        damage = 20;
        attackBox.setSize(Vector2f(65, 1));
        shape.setPosition(200, -200);
        shape.setOrigin(16, 16);
        AnimationState = walking;
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(60, 120));
        collider.setOrigin(collider.getLocalBounds().width / 2, collider.getLocalBounds().height / 2);
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        AttackDetection();
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 2;
            columnindex = columnindex % 8;
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 1;
            columnindex = columnindex % 7;
        }
        shape.setTextureRect(IntRect(32 * columnindex, 32 * rowindex, 32, 32));
        playertargeting();
        if (shape.getPosition().x > shanoa.sprite.getPosition().x) {
            shape.setScale(-2.52, 3.5);
        }
        else {
            shape.setScale(2.52, 3.5);
        }
        if (health <= 0) {
            isDead = true;
            health = 0;
            shape.setScale(0, 0);
        }
        if (isDead && hasDroppedXP == false)
        {
            Vector2f position = shape.getPosition();

            // Randomly decide: 5% chance for Red crystal
            int randValue = (rand() + 1) % 100;
            if (randValue < 5)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue));

            else if (randValue > 5 && randValue < 15)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue));

            hasDroppedXP = true;
        }
    }
}zombie;

struct WEREWOLF :public ENEMY
{
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds())) {
            AnimationState = attacking;
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Werewolf;
        shape.setTexture(werewolfTexture);
        shape.setPosition(200, 200);
        speed = 175;
        health = 150;
        damage = 10;
        AnimationState = walking;
        attackBox.setSize(Vector2f(70, 1));
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(128, 128));
        collider.setOrigin(collider.getLocalBounds().width / 2, collider.getLocalBounds().height / 2);
        shape.setOrigin(64, 73);
    }
    void update() override {
        AttackDetection();
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 1;
            columnindex = columnindex % 8;
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 2;
            columnindex = columnindex % 6;
        }
        shape.setTextureRect(IntRect(128 * columnindex, 145.666 * rowindex, 128, 145.666));
        if (shape.getPosition().x > shanoa.sprite.getPosition().x) {
            shape.setScale(-1, 1);
        }
        else {
            shape.setScale(1, 1);
        }
        playertargeting();
        if (health <= 0) {
            isDead = true;
            health = 0;
            shape.setScale(0, 0);
        }
        if (isDead && hasDroppedXP == false)
        {
            Vector2f position = shape.getPosition();

            // Randomly decide: 5% chance for Red crystal
            int randValue = (rand() + 1) % 100;
            if (randValue < 5)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue));

            else if (randValue > 5 && randValue < 15)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue));

            hasDroppedXP = true;
        }
    }
}werewolf;

struct BAT :public ENEMY
{
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds())) {
            AnimationState = attacking;
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Bat;
        shape.setTexture(batTexture);
        shape.setScale(3, 3);
        health = 50;
        speed = 250;
        damage = 5;
        attackBox.setSize(Vector2f(70, 1));
        shape.setPosition(200, -200);
        shape.setOrigin(24.1, 17.5);
        AnimationState = walking;
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(60, 120));
        collider.setOrigin(collider.getLocalBounds().width / 2, collider.getLocalBounds().height / 2);
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        AttackDetection();
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 0;
            columnindex = columnindex % 3;
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 2;
            columnindex = columnindex % 10;
        }
        shape.setTextureRect(IntRect(48.2 * columnindex, 35 * rowindex, 48.2, 35));
        playertargeting();
        if (shape.getPosition().x > shanoa.sprite.getPosition().x) {
            shape.setScale(-3, 3);
        }
        else {
            shape.setScale(3, 3);
        }
        if (health <= 0) {
            isDead = true;
            health = 0;
            shape.setScale(0, 0);
        }
        if (isDead && hasDroppedXP == false)
        {
            Vector2f position = shape.getPosition();

            // Randomly decide: 5% chance for Red crystal
            int randValue = (rand() + 1) % 100;
            if (randValue < 5)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue));

            else if (randValue > 5 && randValue < 15)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue));

            hasDroppedXP = true;
        }
    }
}bat;

struct LeaderboardEntry {
    string name;
    float score; // Using float to match totalGameTime
};

struct sword {
    Sprite shape;
    RectangleShape collider;
    Vector2f velocity;
    float damage = 200;
    float speed;
    float deletiontimer = 0;

    void update()
    {
        shape.move(velocity * deltaTime);
        collider.setPosition(shape.getPosition());
        collider.setOrigin(shape.getOrigin());
        collider.setSize(Vector2f(32, 32));
        shape.setOrigin(16, 16);
        collider.setOrigin(15, 7.5);
        deletiontimer += deltaTime;

    }

};

struct MathEquation {
    Sprite sprite;
    Text userAnsText;
};

Gamestate gamestate = mainmenu;

Font defgamefont; // default game font

View view;

Text StartGameText, SettingsText, ExitText, LeaderboardText, CreditsText, volumeText, settingsmenuText;
Text DEV_T, TEAMNAME, NAMES, prof, teamname;
Text GameOverText, ScoreText, RestartText;
Text mathRevivalText;
Text nametext;
Text nameInputPromptText; // For the next screen
Text playerNameDisplayText; // For the next screen
Text GiveUpText; // *** Add text for the Give Up option ***

// *** Pause Menu Elements ***
Text PauseText;
Text ContinueText;
Text PauseReturnToMenuText;
RectangleShape ContinueButton;
RectangleShape PauseReturnToMenuButton;
FloatRect ContinueButtonBounds;
FloatRect PauseReturnToMenuButtonBounds;

string playerName = "";


Sprite MainMenuButtons, MainMenuBackground, Map, healthbar, creditsbutton, creditback, volume_up, volume_down, settingsBackground;

Vector2i mouseScreenpos;
Vector2f mouseWorldpos;

RectangleShape StartButton(Vector2f(490, 110)), SettingsButton(Vector2f(490, 110)), LeaderboardButton(Vector2f(490, 110)),
ExitButton(Vector2f(490, 110)), creditsButton(Vector2f(490, 110)), MathRevivlaButton(Vector2f(250, 50)), restartButton(Vector2f(250, 50));
RectangleShape GiveUpButton(Vector2f(250, 50)); // *** Add shape for the Give Up button *** 
RectangleShape equationAnsCellBox;
RectangleShape gameOverOverlay; // red color in gameover background
RectangleShape menuCursor;

FloatRect StartButtonBounds, SettingsButtonBounds, LeaderboardButtonBounds, ExitButtonBounds, creditsButtonBounds, volumeUpBounds,
volumeDownBounds;

FloatRect MathRevivlaButtonBounds, RestartButtonBounds, GiveUpButtonBounds; // *** Add bounds for Give Up button ***

float volumebarcontroller;

int selectedMenuButtonIndex = 0; // 0 for Start, 1 for Settings, 2 for Leaderboard, 3 for Exit
int selectedGameOverOptionIndex = 0; // 0 for Restart, 1 for Math Revival, 2 for Give Up
int randIndex;// equations elements random index

Listener GameVolume;
Sound MainMenuMusic, GameOverSound, GameloopMusic;
SoundBuffer MainMenuMusic_source, GameOverSound_source, GameloopMusic_source;

bool gameOverSoundPlayed = false;
VideoMode desktopMode = VideoMode::getDesktopMode();
RenderWindow window(desktopMode, "Vampire Survivors :The path to the legendary formula", sf::Style::Fullscreen);
//fullscreen fix
float beastspawntimer = 0;
float batspawntimer = 0;
float werewolfspawntimer = 0;
float zombiespawntimer = 0;
float spawnTime = 3.f;
float healthRatio;
float shootingtime = 0.f;
float shootingrate = 1;
float totalGameTime = 0.f;
float menuInputDelay = 0.f;
const float MENU_INPUT_COOLDOWN = 0.3f; // Time in seconds between allowed inputs
const float POST_TRANSITION_DELAY = 0.6f; // Define the delay duration in seconds
float postTransitionCooldown = 0.f;     // Make sure this is also declared (it is in your code around source 151)
float soundcontroller = 100;
RectangleShape volumebar[10];





vector<sword> swords;
vector<shared_ptr<ENEMY>> enemies;

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
MathEquation SuvivalEquation;
int EquationsAns[8] = { 3,4,4,2,7,4,1,6 };
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
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape && postTransitionCooldown <= 0)
            {
                if (gamestate == gameloop)
                {
                    gamestate = paused;
                    // Pause game loop music if it's playing
                    GameloopMusic.pause();
                    postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                }
                else if (gamestate == paused)
                {
                    gamestate = gameloop;
                    // Resume game loop music
                    GameloopMusic.play();
                    postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                    selectedMenuButtonIndex = 0; // Reset pause menu selection
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Tab)) // DEBUGGING BUTTON
            {
                window.close();
            }

            // --- Handle Text Entered event ---
            if (event.type == sf::Event::TextEntered)
            {
                if (gamestate == gameover && MathRevivalON) // Math Revival input (only when MathRevivalON is true)
                {
                    // ... existing Math Revival input logic using userInput ...
                    // (Handle Backspace/Enter for Math Revival within this block or outside with isKeyPressed)
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
                    // Enter key for Math Revival answer submission is handled here in your provided code
                    else if (event.text.unicode == 13)// iskeypressed::Enter
                    {
                        if (!userInput.empty())
                        {
                            if (stoi(userInput) == EquationsAns[randIndex]) // correct answer
                            {
                                gamestate = gameloop; // Go back to gameloop
                                postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                                GameOverSound.stop();
                                MathRevivalON = false;
                                userInput = ""; // Clear input
                                // *** Reset player state for revival ***
                                shanoa.health = shanoa.Maxhp / 2.f; // Restore half health (example)
                                shanoa.isDead = false; // Player is no longer dead
                                shanoa.hasRevived = true; // Mark that revival occurred
                                // You might need to adjust player position away from enemies
                            }
                            else
                            {
                                // Math Revival failed
                                gamestate = gameover; // Go to main menu (or gameover options again?)
                                postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                                MainMenuMusic.play();
                                MathRevivalON = false; // Turn off Math Revival puzzle
                                userInput = ""; // Clear input
                                // Game over screen is skipped, so score won't be saved from here.
                                // If you want to save score on Math Revival fail,
                                // transition back to gameover options instead of mainmenu.
                            }
                        }
                    }
                    SuvivalEquation.userAnsText.setString(userInput); // Update Math Revival text display
                }
                // *** Add Name Input handling ***
                else if (gamestate == nameinput) // Name input
                {
                    handleNameInput(event);
                }
            }


        } // End of while (window.pollEvent(event))


        // --- Handle Keyboard::isKeyPressed checks (for continuous actions like Backspace/Enter on name input) ---
        // These checks run every frame, outside the event polling loop.


        Update();
        Draw();
    }
    return 0;
}

// Function to handle name input
void handleNameInput(sf::Event& event)
{
    if (event.type == sf::Event::TextEntered)
    {
        // Allow printable ASCII characters (adjust range if needed)
        if (event.text.unicode >= 32 && event.text.unicode <= 126)
        {
            // Avoid Enter (13) and Backspace (8) as they are handled by KeyPressed
            if (event.text.unicode != 13 && event.text.unicode != 8)
            {
                const int MAX_NAME_LENGTH = 20; // Define a max length
                if (playerName.length() < MAX_NAME_LENGTH)
                {
                    playerName += static_cast<char>(event.text.unicode);
                }
            }
        }
        else if ((event.key.code == sf::Keyboard::Backspace || event.text.unicode == 8) && menuInputDelay >= MENU_INPUT_COOLDOWN / 3)
        {
            if (!playerName.empty())
            {
                playerName.pop_back();
                menuInputDelay = 0.f; // Reset menu delay
            }
        }
        else if (Keyboard::isKeyPressed(Keyboard::Enter) && menuInputDelay >= MENU_INPUT_COOLDOWN)
        {
            if (!playerName.empty()) // Only save if name is not empty
            {
                // *** Calculate Final Score ***
                float finalScore = 0.f;
                if (shanoa.hasRevived) {
                    // Died after reviving
                    finalScore = (shanoa.timeAtFirstDeath * 2.0f) + ((totalGameTime - shanoa.timeAtFirstDeath) * 1.0f);
                }
                else {
                    // Died without reviving
                    finalScore = totalGameTime * 2.0f;
                }

                // *** Create and add new leaderboard entry (using vector) ***
                leaderboardEntriesMap.insert({ -finalScore, playerName });

                const int MAX_LEADERBOARD_SIZE = 10;
                if (leaderboardEntriesMap.size() > MAX_LEADERBOARD_SIZE)
                {
                    // The map is sorted by key (-score), so the elements *after* the first 10
                    // are the ones with lower scores that should be removed.
                    auto it = leaderboardEntriesMap.begin();
                    std::advance(it, MAX_LEADERBOARD_SIZE); // Move iterator to the 11th element
                    leaderboardEntriesMap.erase(it, leaderboardEntriesMap.end()); // Erase from 11th to the end
                }

                // *** Save the updated leaderboard to file ***
                SaveLeaderboard();
            }

            // *** Transition to leaderboard state ***
            gamestate = leaderboard; // Transition to the leaderboard display screen
            menuInputDelay = 0.f; // Reset menu delay
            playerName = ""; // Clear name input field

            // Decide which music to play when entering leaderboard state
            // MainMenuMusic.play();
        }
    }
    // Update the displayed name text object string
    playerNameDisplayText.setString(playerName);
}

void shooting()
{
    shootingtime += deltaTime;

    if (shootingtime >= shootingrate)
    {
        shootingtime = 0;
        float erasuretimer = 0;
        sword newSword;
        newSword.speed = 5000 * deltaTime;
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

void SaveLeaderboard()
{
    std::ofstream outputFile("leaderboard.txt");

    if (outputFile.is_open())
    {
        // Iterate through the map. It's already sorted by key (-score).
        for (const auto& pair : leaderboardEntriesMap) // pair.first is -score, pair.second is name
        {
            outputFile << pair.second << std::endl; // Write name
            outputFile << std::fixed << std::setprecision(2) << -pair.first << std::endl; // Write positive score (-key)
        }
        outputFile.close();
        // std::cout << "Leaderboard saved successfully." << std::endl;
    }
    else
    {
        std::cerr << "Error: Unable to open leaderboard.txt for saving!" << std::endl;
    }
}

void LoadLeaderboard()
{
    leaderboardEntriesMap.clear(); // Clear any existing entries
    std::ifstream inputFile("leaderboard.txt");

    if (inputFile.is_open())
    {
        string name;
        string scoreStr;

        // Read pairs of lines (name and score)
        while (std::getline(inputFile, name) && std::getline(inputFile, scoreStr))
        {
            try
            {
                float score = stof(scoreStr); // Convert score string to float
                // Insert into multimap: key is negative score, value is name
                leaderboardEntriesMap.insert({ -score, name });
            }
            catch (const invalid_argument& ia)
            {
                cerr << "Invalid score format in leaderboard file: " << ia.what() << std::endl;
            }
            catch (const std::out_of_range& oor)
            {
                cerr << "Score value out of range in leaderboard file: " << oor.what() << std::endl;
            }
        }
        inputFile.close();

        // Trim to top 10 after loading, in case the file has too many entries
        const int MAX_LEADERBOARD_SIZE = 10;
        // multimap is already sorted by key (negative score), so the first 10 are the top 10
        // If there are more than 10, erase the elements beyond the 10th
        if (leaderboardEntriesMap.size() > MAX_LEADERBOARD_SIZE)
        {
            auto it = leaderboardEntriesMap.begin();
            std::advance(it, MAX_LEADERBOARD_SIZE); // Move iterator to the 11th element
            leaderboardEntriesMap.erase(it, leaderboardEntriesMap.end()); // Erase from 11th to the end
        }

        // std::cout << "Leaderboard loaded successfully. Entries: " << leaderboardEntriesMap.size() << std::endl; // Optional: for debugging
    }
    // If the file doesn't exist, the map remains empty, which is correct.
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

void PauseMenuInit()
{
    PauseText.setFont(defgamefont);
    PauseText.setString("Paused");
    PauseText.setCharacterSize(80);
    PauseText.setFillColor(Color::White);

    ContinueText.setFont(defgamefont);
    ContinueText.setString("Continue");
    ContinueText.setCharacterSize(20);
    ContinueText.setFillColor(Color::White);

    PauseReturnToMenuText.setFont(defgamefont);
    PauseReturnToMenuText.setString("Main Menu");
    PauseReturnToMenuText.setCharacterSize(20);
    PauseReturnToMenuText.setFillColor(Color::White);

    // Configure button shapes (reuse similar look from game over)
    ContinueButton.setFillColor(Color(100, 0, 0));
    ContinueButton.setOutlineColor(Color::Yellow);
    ContinueButton.setSize(Vector2f(250, 50)); // Match other button sizes

    PauseReturnToMenuButton.setFillColor(Color(100, 0, 0));
    PauseReturnToMenuButton.setOutlineColor(Color::Yellow);
    PauseReturnToMenuButton.setSize(Vector2f(250, 50)); // Match other button sizes
}

void CharacterInit() {
    shanoa.texture.loadFromFile("Assets\\shanoa.png");
    shanoa.sprite.setTexture(shanoa.texture);
    shanoa.speed = 200;
    shanoa.sprite.setPosition(0, 0);
    shanoa.collider.setSize(Vector2f(60, 125));
    shanoa.collider.setOrigin(shanoa.collider.getLocalBounds().width / 2, shanoa.collider.getLocalBounds().height / 2);
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
    Map.setTextureRect(IntRect(0, 0, 20000, 20000));
    Map.setTexture(Map_Texture);
    Map.setPosition(-10000, -10000);
}

void EnemySpawn() {
    Vector2f randomspawnpoint = Vector2f(shanoa.sprite.getPosition().x + (rand() % 2001 - 1000), shanoa.sprite.getPosition().y + (rand() % 2001 - 1000));
    if (enemies.size() <= 25) {
        beastspawntimer += deltaTime;
        batspawntimer += deltaTime;
        werewolfspawntimer += deltaTime;
        zombiespawntimer += deltaTime;
        if (beastspawntimer >= 4) {

            auto newBeast = make_unique<BEAST>();

            newBeast->start();

            newBeast->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newBeast));

            beastspawntimer = 0;
        }
        if (zombiespawntimer >= 2) {

            auto newZombie = make_unique<ZOMBIE>();

            newZombie->start();

            newZombie->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newZombie));

            zombiespawntimer = 0;
        }
        if (werewolfspawntimer >= 3) {

            auto newWerewolf = make_unique<WEREWOLF>();

            newWerewolf->start();

            newWerewolf->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newWerewolf));

            werewolfspawntimer = 0;
        }
        if (batspawntimer >= 1) {

            auto newBat = make_unique<BAT>();

            newBat->start();

            newBat->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newBat));

            batspawntimer = 0;
        }
    }
}

void EnemyHandler() {
    for (int i = 0;i < enemies.size();i++) {
        if (enemies[i]->isDead) {
            enemies.erase(enemies.begin() + i);
            continue;
        }
        else {
            enemies[i]->update();
        }
    }
}

void healthbarhandling() {
    healthRatio = shanoa.health / shanoa.Maxhp;
    if (healthRatio > 0.9) {
        healthbar.setTextureRect(IntRect(0, 0, 445, 177));
    }
    else if (healthRatio > 0.8) {
        healthbar.setTextureRect(IntRect(444.4 + 5, 0, 445, 177));
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
    else if (shanoa.health <= 0) {
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
    RestartText.setString("Restart");
    RestartText.setCharacterSize(20);
    RestartText.setFillColor(Color::White);

    mathRevivalText.setFont(defgamefont);
    mathRevivalText.setString("Math Revival?");
    mathRevivalText.setCharacterSize(25);
    mathRevivalText.setFillColor(Color::White);

    // *** Initialize Give Up Text and Button ***
    GiveUpText.setFont(defgamefont);
    GiveUpText.setString("Give Up"); // Or "Submit Score"
    GiveUpText.setCharacterSize(20); // Match other option text size
    GiveUpText.setFillColor(Color::White);
    // Position will be set in Draw()

    GiveUpButton.setFillColor(Color(100, 0, 0)); // Match other button colors
    GiveUpButton.setOutlineColor(Color::Yellow);
    GiveUpButton.setSize(Vector2f(250, 50)); // Match other button sizes
    // Position and bounds will be set in Draw() and Update()

    equationSpriteSheet.loadFromFile("Assets\\equationSpriteSheet.png");
    SuvivalEquation.sprite.setTexture(equationSpriteSheet);
    SuvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));
    SuvivalEquation.sprite.setScale(0.3 / 1.5, 0.45 / 1.5);

    equationAnsCellBox.setFillColor(Color::Black);
    equationAnsCellBox.setSize(Vector2f(200, 50));
    equationAnsCellBox.setOutlineColor(Color::Red);
    equationAnsCellBox.setOutlineThickness(5);

    restartButton.setFillColor(Color(100, 0, 0));
    restartButton.setOutlineColor(Color::Yellow);

    MathRevivlaButton.setFillColor(Color(100, 0, 0));
    MathRevivlaButton.setOutlineColor(Color::Yellow);

    restartButton.setSize(Vector2f(250, 50)); // Example size
    MathRevivlaButton.setSize(Vector2f(250, 50)); // Example size


    gameOverOverlay.setSize(view.getSize()); // Set size based on the view's size
    // Set its origin to the center, so position(view.getCenter()) works correctly
    gameOverOverlay.setOrigin(view.getSize().x / 2.f, view.getSize().y / 2.f);

    // The fourth parameter (100) is the alpha channel (transparency), from 0 (fully transparent) to 255 (fully opaque)
    gameOverOverlay.setFillColor(Color(100, 0, 0, 120));


    // REMEMBER TO REPLACE GAMEOVER SOUND WITH NEW ONE
    GameOverSound_source.loadFromFile("Assets\\MainMenuMusic.ogg");
    GameOverSound.setBuffer(GameOverSound_source);

    gameOverSoundPlayed = false;

    // Ensure MathRevivalON is false initially for the gameover screen
    MathRevivalON = false;

}

void MainMenuButtonCheck()
{
    if (StartButtonBounds.contains(mouseWorldpos))
    {
        selectedMenuButtonIndex = 0;
        if (Mouse::isButtonPressed(Mouse::Left))
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

void NameInputInit()
{
    // ... Initialize nameInputPromptText and playerNameDisplayText ...

    // Initialize the player name string
    playerName = "";
    // Initialize the prompt text
    nameInputPromptText.setFont(defgamefont);
    nameInputPromptText.setCharacterSize(40); // Choose a suitable size
    nameInputPromptText.setFillColor(sf::Color::White);
    nameInputPromptText.setString("Enter Your Name:"); // Set the prompt message

    // Initialize the player name display text
    playerNameDisplayText.setFont(defgamefont);
    playerNameDisplayText.setCharacterSize(40); // Match prompt size or adjust
    playerNameDisplayText.setFillColor(sf::Color::Yellow); // Different color for input?
    playerNameDisplayText.setString(playerName); // Start with an empty string

    // You might also want a shape for the input box
    equationAnsCellBox.setFillColor(sf::Color::Black);
    equationAnsCellBox.setSize(sf::Vector2f(400, 60)); // Adjust size as needed
    equationAnsCellBox.setOutlineColor(sf::Color::Blue); // Choose a color
    equationAnsCellBox.setOutlineThickness(3); // Choose a thickness
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
                    //cout << "l";
                    Object.move(intersection.width, 0);
                }
                else { // right
                    Object.move(-intersection.width, 0);
                    //cout << "r";
                }
            }
            else if (intersection.height < intersection.width) {

                if (objectBound.top > wall.top) {
                    Object.move(0, intersection.height);
                    //cout << "Lower";
                }
                else {
                    Object.move(0, -intersection.height);
                    //cout << "upper";

                }

            }
        } // upper/lower 
    }
}

void AttackDetection(RectangleShape& playerCollider, RectangleShape& enemyCollider, ENEMY& theEnemy) {
    if (playerCollider.getGlobalBounds().intersects(enemyCollider.getGlobalBounds())) {
        theEnemy.AnimationState = attacking;
    }
    else
        theEnemy.AnimationState = walking;
}

void GetRandIndex(int& randomIndex)
{
    randomIndex = rand() % 6;
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
    generalCollision(bat.collider, shanoa.collider, bat.shape);
    swordFullCollision();
}

void Start()
{
    // code here is only executed at the start of the program
    // initializations of everything

    window.setFramerateLimit(30);


    //Game font initialization
    defgamefont.loadFromFile("VampireZone.ttf");

    BlueXP.loadFromFile("Assets\\XPc.png");
    GreenXP.loadFromFile("Assets\\GreenXPc.png");
    RedXP.loadFromFile("Assets\\RedXPc.png");
    beasttexture.loadFromFile("Assets\\beastTexture.png");
    zombieTexture.loadFromFile("Assets\\Zombie.png");
    batTexture.loadFromFile("Assets\\Bat.png");
    werewolfTexture.loadFromFile("Assets\\werewolfwhite.png");
    swordspritesheet.loadFromFile("Assets\\SWORDS.png");


    MapInit();
    PauseMenuInit();
    MainmenuInit();
    GameOverInit();
    CharacterInit();
    MapInit();
    creditsInit();
    SettingsMenuInit();
    NameInputInit(); // Make sure name input init is called
    LoadLeaderboard(); // <-- Add this line

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
        menuCursor.setSize(Vector2f(selectedButtonSize.x - 200, selectedButtonSize.y - 45));

        view.setCenter(10000, 9800);

    }

    else if (gamestate == gameloop)
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
            if (swords[i].deletiontimer > 10) {
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
        if (Keyboard::isKeyPressed(Keyboard::Q) || shanoa.health <= 0)
        {
            // Only record timeAtFirstDeath the very first time the player dies
            if (!shanoa.isDead && !shanoa.hasRevived)
            {
                shanoa.timeAtFirstDeath = totalGameTime;
            }
            shanoa.isDead = true; // Set isDead regardless of revival state

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
            postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
            selectedGameOverOptionIndex = 0; // Reset game over menu selection
            enemies.clear();
            swords.clear();
            selectedMenuButtonIndex = 0;
        }
        EnemySpawn();
        EnemyHandler();
        shanoa.update();
        globalCollsion();
        healthbarhandling();
        for (int i = 0;i < swords.size();i++) {
            bool test = false;
            for (int j = 0;j < enemies.size();j++) {
                if (swords[i].collider.getGlobalBounds().intersects(enemies[i]->collider.getGlobalBounds())) {
                    //cout << " bb ";
                    enemies[i]->health -= swords[i].damage;
                    cout << swords[i].damage << endl;
                    swords.erase(swords.begin() + i);
                    //cout << enemies[i] -> health << ' ';
                    test = true;
                    break;
                }
            }

        }
        for (int i = 0; i < Crystals.size(); i++) {
            // Pass player position to update function
            Crystals[i].update(deltaTime, shanoa.sprite.getPosition());

            if (Crystals[i].isCollected && !Crystals[i].CrystalsRemove) {
                shanoa.xp += Crystals[i].xpValue;
                cout << "Collected XP: " << Crystals[i].xpValue << " (Total: " << shanoa.xp << ")" << endl;
                Crystals[i].CrystalsRemove = true;
            }
        }
        for (int i = Crystals.size() - 1; i >= 0; i--) {
            if (Crystals[i].CrystalsRemove) {
                Crystals.erase(Crystals.begin() + i);
            }
        }
        view.setCenter(shanoa.sprite.getPosition());
    }

    else if (gamestate == paused) // <-- New Paused State Update
    {
        window.setMouseCursorVisible(true);
        // Game logic is paused, only handle menu input

        menuInputDelay += deltaTime;

        // Position buttons relative to the view center
        Vector2f viewCenter = view.getCenter();
        ContinueButton.setPosition(viewCenter.x - ContinueButton.getLocalBounds().width / 2.f, viewCenter.y - 50.f);
        PauseReturnToMenuButton.setPosition(viewCenter.x - PauseReturnToMenuButton.getLocalBounds().width / 2.f, viewCenter.y + 50.f);

        // Update bounds after setting position
        ContinueButtonBounds = ContinueButton.getGlobalBounds();
        PauseReturnToMenuButtonBounds = PauseReturnToMenuButton.getGlobalBounds();

        // --- Input Handling (Keyboard and Mouse) ---
        // Use menuInputDelay for navigation
        if (menuInputDelay >= MENU_INPUT_COOLDOWN)
        {
            bool moved = false;
            // Keyboard Navigation
            if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down))
            {
                selectedMenuButtonIndex++; // Use selectedMenuButtonIndex for pause menu (0: Continue, 1: Main Menu)
                moved = true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up))
            {
                selectedMenuButtonIndex--;
                moved = true;
            }

            if (moved)
            {
                // Wrap around options (0, 1)
                if (selectedMenuButtonIndex < 0) {
                    selectedMenuButtonIndex = 1;
                }
                else if (selectedMenuButtonIndex > 1) {
                    selectedMenuButtonIndex = 0;
                }
                menuInputDelay = 0; // Reset delay
            }

            // Mouse Hover Highlighting (updates selectedMenuButtonIndex)
            if (ContinueButtonBounds.contains(mouseWorldpos)) {
                selectedMenuButtonIndex = 0;
            }
            else if (PauseReturnToMenuButtonBounds.contains(mouseWorldpos)) {
                selectedMenuButtonIndex = 1;
            }

            // Selection (Enter Key or Mouse Click)
            bool activateSelection = false;
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                activateSelection = true;
            }
            // Check for Left mouse button press over the *currently selected* button
            if (Mouse::isButtonPressed(Mouse::Left)) {
                if (selectedMenuButtonIndex == 0 && ContinueButtonBounds.contains(mouseWorldpos)) activateSelection = true;
                if (selectedMenuButtonIndex == 1 && PauseReturnToMenuButtonBounds.contains(mouseWorldpos)) activateSelection = true;
            }

            if (activateSelection && postTransitionCooldown <= 0) // Also check postTransitionCooldown
            {
                if (selectedMenuButtonIndex == 0) // Continue selected
                {
                    gamestate = gameloop; // Resume game
                    GameloopMusic.play(); // Resume music
                    postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                }
                else if (selectedMenuButtonIndex == 1) // Return to Main Menu selected
                {
                    gamestate = mainmenu; // Go to main menu
                    MainMenuMusic.play(); // Play main menu music
                    GameloopMusic.stop(); // Stop game loop music if it was paused
                    postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown

                    // Reset game state elements for a new game
                    enemies.clear();
                    swords.clear();
                    Crystals.clear();
                    shanoa.health = shanoa.Maxhp; // Reset player health
                    shanoa.isDead = false;
                    totalGameTime = 0.f;
                    gameOverSoundPlayed = false;
                    MathRevivalON = false;
                    userInput = "";
                    selectedMenuButtonIndex = 0; // Reset main menu selection
                }
                menuInputDelay = 0; // Reset delay after selection
            }
        }


        // Update menu cursor position based on selected pause menu option
        Vector2f selectedOptionPosition;
        Vector2f selectedOptionSize;
        float cursorAdjust = 3.f; // Adjust this value for cursor offset

        if (selectedMenuButtonIndex == 0) {
            selectedOptionPosition = ContinueButton.getPosition();
            selectedOptionSize = ContinueButton.getSize();
        }
        else if (selectedMenuButtonIndex == 1) {
            selectedOptionPosition = PauseReturnToMenuButton.getPosition();
            selectedOptionSize = PauseReturnToMenuButton.getSize();
        }
        menuCursor.setPosition(selectedOptionPosition.x - cursorAdjust, selectedOptionPosition.y - cursorAdjust);
        menuCursor.setSize(Vector2f(selectedOptionSize.x + cursorAdjust * 2.f, selectedOptionSize.y + cursorAdjust * 2.f));

        view.setCenter(viewCenter); // Keep view centered on pause screen
    }

    else if (gamestate == settings)
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

    else if (gamestate == leaderboard)
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

    else if (gamestate == gameover)
    {
        // gameover screen update
        window.setMouseCursorVisible(true);

        // Position buttons and get their bounds in Update() before handling input
        // This is necessary because view.getCenter() is dynamic.
        // (These positions are also calculated in Draw, but recalculating them here
        // ensures mouse collision detection is accurate based on current frame's positions)
        Vector2f viewCenter = view.getCenter();
        restartButton.setPosition(viewCenter.x - restartButton.getLocalBounds().width / 2.f, viewCenter.y + 90.f);
        MathRevivlaButton.setPosition(viewCenter.x - MathRevivlaButton.getLocalBounds().width / 2.f, viewCenter.y + 175.f);
        GiveUpButton.setPosition(viewCenter.x - GiveUpButton.getLocalBounds().width / 2.f, viewCenter.y + 260.f);

        // Update bounds after setting position
        RestartButtonBounds = restartButton.getGlobalBounds();
        MathRevivlaButtonBounds = MathRevivlaButton.getGlobalBounds();
        GiveUpButtonBounds = GiveUpButton.getGlobalBounds();

        // --- Input Handling ---

        // Only process menu input if Math Revival is NOT active
        if (!MathRevivalON)
        {
            // *** Mouse Hover Detection for Highlighting ***
            // This should come before keyboard input so keyboard can override mouse
            if (RestartButtonBounds.contains(mouseWorldpos)) {
                selectedGameOverOptionIndex = 0;
            }
            else if (MathRevivlaButtonBounds.contains(mouseWorldpos)) {
                selectedGameOverOptionIndex = 1;
            }
            else if (GiveUpButtonBounds.contains(mouseWorldpos)) {
                selectedGameOverOptionIndex = 2;
            }

            // Handle Keyboard Navigation (Up/Down or W/S) - Keep this logic
            if (menuInputDelay >= MENU_INPUT_COOLDOWN)
            {
                bool moved = false;
                if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down))
                {
                    selectedGameOverOptionIndex++;
                    moved = true;
                }
                else if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up))
                {
                    selectedGameOverOptionIndex--;
                    moved = true;
                }

                if (moved)
                {
                    // Wrap around options (0, 1, 2)
                    if (selectedGameOverOptionIndex < 0) {
                        selectedGameOverOptionIndex = 2; // Wrap from top to bottom
                    }
                    else if (selectedGameOverOptionIndex > 2) {
                        selectedGameOverOptionIndex = 0; // Wrap from bottom to top
                    }
                    menuInputDelay = 0.f; // Reset delay after movement
                }

                // Handle Selection (Enter Key or Mouse Click)
                bool activateSelection = false;

                // Check for Enter key press
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    activateSelection = true;
                }
                // Check for Left mouse button press over the *currently selected* button
                // This makes click selection precise.
                if (Mouse::isButtonPressed(Mouse::Left))
                {
                    if (selectedGameOverOptionIndex == 0 && RestartButtonBounds.contains(mouseWorldpos)) activateSelection = true;
                    if (selectedGameOverOptionIndex == 1 && MathRevivlaButtonBounds.contains(mouseWorldpos)) activateSelection = true;
                    if (selectedGameOverOptionIndex == 2 && GiveUpButtonBounds.contains(mouseWorldpos)) activateSelection = true;
                }


                if (activateSelection && menuInputDelay >= MENU_INPUT_COOLDOWN)
                {
                    if (selectedGameOverOptionIndex == 0) // Restart selected
                    {
                        GameOverSound.stop();
                        menuInputDelay = 0.f; // Reset delay BEFORE state change
                        gamestate = mainmenu; // Transition to main menu
                        view.setCenter(10000, 9800); // Center view back on main menu
                        MainMenuMusic.play();
                        gameOverSoundPlayed = false;
                        selectedMenuButtonIndex = 0; // Reset main menu selection
                        // Reset game state elements (enemies, crystals, etc.) when going to main menu for a new game
                        enemies.clear();
                        Crystals.clear();
                        shanoa.health = 120; // Or your starting health
                        shanoa.isDead = false;
                        totalGameTime = 0.f;
                        swords.clear();
                    }
                    else if (selectedGameOverOptionIndex == 1) // Math Revival selected
                    {
                        MathRevivalON = true; // Activate Math Revival
                        menuInputDelay = 0.f; // Reset delay
                    }
                    else if (selectedGameOverOptionIndex == 2) // Give Up selected
                    {
                        GameOverSound.stop();
                        menuInputDelay = 0.f; // Reset delay BEFORE state change
                        gamestate = nameinput; // Transition to name input state
                        playerName = ""; // Clear name for new input
                        // totalGameTime (score) is already stored globally
                    }
                    // menuInputDelay is reset above for each case
                }
            }
            menuInputDelay += deltaTime; // Increment delay for gameover screen

            // *** Update menuCursor position based on selectedGameOverOptionIndex ***
            Vector2f selectedOptionPosition;
            Vector2f selectedOptionSize;
            float cursorAdjust = 3.f; // Adjust this value for cursor offset

            if (selectedGameOverOptionIndex == 0) {
                selectedOptionPosition = restartButton.getPosition();
                selectedOptionSize = restartButton.getSize();
            }
            else if (selectedGameOverOptionIndex == 1) {
                selectedOptionPosition = MathRevivlaButton.getPosition();
                selectedOptionSize = MathRevivlaButton.getSize();
            }
            else if (selectedGameOverOptionIndex == 2) { // Position for Give Up option
                selectedOptionPosition = GiveUpButton.getPosition();
                selectedOptionSize = GiveUpButton.getSize();
            }

            // Position and size the cursor rectangle
            menuCursor.setPosition(selectedOptionPosition.x - cursorAdjust, selectedOptionPosition.y - cursorAdjust);
            menuCursor.setSize(Vector2f(selectedOptionSize.x + cursorAdjust * 2.f, selectedOptionSize.y + cursorAdjust * 2.f));

        }
        // Handle Math Revival Input (if MathRevivalON is true)
        else
        {
            // This part remains as you had it, likely handling input via event loop
            // and updating MathRevivalON or gamestate based on the answer.
            // Ensure menuInputDelay is also managed here if needed for Math Revival input timing.
            // Math Revival input logic is currently in main's event loop.
            // If you want to use menuInputDelay for that, you'd need to manage it here too.
        }


    }

    else if (gamestate == credits) {
        creditback.setColor(Color(70, 70, 70));
        view.setCenter(10000, 9800);
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
        }
    }

    else if (gamestate == nameinput)
    {
        menuInputDelay += deltaTime; // Increment delay for nameinput state

        // *** Ensure background color is reset ***
        if (creditback.getTexture() != nullptr) // Check if texture is loaded
        {
            creditback.setColor(Color::White); // Reset to default color (assuming white texture)
        }

        window.setMouseCursorVisible(true); // Make cursor visible
        view.setCenter(10000, 9800); // Center view on static UI elements
    }


    // Decrement post-transition cooldown
    if (postTransitionCooldown > 0) {
        postTransitionCooldown -= deltaTime;
        if (postTransitionCooldown < 0) {
            postTransitionCooldown = 0;
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

    // Draw elements that are visible in multiple states (like the game world)
    // Draw the game scene if not in main menu, settings, credits, name input, or leaderboard
    if (gamestate == paused || gamestate == gameover)
    {
        window.draw(Map);
        for (size_t i = 0; i < swords.size(); i++)
        {
            window.draw(swords[i].shape);
        }
        window.draw(shanoa.sprite);
        // Draw enemies
        for (size_t i = 0; i < enemies.size(); ++i) {
            //window.draw(enemies[i]->collider); // Usually don't draw colliders in final game
            window.draw(enemies[i]->shape);
        }
        // Draw crystals
        for (size_t i = 0; i < Crystals.size(); i++)
        {
            if (Crystals[i].isCollected == false) {
                window.draw(Crystals[i].sprite);
            }
        }
        if (gamestate == gameloop || gamestate == paused) // Draw healthbar in gameloop and paused
        {
            window.draw(healthbar);
        }
    }

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

    else if (gamestate == paused) // <-- New Paused State Draw
    {
        // Game scene is already drawn above
        gameOverOverlay.setPosition(view.getCenter()); // Center the overlay on the view
        window.draw(gameOverOverlay); // Draw the dimming overlay

        Vector2f viewCenter = view.getCenter();
        // Position static text elements relative to view center
        PauseText.setPosition(viewCenter.x - PauseText.getGlobalBounds().width / 2.f, viewCenter.y - 150.f);
        window.draw(PauseText);

        // Position the buttons relative to the view center
        ContinueButton.setPosition(viewCenter.x - ContinueButton.getLocalBounds().width / 2.f, viewCenter.y - 50.f);
        PauseReturnToMenuButton.setPosition(viewCenter.x - PauseReturnToMenuButton.getLocalBounds().width / 2.f, viewCenter.y + 50.f);

        // Position the text on top of their corresponding buttons
        ContinueText.setPosition(ContinueButton.getPosition().x + (ContinueButton.getSize().x - ContinueText.getLocalBounds().width) / 2.f,
            ContinueButton.getPosition().y + (ContinueButton.getSize().y - ContinueText.getLocalBounds().height) / 2.f - 5.f); // Center text on button, adjust -5.f as needed
        PauseReturnToMenuText.setPosition(PauseReturnToMenuButton.getPosition().x + (PauseReturnToMenuButton.getSize().x - PauseReturnToMenuText.getLocalBounds().width) / 2.f,
            PauseReturnToMenuButton.getPosition().y + (PauseReturnToMenuButton.getSize().y - PauseReturnToMenuText.getLocalBounds().height) / 2.f - 5.f); // Center text on button

        window.draw(ContinueButton);
        window.draw(ContinueText);
        window.draw(PauseReturnToMenuButton);
        window.draw(PauseReturnToMenuText);
        window.draw(menuCursor); // Draw menu cursor over selected button
    }

    else if (gamestate == gameloop)
    {
        // gameloop draw
        window.draw(Map);
        //window.draw(shanoa.collider);
        for (int i = 0; i < swords.size(); i++)
        {
            /*  window.draw(swords[i].collider);*/
            window.draw(swords[i].shape);
        }
        window.draw(healthbar);
        window.draw(shanoa.sprite);
        for (int i = 0;i < enemies.size();i++) {
            window.draw(enemies[i]->collider);
            window.draw(enemies[i]->shape);
        }
        for (int i = 0; i < Crystals.size(); i++)
        {
            if (Crystals[i].isCollected == false) {
                window.draw(Crystals[i].sprite);
            }
        }
    }

    else if (gamestate == settings)
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

    else  if (gamestate == gameover)
    {
        // gameover screen draw (game scene already drawn above)
        gameOverOverlay.setPosition(view.getCenter()); // Center the overlay on the view
        window.draw(gameOverOverlay);

        Vector2f viewCenter = view.getCenter();
        // Position static text elements relative to view center
        GameOverText.setPosition(viewCenter.x - GameOverText.getGlobalBounds().width / 2.f, viewCenter.y - 100.f);
        ScoreText.setPosition(viewCenter.x - ScoreText.getGlobalBounds().width / 2.f, viewCenter.y + 10.f);
        window.draw(GameOverText);
        window.draw(ScoreText);

        if (MathRevivalON)
        {
            // Position and draw Math Revival elements when active
            SuvivalEquation.sprite.setPosition(viewCenter.x - SuvivalEquation.sprite.getGlobalBounds().width / 2.f, viewCenter.y + 100.f);
            equationAnsCellBox.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f, viewCenter.y + 160.f);
            SuvivalEquation.userAnsText.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f + 5, viewCenter.y + 163.f); // Adjust +5 for padding

            window.draw(SuvivalEquation.sprite);
            window.draw(equationAnsCellBox);
            window.draw(SuvivalEquation.userAnsText);
        }
        else // Draw the buttons/options when Math Revival is NOT active
        {
            // Position the buttons relative to the view center
            // Use getLocalBounds() for the button size itself when centering
            restartButton.setPosition(viewCenter.x - restartButton.getLocalBounds().width / 2.f, viewCenter.y + 90.f);
            MathRevivlaButton.setPosition(viewCenter.x - MathRevivlaButton.getLocalBounds().width / 2.f, viewCenter.y + 175.f);
            GiveUpButton.setPosition(viewCenter.x - GiveUpButton.getLocalBounds().width / 2.f, viewCenter.y + 260.f); // Position below Math Revival button

            // Position the text on top of their corresponding buttons
            // Use getLocalBounds() for text size when centering on the button
            RestartText.setPosition(restartButton.getPosition().x + (restartButton.getSize().x - RestartText.getLocalBounds().width) / 2.f, restartButton.getPosition().y + (restartButton.getSize().y - RestartText.getLocalBounds().height) / 2.f - 5.f); // Center text on button, adjust -5.f as needed
            mathRevivalText.setPosition(MathRevivlaButton.getPosition().x + (MathRevivlaButton.getSize().x - mathRevivalText.getLocalBounds().width) / 2.f, MathRevivlaButton.getPosition().y + (MathRevivlaButton.getSize().y - mathRevivalText.getLocalBounds().height) / 2.f - 5.f); // Center text on button
            // Position Give Up Text on its button
            GiveUpText.setPosition(GiveUpButton.getPosition().x + (GiveUpButton.getSize().x - GiveUpText.getLocalBounds().width) / 2.f, GiveUpButton.getPosition().y + (GiveUpButton.getSize().y - GiveUpText.getLocalBounds().height) / 2.f - 5.f); // Center text on button


            window.draw(restartButton);
            window.draw(RestartText);
            window.draw(MathRevivlaButton);
            window.draw(mathRevivalText);
            window.draw(GiveUpButton); // Draw Give Up button
            window.draw(GiveUpText); // Draw Give Up text

            // Draw the cursor when Math Revival is NOT active
            // Cursor position is updated in the Update function
            window.draw(menuCursor);
        }
    }

    else  if (gamestate == credits)
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

    else if (gamestate == nameinput) // <-- Add this block
    {
        // Center the view on the UI elements for static states like this
        sf::Vector2f viewCenter = view.getCenter();

        // Draw background (using creditback or a dedicated name input background)
        window.draw(creditback); // Or window.draw(nameInputBackgroundSprite);

        // Position and draw the prompt text
        nameInputPromptText.setPosition(viewCenter.x - nameInputPromptText.getGlobalBounds().width / 2.f, viewCenter.y - 100.f);
        window.draw(nameInputPromptText);

        // Position and draw the input box
        equationAnsCellBox.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f, viewCenter.y - 20.f);
        window.draw(equationAnsCellBox);

        // Position and draw the player name text (inside the box)
        // Adjust +5.f padding as needed
        playerNameDisplayText.setPosition(equationAnsCellBox.getPosition().x + 5.f, equationAnsCellBox.getPosition().y + (equationAnsCellBox.getSize().y - playerNameDisplayText.getLocalBounds().height) / 2.f - 5.f);
        window.draw(playerNameDisplayText);

        // You might also want to draw a cursor hint or instructions like "Press Enter to confirm"
    }

    else if (gamestate == leaderboard)
    {
        // Draw background (using creditback)
        window.draw(creditback);
        Vector2f viewCenter = view.getCenter();
        float startY = viewCenter.y - 180.f; // Starting Y position for the first entry
        float lineHeight = 50.f; // Vertical space between entries
        int rank = 1; // Start rank counter

        // Add Leaderboard Title
        Text leaderboardTitleText;
        leaderboardTitleText.setFont(defgamefont);
        leaderboardTitleText.setCharacterSize(50);
        leaderboardTitleText.setFillColor(Color::Yellow);
        leaderboardTitleText.setString("Leaderboard");
        leaderboardTitleText.setPosition(viewCenter.x - leaderboardTitleText.getGlobalBounds().width / 2.f, viewCenter.y - 250.f);
        window.draw(leaderboardTitleText);


        // Multimap: Iterate through the map
        for (const auto& pair : leaderboardEntriesMap)
        {
            // Stop after drawing the top 10
            if (rank > 10) {
                break;
            }
            sf::Text entryText;
            entryText.setFont(defgamefont);
            entryText.setCharacterSize(30);
            entryText.setFillColor(sf::Color::White);
            // Format the entry string (Rank. Name - Score)
            std::stringstream ss;
            ss << rank << ". " // Current rank
                << pair.second // Player name (the value)
                << " - "
                << std::fixed << std::setprecision(2) << -pair.first; // Positive score (-key)
            entryText.setString(ss.str());

            // Position the entry text
            entryText.setPosition(viewCenter.x - entryText.getGlobalBounds().width / 2.f, startY + (rank - 1) * lineHeight);
            window.draw(entryText);
            rank++; // Increment rank
        }
        // Add instruction to return
        Text returnText;
        returnText.setFont(defgamefont);
        returnText.setCharacterSize(24);
        returnText.setFillColor(Color::White);
        returnText.setString("Press R to return to Main Menu");
        returnText.setPosition(viewCenter.x - returnText.getGlobalBounds().width / 2.f, viewCenter.y + 300.f);
        window.draw(returnText);
    }

    window.display(); // Display sprites on screen
}
