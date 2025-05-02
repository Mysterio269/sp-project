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
    toright,toleft
};

enum monstertype
{
    Beast,
    Werewolf,
    Zombie,
    Bat
};

float deltaTime;
Texture MainMenuButtons_Texture,MainMenuBackground_Texture,Map_Texture,healthbar_Texture, credits_Texture, credits_background
        ,volume_up_Texture,volume_down_Texture;

Text mathRevivalText;
Text Quote;
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
float totalGameTime = 0.f;
float menuInputDelay = 0.f;
float attackDelay = 0.f;
const float timeForAttack = 0.15f;
const float MENU_INPUT_COOLDOWN = 0.2f; // Time in seconds between allowed inputs
float soundcontroller = 100;
float beastspawntimer = 0;
float batspawntimer = 0;
float werewolfspawntimer = 0;
float zombiespawntimer = 0;
float enemyAttackDelay = 1.0f;
RectangleShape volumebar[10];
Texture beasttexture,zombieTexture,batTexture,werewolfTexture;
Texture BlueXP, GreenXP, RedXP;


int BlueXPcValue = 10, GreenXPcValue = 30, RedXPcValue = 50;

void creditsInit();
Vector2f unitVector(Vector2f vector) {
    float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
    Vector2f unit = Vector2f(vector.x / magnitude, vector.y / magnitude);
    return unit;
}
void generalCollision(RectangleShape& objectTOBeMovedCollider, RectangleShape& Wall, Sprite& Object);
void quoteUpdate();
void quotesInit();
void GetRandIndex(int &randomIndex);
void MainMenuInput();
void Update();
void Start();
void Draw();

struct character
{
    RectangleShape collider;// Sprite collider
    RectangleShape attackSpace;
    Sprite sprite;
    Texture texture;
    Texture playerspreadsheet;
    float health = 120, xp;
    float Maxhp = 120;
    float speed;
    int level = 1;
    float MeleeDamage;
    Vector2f velocity;
    bool isDead,isAttacking;
    bool revivalCrystal;
    animationstate AnimationState ;
    playerDirection spriteDirection;
    int columnIndex = 0;
    int rowIndex = 0;
    int animationdelaytimer = 0;
    float AutoAttackTimer = 0;
    bool startattack = false;


    void update()
    {
        //spriteDirection = toright;
        if(!isAttacking){
            AnimationState = idle;
        }
        animationdelaytimer++;
        AutoAttackTimer += deltaTime;
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
                attackSpace.setScale(-1, 1);
                spriteDirection = toleft;
                if (!isAttacking) {
                    AnimationState = walking;
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) {
                sprite.move(speed * deltaTime, 0);
                sprite.setScale(1, 1.5);
                attackSpace.setScale(1, 1);
                spriteDirection = toright;
                if (!isAttacking) {
                    AnimationState = walking;
                }
                
            }
            if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) {
                sprite.move(0, speed * deltaTime);
                if (!isAttacking) {
                    AnimationState = walking;
                }

            }
            if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) {
                sprite.move(0, -speed * deltaTime);
                if (!isAttacking) {
                    AnimationState = walking;
                }

            }
            if (AutoAttackTimer >= 1.5){
                AnimationState = attacking;
                if (startattack == false) {
                    columnIndex = 0;
                    isAttacking = true;
                    startattack =  true;
                    AutoAttackTimer = 0;
                }
            }
            collider.setPosition(sprite.getPosition());
            attackSpace.setPosition(sprite.getPosition());
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
                else if (columnIndex == 5){
                        sprite.setTextureRect(IntRect(columnIndex * 98 + 195, rowIndex * 130 + 8, 160, 140));
                        isAttacking = false;
                    }
                break;
            case walking:
                rowIndex = 1;
                columnIndex %= 8;
                sprite.setTextureRect(IntRect(columnIndex * 98 + 30, rowIndex * 130, 120, 148));
                startattack = false;
                isAttacking = false;
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

struct ENEMY
{
    RectangleShape attackBox, collider;
    RectangleShape healthBarHolder, healthBarOfEnemy;
    Vector2f velocity;
    monstertype MonsterType;
    Sprite shape;
    Texture enemyspreadsheet;
    animationstate AnimationState;
    float speed,animationdelaytimer;
    float healthBarWidth;
    int columnindex = 0, rowindex;
    int health, damage,maxHealth;
    bool isAttacking = false, isDead = false, hasDroppedXP = false;
    virtual void start() {
        //virtual start to be edited afterwards in the code
    }
    virtual void update(){}
    virtual ~ENEMY() {}
};
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
vector<XPc> Crystals;

struct BEAST:public ENEMY
{
    float beastAttackTime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            beastAttackTime += deltaTime;
            if (beastAttackTime >= enemyAttackDelay)
            {
                beastAttackTime = 0;
                AnimationState = attacking;
                shanoa.health -= damage;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override{
        MonsterType = Beast;
        shape.setTexture(beasttexture);
        shape.setScale(1.8, 2.5);
        health = 250;
        maxHealth = 250;
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
        healthBarHolder.setFillColor(Color::Black);
        healthBarHolder.setSize(Vector2f(60, 10));
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height/2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(60, 10));
        healthBarWidth = 60;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override{
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x , shape.getPosition().y-70);
        healthBarOfEnemy.setPosition(shape.getPosition().x , shape.getPosition().y - 70);
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
    float zombieAttackTime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            zombieAttackTime += deltaTime;
            if (zombieAttackTime >= enemyAttackDelay)
            {
                zombieAttackTime = 0;
                AnimationState = attacking;
                shanoa.health -= damage;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Zombie;
        shape.setTexture(zombieTexture);
        shape.setScale(2.52, 3.5);
        health = 100;
        maxHealth = 100;
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
        healthBarHolder.setFillColor(Color::Black);
        healthBarHolder.setSize(Vector2f(40, 10));
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height /2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(40, 10));
        healthBarWidth = 40;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x, shape.getPosition().y - 50);
        healthBarOfEnemy.setPosition(shape.getPosition().x , shape.getPosition().y - 50);
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
    float werewolfAttackTime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            werewolfAttackTime += deltaTime;
            if (werewolfAttackTime >= enemyAttackDelay)
            {
                werewolfAttackTime = 0;
                AnimationState = attacking;
                shanoa.health -= damage;
            }
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
        maxHealth = 150;
        damage = 10;
        AnimationState = walking;
        attackBox.setSize(Vector2f(70, 1));
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(128, 128));
        collider.setOrigin(collider.getLocalBounds().width / 2, collider.getLocalBounds().height / 2);
        healthBarHolder.setFillColor(Color::Black);
        healthBarHolder.setSize(Vector2f(50, 10));
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height/2);
        shape.setOrigin(64, 73);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(50, 10));
        healthBarWidth = 50;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        AttackDetection();
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarOfEnemy.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
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
    float batAttacktime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        if (shanoa.sprite.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            batAttacktime += deltaTime;
            if (batAttacktime >= enemyAttackDelay)
            {
                batAttacktime = 0;
                AnimationState = attacking;
                shanoa.health -= damage;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Bat;
        shape.setTexture(batTexture);
        shape.setScale(3, 3);
        health = 50;
        maxHealth = 50;
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
        healthBarHolder.setFillColor(Color::Black);
        healthBarHolder.setSize(Vector2f(30, 10));
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height /2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(30, 10));
        healthBarWidth = 30;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x +5, shape.getPosition().y - 45);
        healthBarOfEnemy.setPosition(shape.getPosition().x + 5, shape.getPosition().y - 45);
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
            shape.setScale(3, 3);
        }
        else {
            shape.setScale(-3, 3);
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


struct sword {
    Sprite shape;
    RectangleShape collider;
    Vector2f velocity;
    float damage = 50;
    float speed;
    float deletiontimer = 0;

    void update()
    {
        shape.move(velocity * deltaTime);
        collider.setPosition(shape.getPosition());
        collider.setOrigin(shape.getOrigin());
        collider.setSize(Vector2f(32,32));
        shape.setOrigin(16, 16);
        collider.setOrigin(15, 7.5);
        deletiontimer += deltaTime;
        
    }
    
};


Gamestate gamestate = mainmenu;

Font defgamefont; // default game font


Text StartGameText, SettingsText, ExitText,LeaderboardText, CreditsText,volumeText,settingsmenuText;
Text DEV_T, TEAMNAME, NAMES, prof, teamname;
Text GameOverText, ScoreText, RestartText;


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
string quotes[5] = {
    "Games don't make you violent, lag does.",
    "It�s me, Mario!",
    "Ah Shit, Here We Go Again.",
    "Press F to Pay Respect.",
    "Nah, I'd Win."
};

struct MathEquation {
        Sprite sprite;
    Text userAnsText;
};

MathEquation SurvivalEquation;
int EquationsAns[8] = {3,4,4,2,7,4,1,6};
    string userInput = "";
    bool MathRevivalON;

int main()
{
    SurvivalEquation.userAnsText.setFont(defgamefont);
    SurvivalEquation.userAnsText.setCharacterSize(30);
    SurvivalEquation.userAnsText.setFillColor(sf::Color::White);
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
            if (event.type == Event::TextEntered)
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
                                gameOverSoundPlayed = false;
                                GameloopMusic.play();
                                MathRevivalON = false;
                                quoteUpdate();
                                shanoa.isDead = false;
                                shanoa.health = shanoa.Maxhp;
                            }
                            else
                            {
                                gamestate = mainmenu;
                                MainMenuMusic.play();
                                MathRevivalON = false;
                                gameOverSoundPlayed = false;
                                GameOverSound.stop();
                                menuInputDelay = 0.f;
                            }
                            userInput = "";
                        }
                    }
                }
            }
            SurvivalEquation.userAnsText.setString(userInput);
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
    shanoa.attackSpace.setSize(Vector2f(80, 45));
    shanoa.attackSpace.setFillColor(Color::Black);
    shanoa.attackSpace.setOrigin(0, shanoa.attackSpace.getLocalBounds().height / 2);
    shanoa.MeleeDamage = 90;
    shanoa.sprite.setOrigin(66, 74);
    shanoa.sprite.setScale(1, 1.5);
    shanoa.AnimationState = idle;
    shanoa.revivalCrystal = 1;
    healthbar.setTexture(healthbar_Texture);
    healthbar.setScale(0.84, 1.2);
}

void inRange(float& damage,shared_ptr< ENEMY>& Enemy) {
    if (Enemy->health >= damage)
    {
        Enemy->health -= damage;
        float decreaseRatio = float(Enemy->health) / float(Enemy->maxHealth);
        Enemy->healthBarOfEnemy.setSize(Vector2f(Enemy->healthBarWidth * (decreaseRatio), 10));
    }
    else
        Enemy->health = 0;
    cout << damage;
}

void enemiesInAttackSpace(shared_ptr<ENEMY>& Enemy) {
    if (Enemy->collider.getGlobalBounds().intersects(shanoa.attackSpace.getGlobalBounds()))
    {
        inRange(shanoa.MeleeDamage, Enemy);
    }
}


void MapInit() {
    Map_Texture.loadFromFile("Assets\\mapfinal.png");
    Map_Texture.setRepeated(true);
    Map.setTextureRect(IntRect(0,0,20000,20000));
    Map.setTexture(Map_Texture);
    Map.setPosition(-10000, -10000);
}

void EnemySpawn() {
    Vector2f randomspawnpoint = Vector2f(shanoa.sprite.getPosition().x + (rand() % 2001 - 1000), shanoa.sprite.getPosition().y + (rand() % 2001 - 1000));
    if(enemies.size() <= 25){
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
    SurvivalEquation.sprite.setTexture(equationSpriteSheet);
    SurvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));
    SurvivalEquation.sprite.setScale(0.3/1.5, 0.45/1.5);

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

void meleeAttack() {
    if (shanoa.isAttacking) {
        attackDelay += deltaTime;
        if (attackDelay >= timeForAttack) {
            attackDelay = 0;
            for (int i = 0; i < enemies.size(); ++i)
            {
                enemiesInAttackSpace(enemies[i]);
            }
        }
    }
    else
        attackDelay = 0;
}

void GetRandIndex(int &randomIndex)
{randomIndex = rand() % 6;}

int indexForRandomQuote() {
    return rand() % 5;
}

void quotesInit() {
    Quote.setFont(defgamefont);
    Quote.setString(quotes[indexForRandomQuote()]);
    Quote.setCharacterSize(30);
    Quote.setFillColor(Color::White);
    Quote.setOrigin(Quote.getLocalBounds().width / 2, Quote.getLocalBounds().height / 2);
    Quote.setPosition(0, 300);
}

void quoteUpdate() {
    Quote.setString(quotes[indexForRandomQuote()]);
    Quote.setOrigin(Quote.getLocalBounds().width / 2, Quote.getLocalBounds().height / 2);
}

void swordFullCollisionAndDamage() {
   
        /*------------------enemys-----------------*/
        for (int i = 0; i < swords.size(); i++) {
            swords[i].update();
            bool SwordIsRemoved = false;
            for (int j = 0; j < enemies.size(); j++) {
                if (swords[i].collider.getGlobalBounds().intersects(enemies[j]->collider.getGlobalBounds())) {
                    enemies[j]->health -= swords[i].damage;
                    float decreaseRatio = float(enemies[j]->health) / float(enemies[j]->maxHealth);
                    enemies[j]->healthBarOfEnemy.setSize(Vector2f(enemies[j]->healthBarWidth * (decreaseRatio), 10));
                    swords.erase(swords.begin() + i);
                    cout << " bb ";
                    cout << enemies[j]->health << ' ';
                    SwordIsRemoved = true;
                    break;
                }
            }
            if (SwordIsRemoved)
                break;
        }







        /*------------------obstacles-----------------*/





       
      
    
}

// For All Game Collision
void globalCollsion() {
  
    swordFullCollisionAndDamage();
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
    MapInit();
    swordspritesheet.loadFromFile("Assets\\SWORDS.png");
    MainmenuInit();
    GameOverInit();
    CharacterInit();
    MapInit();
    creditsInit();
    SettingsMenuInit();
    view.setCenter(10000, 9800);
    window.setView(view);
    quotesInit();

    
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
        quoteUpdate();
        totalGameTime = 0;


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
            SurvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));

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
                if (swords[i].collider.getGlobalBounds().intersects(enemies[j]->collider.getGlobalBounds())) {
                    //cout << " bb ";
                    enemies[j]->health -= swords[i].damage;
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
        meleeAttack();
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
 
        
        
        if (Mouse::isButtonPressed(Mouse::Left) && restartButton.getGlobalBounds().contains(mouseWorldpos))/// change to keyboard
        {
            GameOverSound.stop();
            GameOverSound.play();
            gamestate = mainmenu;
            view.setCenter(10000, 9800); // Center view back on main menu
            //reset gameover sound so it works next time
            gameOverSoundPlayed = false;
            selectedMenuButtonIndex = 0;
            menuInputDelay = 0.f;
        }
        if (Mouse::isButtonPressed(Mouse::Left) && MathRevivlaButton.getGlobalBounds().contains(mouseWorldpos) && shanoa.revivalCrystal) //// change to keyboard
        {
            MathRevivalON = true;
        }
    }
    if (gamestate != gameover)
    {
        userInput = ""; // delete the last user input   
        SurvivalEquation.userAnsText.setString("");
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
        //window.draw(shanoa.collider);
        for (int i = 0; i < swords.size(); i++)
        {
            window.draw(swords[i].shape);
        }

      /*  window.draw(shanoa.attackSpace);*/
        window.draw(shanoa.sprite);
        window.draw(healthbar);
        
        for (int i = 0; i < Crystals.size(); i++)
        {
            if (Crystals[i].isCollected == false) {
                window.draw(Crystals[i].sprite);
            }
        }
        for (int i = 0;i < enemies.size();i++) {
           /* window.draw(enemies[i]->collider);*/
            window.draw(enemies[i]->healthBarHolder);
            window.draw(enemies[i]->healthBarOfEnemy);
            window.draw(enemies[i]->shape);
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

        Quote.setPosition(viewCenter.x, viewCenter.y + 275.f);


        SurvivalEquation.sprite.setPosition(viewCenter.x - SurvivalEquation.sprite.getGlobalBounds().width / 2.f , viewCenter.y + 100.f);
        equationAnsCellBox.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f , viewCenter.y + 160.f);
        SurvivalEquation.userAnsText.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f +5, viewCenter.y + 163.f);//

        window.draw(GameOverText);
        window.draw(ScoreText);

        if (MathRevivalON)
        {
             window.draw(SurvivalEquation.sprite);
             window.draw(equationAnsCellBox);  
             window.draw(SurvivalEquation.userAnsText);
        } 
        else
        {
            window.draw(restartButton);
            window.draw(RestartText);
            if (shanoa.revivalCrystal)
            {
                window.draw(MathRevivlaButton);
                window.draw(mathRevivalText);
            }
            window.draw(Quote);
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
