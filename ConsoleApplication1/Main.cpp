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
    IDLE, ATTRACT, ABSORB
};

enum droptype {
    xpc,
    magnet,
    healthregain,
    mathrevivalactivator,
    speedboost
};


enum ObstacleType
{
    tree,
    rock,
    statue,
    objectwillbeadded,
    wall
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
    Bat,
    Boss
};

float deltaTime;

Text mathRevivalText;
Text Quote;
Text xpBarText;
Text HMtext;
Text HMindicator;
bool MathRevivalON;


Texture MainMenuButtons_Texture, MainMenuBackground_Texture, Map_Texture, healthbar_Texture, credits_Texture, credits_background
, volume_up_Texture, volume_down_Texture;
Texture equationSpriteSheet;
Texture swordspritesheet;
Texture beasttexture, zombieTexture, batTexture, werewolfTexture, aresWalkTexture,aresAttackTexture;
Texture BlueXP, GreenXP, RedXP;
Texture levelup;
Texture lock;
Texture HMbuttonTexture;


Sprite levelupsprite;
bool showLevelUp = false;
bool BossthemeIsPlayed = 0;
bool preBossThemeIsPlayed = 0;
bool bossHasSpawned = 0;
bool isday = true;

float levelUpDisplayTimer = 0.0f;
Sprite MainMenuButtons, MainMenuBackground, Map, healthbar, creditsbutton, creditback, volume_up, volume_down, settingsBackground;
Sprite MathRevivalLock;
Sprite HMbutton;

View view;
Vector2i mouseScreenpos;
Vector2f mouseWorldpos;
int selectedGameOverOptionIndex = 0;
RectangleShape StartButton(Vector2f(490, 110)), SettingsButton(Vector2f(490, 110)), LeaderboardButton(Vector2f(490, 110)),
ExitButton(Vector2f(490, 110)), creditsButton(Vector2f(490, 110)), MathRevivalButton(Vector2f(250, 50)), restartButton(Vector2f(250, 50));
RectangleShape GiveUpButton(Vector2f(250, 50)); // *** Add shape for the Give Up button *** 
RectangleShape equationAnsCellBox;
RectangleShape gameOverOverlay, LeaderboardOverlay; // red color in gameover background
RectangleShape xpBarHolder;
RectangleShape xpBar;
RectangleShape DN_overlay;
float xpBarWidth;
FloatRect StartButtonBounds, SettingsButtonBounds, LeaderboardButtonBounds, ExitButtonBounds, creditsButtonBounds, volumeUpBounds,
volumeDownBounds;
FloatRect HMbuttonbounds;
RectangleShape menuCursor;
FloatRect MathRevivlaButtonBounds, RestartButtonBounds, GiveUpButtonBounds; // *** Add bounds for Give Up button ***
Text nametext;
Listener GameVolume;
int selectedMenuButtonIndex = 0; // 0 for Start, 1 for Settings, 2 for Leaderboard, 3 for Exit
float volumebarcontroller;
int randIndex;// equations elements random index
const int MAX_OBSTACLES = 25;




Sound MainMenuMusic, GameOverSound, GameloopMusic, TheLegendaryTutor,preBossSpawnSound,BossTheme;
SoundBuffer MainMenuMusic_source, GameOverSound_source, GameloopMusic_source, TheLegendaryTutor_voice, preBossSpawnSoundBuffer,BossThemeBuffer;
bool gameOverSoundPlayed = false;

sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
RenderWindow window(desktopMode, "Vampire Survivors :The path to the legendary formula", sf::Style::Fullscreen);
//fullscreen fix
float healthRatio;
float freezeTimeForBossSpawning = 0.f;
float shootingtime = 0.f;
float shootingrate = 2;
float totalGameTime = 0.f;
float menuInputDelay = 0.f;
float attackDelay = 0.f;
float HorrorModeDelay = 0.4f;
float HorrorModeTimerDelay = 0.f;
const float timeForAttack = 0.4f;
const float MENU_INPUT_COOLDOWN = 0.5f; // Time in seconds between allowed inputs
float postTransitionCooldown = 0.f;     // Make sure this is also declared (it is in your code around source 151)
const float POST_TRANSITION_DELAY = 0.6f; // Define the delay duration in seconds
float soundcontroller = 100;
float beastspawntimer = 0;
float batspawntimer = 0;
float aresspawntimer = 0;
float werewolfspawntimer = 0;
float zombiespawntimer = 0;
float enemyAttackDelay = 1.0f;
float HMfactor = 1;
float DN_timer = 0.0f;
float DN_duration = 30.0f;
RectangleShape volumebar[10];
multimap<float, string> leaderboardEntriesMap; // Key: -score (float), Value: playerName (string)

int BlueXPcValue = 10, GreenXPcValue = 30, RedXPcValue = 50;
bool HMactive = false;
bool checklevel = false;
bool freezeTimeIsOn = false;
Vector2f unitVector(Vector2f vector) {
    float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
    if (magnitude == 0) {
        return Vector2f(0, 0); // Or handle as needed, perhaps return the zero vector
    }
    Vector2f unit = Vector2f(vector.x / magnitude, vector.y / magnitude);
    return unit;
}
Sound shanoadamaged, swordsound;
SoundBuffer shanoadamaged_source, swordsound_source;
void generalCollision(RectangleShape& objectTOBeMovedCollider, RectangleShape& Wall, Sprite& Object);
void quoteUpdate();
void quotesInit();
void GetRandIndex(int& randomIndex);
void MainMenuInput();
void PauseMenuInit();
void handleNameInput(sf::Event& event);
void stopSounds();
void SaveLeaderboard();
void LoadLeaderboard();
void creditsInit();
void MapInit();
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
    Sound damageSFX;
    SoundBuffer damageSound;
    float health = 200;
    float Maxhp = 200;
    int xp = 0, MaxXp = 100;
    float speed = 250;
    float originalSpeed = 250;//this is used to stop speed boost from overlapping speeds if collected more than 1 at a time
    int level = 1;
    float MeleeDamage;
    Vector2f velocity;
    bool isDead, isAttacking;
    bool RevivalScrollAcquired;
    bool canThrowSwords = false;
    animationstate AnimationState;
    playerDirection spriteDirection;
    int columnIndex = 0;
    int rowIndex = 0;
    int animationdelaytimer = 0;
    float AutoAttackTimer = 0;
    bool startattack = false;
    // *** Add members for revival tracking and score calculation ***
    bool hasRevived = false;
    float timeAtFirstDeath = 0.f; // totalGameTime when the player first died
    void updatelevel() {
        if (xp >= MaxXp) {
            float excessXP = xp - MaxXp;
            level++;
            MaxXp += (level - 1) * 20 + 100;
            xp = excessXP;
            Maxhp += 10;
            health += 10;
            MeleeDamage += 10;
            speed += 5;
            originalSpeed += 5;
            showLevelUp = true;
            levelUpDisplayTimer = 0.0f;
        }
        if (level >= 2) {
            canThrowSwords = true;
        }
        //cout << xp << ' ' << level <<" "<< "level up sprite : " <<  showLevelUp << '\n';
    }

    void update()
    {
        //spriteDirection = toright;
        if (!isAttacking) {
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
            if (AutoAttackTimer >= 1) {
                AnimationState = attacking;
                if (startattack == false) {
                    columnIndex = 0;
                    isAttacking = true;
                    startattack = true;
                    AutoAttackTimer = 0;
                }
            }
            collider.setPosition(sprite.getPosition());
            attackSpace.setPosition(sprite.getPosition());
        }
        updatelevel();
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
                else if (columnIndex == 5) {
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
    float speed, animationdelaytimer;
    float healthBarWidth , healthBarHeight;
    int columnindex = 0, rowindex;
    int health, damage, maxHealth;
    bool isAttacking = false, isDead = false, hasDroppedXP = false;
    virtual void start() {
        //virtual start to be edited afterwards in the code
    }
    virtual void update() {}
    virtual ~ENEMY() {}
};

//variables for power ups
bool SpeedBoostEffectActive = false;
float SpeedBoostEffectTimer = 0.0f;
float EffectDuration = 10.0f;
float freezeDuration = 4.1f;
Texture PowerupsTexture[5];

struct XPc
{
    Sprite sprite;
    bool isCollected = false;
    bool CrystalsRemove = false;
    int xpValue;
    float lifetime = 35.0f;  // Crystal will exist for 35 seconds 
    CrystalState state = IDLE;
    droptype DropType;

    // Animation parameters
    float absorbSpeed = 200.0f; // Speed during final absorption
    float absorbDistance = 75.0f; // Distance at which absorption animation starts
    float attractRange = 160.0f; // Range at which crystals start getting attracted
    float attractSpeed = 90.0f; // Base speed for attraction
    float returnSpeed = 90.0f;

    Vector2f direction;
    Vector2f originalPosition;

    XPc(float x, float y, Texture& tex, int xp, droptype drop)
    {
        sprite.setTexture(tex);
        sprite.setPosition(x, y);
        DropType = drop;
        sprite.setScale(0.2f, 0.4f);
        // Center the origin for proper positioning
        sprite.setOrigin(tex.getSize().x / 2.0f, tex.getSize().y / 2.0f);
        sprite.setTextureRect(IntRect(0, 0, tex.getSize().x, tex.getSize().y));
        xpValue = xp;

        originalPosition = Vector2f(x, y);
    }

    void update(float deltaTime, const Vector2f& playerPosition)
    {
        // Update lifetime regardless of state
        lifetime -= deltaTime;
        if (lifetime <= 0.0f)
        {
            CrystalsRemove = true;
            return;
        }

        // Calculate distance to player
        Vector2f toPlayer = playerPosition - sprite.getPosition();
        float distToPlayer = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        // Normalize direction to player
        Vector2f normalizedToPlayer = toPlayer;
        if (distToPlayer > 0) {
            normalizedToPlayer.x /= distToPlayer;
            normalizedToPlayer.y /= distToPlayer;
        }

        Vector2f toOriginal = originalPosition - sprite.getPosition();
        float distToOriginal = sqrt(toOriginal.x * toOriginal.x + toOriginal.y * toOriginal.y);

        Vector2f normalizedToOriginal = toOriginal;
        if (distToOriginal > 0) {
            normalizedToOriginal.x /= distToOriginal;
            normalizedToOriginal.y /= distToOriginal;
        }

        // Animation state machine
        switch (state) {
        case IDLE:
            // If player is within attract range, it transfers to the attract mode
            if (distToPlayer <= attractRange)
            {
                state = ATTRACT;
            }
            // If not close to original position, move back toward it
            else if (distToOriginal > 15.0f)
            {
                sprite.move(normalizedToOriginal * returnSpeed * deltaTime);
            }
            break;

        case ATTRACT:

            if (distToPlayer > attractRange)
            {
                state = IDLE;
            }
            // Move toward player with increasing speed based on distance
            else if (distToPlayer <= absorbDistance)
            {
                // if it's close -> transition to final absorption
                state = ABSORB;
            }
            else
            {
                float speedFactor = 1.0f + (attractRange - distToPlayer) / attractRange * 2.0f;
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
                if (DropType == healthregain) {
                    shanoa.health += 100;
                    if (shanoa.health > shanoa.Maxhp) {
                        shanoa.health = shanoa.Maxhp;
                    }
                }
                else if (DropType == mathrevivalactivator) {
                    shanoa.RevivalScrollAcquired = true;
                }
                else if (DropType == speedboost) {
                    SpeedBoostEffectTimer = 0;
                    SpeedBoostEffectActive = true;
                }
            }
            else
            {
                sprite.setScale(currentScale, currentScale * 2.0f);
            }
            break;
        }

    }
};

vector<XPc> Crystals;

void PowerUPTexturesInit()
{
    PowerupsTexture[healthregain].loadFromFile("Assets\\HealthRegain.png");
    PowerupsTexture[speedboost].loadFromFile("Assets\\SpeedBoost.png");
    PowerupsTexture[mathrevivalactivator].loadFromFile("Assets\\mathrevivalscroll.png");
};

struct BEAST :public ENEMY
{
    float beastAttackTime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        beastAttackTime += deltaTime;
        if (shanoa.collider.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            AnimationState = attacking;
            if (beastAttackTime >= enemyAttackDelay)
            {
                beastAttackTime = 0;
                shanoa.health -= damage;
                health -= damage/2.0f;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Beast;
        shape.setTexture(beasttexture);
        shape.setScale(1.8, 2.5);
        health = 400 * HMfactor;
        maxHealth = 400 * HMfactor;
        speed = 100;
        damage = 30 * HMfactor;
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
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(60, 10));
        healthBarWidth = 60;
        healthBarHeight = 10;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x, shape.getPosition().y - 70);
        healthBarOfEnemy.setPosition(shape.getPosition().x, shape.getPosition().y - 70);
        if (!freezeTimeIsOn)
        {
            AttackDetection();
            playertargeting();
        }
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 1;
            columnindex = columnindex % 4;
            if (!freezeTimeIsOn)
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 2;
            columnindex = columnindex % 8;
        }
        shape.setTextureRect(IntRect(128 * columnindex + 10, 64 * rowindex, 128, 64));
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
            if (randValue < 20)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue, xpc));

            else if (randValue >= 20 && randValue < 50)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue, xpc));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue, xpc));

            hasDroppedXP = true;
        }
        if (isDead) {
            int randValue = (rand() + 1) % 100;
            Vector2f position = shape.getPosition();
            if (randValue < 10) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[healthregain], 0, healthregain));
            }
            else if (randValue >= 10 && randValue < 25) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[speedboost], 0, speedboost));
            }
            else if (randValue >= 25 && randValue < 40 && !shanoa.hasRevived && !shanoa.RevivalScrollAcquired) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[mathrevivalactivator], 0, mathrevivalactivator));
            }
        }
    }
};
struct ZOMBIE :public ENEMY
{
    float zombieAttackTime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        zombieAttackTime += deltaTime;
        if (shanoa.collider.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            AnimationState = attacking;
            if (zombieAttackTime >= enemyAttackDelay)
            {
                zombieAttackTime = 0;
                shanoa.health -= damage;
                health -= damage / 2.0f;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Zombie;
        shape.setTexture(zombieTexture);
        shape.setScale(2.52, 3.5);
        health = 250 * HMfactor;
        maxHealth = 250 * HMfactor;
        speed = 120;
        damage = 10 * HMfactor;
        attackBox.setSize(Vector2f(30, 1));
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
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(40, 10));
        healthBarWidth = 40;
        healthBarHeight = 10;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x, shape.getPosition().y - 50);
        healthBarOfEnemy.setPosition(shape.getPosition().x, shape.getPosition().y - 50);
        if (!freezeTimeIsOn)
        {
            AttackDetection();
            playertargeting();
        }
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 2;
            columnindex = columnindex % 8;
            if (!freezeTimeIsOn)
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 1;
            columnindex = columnindex % 7;
        }
        shape.setTextureRect(IntRect(32 * columnindex, 32 * rowindex, 32, 32));
       
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
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue, xpc));

            else if (randValue >= 5 && randValue < 15)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue, xpc));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue, xpc));

            hasDroppedXP = true;
        }
        if (isDead) {
            int randValue = (rand() + 1) % 100;
            Vector2f position = shape.getPosition();
            if (randValue < 5) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[healthregain], 0, healthregain));
            }
            else if (randValue >= 5 && randValue < 10) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[speedboost], 0, speedboost));
            }
            else if (randValue >= 10 && randValue < 15 && !shanoa.hasRevived && !shanoa.RevivalScrollAcquired) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[mathrevivalactivator], 0, mathrevivalactivator));
            }
        }
    }
};

struct WEREWOLF :public ENEMY
{
    float werewolfAttackTime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        werewolfAttackTime += deltaTime;
        if (shanoa.collider.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            AnimationState = attacking;
            if (werewolfAttackTime >= enemyAttackDelay)
            {
                werewolfAttackTime = 0;
                shanoa.health -= damage;
                health -= damage / 2.0f;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Werewolf;
        shape.setTexture(werewolfTexture);
        shape.setPosition(200, 200);
        speed = 200;
        health = 300 * HMfactor;
        maxHealth = 300 * HMfactor;
        damage = 25 * HMfactor;
        AnimationState = walking;
        attackBox.setSize(Vector2f(70, 1));
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(128, 128));
        collider.setOrigin(collider.getLocalBounds().width / 2, collider.getLocalBounds().height / 2);
        healthBarHolder.setFillColor(Color::Black);
        healthBarHolder.setSize(Vector2f(50, 10));
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
        shape.setOrigin(64, 73);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(50, 10));
        healthBarWidth = 50;
        healthBarHeight = 10;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        if (!freezeTimeIsOn)
        {
            AttackDetection();
            playertargeting();
        }
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
            if (!freezeTimeIsOn)
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
            if (randValue < 10)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue, xpc));

            else if (randValue >= 10 && randValue < 25)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue, xpc));

            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue, xpc));

            hasDroppedXP = true;
        }
        if (isDead) {
            int randValue = (rand() + 1) % 100;
            Vector2f position = shape.getPosition();
            if (randValue < 10) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[healthregain], 0, healthregain));
            }
            else if (randValue >= 10 && randValue < 20) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[speedboost], 0, speedboost));
            }
            else if (randValue >= 20 && randValue < 30 && !shanoa.hasRevived && !shanoa.RevivalScrollAcquired) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[mathrevivalactivator], 0, mathrevivalactivator));
            }
        }
    }
};

struct BAT :public ENEMY
{
    float batAttacktime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        batAttacktime += deltaTime;
        if (shanoa.collider.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            AnimationState = attacking;
            if (batAttacktime >= enemyAttackDelay)
            {
                batAttacktime = 0;
                shanoa.health -= damage;
                health -= damage / 2.0f;
            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Bat;
        shape.setTexture(batTexture);
        shape.setScale(3, 3);
        health = 100 * HMfactor;
        maxHealth = 100 * HMfactor;
        speed = 240;
        damage = 3 * HMfactor;
        attackBox.setSize(Vector2f(20, 1));
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
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(30, 10));
        healthBarWidth = 30;
        healthBarHeight = 10;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition());
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 40);
        healthBarHolder.setPosition(shape.getPosition().x + 5, shape.getPosition().y - 45);
        healthBarOfEnemy.setPosition(shape.getPosition().x + 5, shape.getPosition().y - 45);
        if (!freezeTimeIsOn)
        {
            AttackDetection();
            playertargeting();
        }
        animationdelaytimer++;
        if (animationdelaytimer >= 2) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            rowindex = 0;
            columnindex = columnindex % 3;
            if (!freezeTimeIsOn)
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            rowindex = 2;
            columnindex = columnindex % 10;
        }
        shape.setTextureRect(IntRect(48.2 * columnindex, 35 * rowindex, 48.2, 35));
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
            if (randValue == 1)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue, xpc));
            else if (randValue > 1 && randValue < 10)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue, xpc));
            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue, xpc));

            hasDroppedXP = true;
        }
        if (isDead) {
            int randValue = (rand() + 1) % 100;
            Vector2f position = shape.getPosition();
            if (randValue < 5) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[healthregain], 0, healthregain));
            }
            else if (randValue > 5 && randValue < 10) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[speedboost], 0, speedboost));
            }
        }
    }
};
struct Ares :public ENEMY
{
    float aresAttacktime = 0;
    void playertargeting()
    {
        velocity = shanoa.sprite.getPosition() - shape.getPosition();
        velocity = unitVector(velocity) * speed;
    }
    void AttackDetection() {
        aresAttacktime += deltaTime;
        if (shanoa.collider.getGlobalBounds().intersects(attackBox.getGlobalBounds()))
        {
            AnimationState = attacking;
            if (aresAttacktime >= enemyAttackDelay)
            {
                aresAttacktime = 0;
                shanoa.health -= damage;

            }
        }
        else
            AnimationState = walking;
    }
    void start() override {
        MonsterType = Boss;
        shape.setScale(4.5, 5);
        health = 3000 * HMfactor;
        maxHealth = 3000 * HMfactor;
        speed = 100;
        damage = 55 * HMfactor;
        attackBox.setSize(Vector2f(150, 1));
        shape.setPosition(200, -200);
        shape.setOrigin(83/2,(float(263) / float(3))/2);
        AnimationState = walking;
        collider.setFillColor(Color::Yellow);
        collider.setSize(Vector2f(100, 200));
        collider.setOrigin(collider.getLocalBounds().width / 2 , collider.getLocalBounds().height / 2 );
        attackBox.setFillColor(Color::Red);
        attackBox.setOrigin(attackBox.getLocalBounds().width / 2, attackBox.getLocalBounds().height / 2);
        healthBarHolder.setFillColor(Color::Black);
        healthBarHolder.setSize(Vector2f(100,30));
        healthBarHolder.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
        healthBarOfEnemy.setFillColor(Color::Green);
        healthBarOfEnemy.setSize(Vector2f(100, 30));
        healthBarWidth = 100;
        healthBarHeight = 30;
        healthBarOfEnemy.setOrigin(healthBarHolder.getLocalBounds().width / 2, healthBarHolder.getLocalBounds().height / 2);
    }
    void update() override {
        collider.setPosition(shape.getPosition().x+20 , shape.getPosition().y+50);
        attackBox.setPosition(shape.getPosition().x, shape.getPosition().y - 10);
        healthBarHolder.setPosition(shape.getPosition().x , shape.getPosition().y - 90);
        healthBarOfEnemy.setPosition(healthBarHolder.getPosition());
        if(!freezeTimeIsOn)
        AttackDetection();
        animationdelaytimer++;
        if (animationdelaytimer >= 4) {
            columnindex++;
            animationdelaytimer = 0;
        }
        if (AnimationState == walking) {
            shape.setTexture(aresWalkTexture);
            rowindex = 0;
            columnindex = columnindex % 4;
            shape.setOrigin(50/2, 96/2);
            shape.setTextureRect(IntRect(47 * columnindex, 0, 47, 96));
            if (columnindex == 3)
            {
                shape.setOrigin(50 / 2-5, 96 / 2);
                shape.setTextureRect(IntRect(47 * columnindex, 0, 47, 96));
            }
            shape.move(velocity * deltaTime);
        }
        else if (AnimationState == attacking) {
            shape.setTexture(aresAttackTexture);
            rowindex = 2;
            columnindex = columnindex % 9;


            if (columnindex == 0)
            {
                shape.setOrigin((83 / 2) + 22, (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 4, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 1)
            {
                shape.setOrigin((83 / 2) + 10, (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 4, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 2)
            {
                shape.setOrigin((83 / 2) + 17, (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 17, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 3)
            {
                shape.setOrigin((83 / 2)-10 , (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 17, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 4)
            {
                shape.setOrigin((83 / 2)-17 , (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 9, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 5)
            {
                shape.setOrigin((83 / 2) - 19, (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 4, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 6)
            {
                shape.setOrigin((83 / 2)-10 , (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 10, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 7)
            {
                shape.setOrigin((83 / 2) +5, (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 4, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 8)
            {
                shape.setOrigin((83 / 2)+20 , (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 4, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
            else if (columnindex == 9)
            {
                shape.setOrigin((83 / 2) + 20, (float(263) / float(3)) / 2);
                shape.setTextureRect(IntRect(83 * columnindex - 4, (double(263) / double(3)) * rowindex - 5, 83, (double(263) / double(3)) + 5));
            }
        }
        playertargeting();
        if (shape.getPosition().x > shanoa.sprite.getPosition().x) {
            shape.setScale(-4.5, 5);
            collider.setPosition(shape.getPosition().x-10 , shape.getPosition().y + 50);
            healthBarHolder.setPosition(shape.getPosition().x-10, shape.getPosition().y - 90);
            attackBox.setPosition(shape.getPosition().x+15, shape.getPosition().y +50);
            healthBarOfEnemy.setPosition(healthBarHolder.getPosition());
           
        }
        else {
            shape.setScale(4.5, 5);
            collider.setPosition(shape.getPosition().x + 20, shape.getPosition().y + 50);
            healthBarHolder.setPosition(shape.getPosition().x +20, shape.getPosition().y - 90);
            attackBox.setPosition(shape.getPosition().x, shape.getPosition().y+50 );
            healthBarOfEnemy.setPosition(healthBarHolder.getPosition());
        }
        if (health <= 0) {
            isDead = true;
            health = 0;
            shape.setScale(0, 0);
            bossHasSpawned = 0;
            aresspawntimer = 0;
            BossTheme.stop();
            BossthemeIsPlayed = 0;
            GameloopMusic.play();
            shanoa.level += 1;
            showLevelUp = true;
        }
        if (isDead && hasDroppedXP == false)
        {
            Vector2f position = shape.getPosition();

            // Randomly decide: 5% chance for Red crystal
            int randValue = (rand() + 1) % 100;
            if (randValue == 1)
                Crystals.push_back(XPc(position.x, position.y, RedXP, RedXPcValue, xpc));
            else if (randValue > 1 && randValue < 10)
                Crystals.push_back(XPc(position.x, position.y, GreenXP, GreenXPcValue, xpc));
            else
                Crystals.push_back(XPc(position.x, position.y, BlueXP, BlueXPcValue, xpc));

            hasDroppedXP = true;
        }
        if (isDead) {
            int randValue = (rand() + 1) % 100;
            Vector2f position = shape.getPosition();
            if (randValue <= 50) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[healthregain], 0, healthregain));
            }
            else if (randValue >=50) {
                Crystals.push_back(XPc(position.x, position.y, PowerupsTexture[speedboost], 0, speedboost));
            }
        }
    }
};
struct sword {
    Sprite shape;
    Sound damageSFX;
    SoundBuffer damageSound;
    RectangleShape collider;
    Vector2f velocity;
    float damage = 100;
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

struct Obstacle
{
    Sprite sprite;
    RectangleShape collider;
    bool isActive = false;
    float timeOffCamera;
    bool isOffCamera;
    ObstacleType type;
    float health = 200;//used for luckystatue
    bool hasdroppedScroll = false;
    Obstacle(const Texture& texture, ObstacleType obstacleType) {
        type = obstacleType;
        sprite.setTexture(texture);

        switch (type)
        {
            /*case tree:
                sprite.setScale(2.0f, 3.2f);
                break;*/

        case rock:
            sprite.setScale(0.8f, 1.2f);
            break;

        case statue:
            sprite.setScale(1.7f, 3.0f);
            break;

            /*case objectwillbeadded:
                sprite.setScale(2.0f, 1.0f);
                break; */

        case wall:
            sprite.setScale(0.8f, 1.5f);
            break;
        }

        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

        float Width, Height; // That's for the collider ( The percent that will used )

        switch (type) {
        case wall:
            Width = 0.67f;
            Height = 0.55f;
            break;

        case rock:
            Width = 0.8f;
            Height = 0.8f;
            break;

        case statue:
            Width = 0.7f;
            Height = 0.75f;
            break;

            /*case objectwillbeadded:
                Width = 0.9f;
                Height = 0.5f;
                break;*/

                /*case tree:
                    Width = 0.5f;
                    Height = 0.9f;
                    break;*/
        }

        float colliderWidth = texture.getSize().x * sprite.getScale().x * Width;
        float colliderHeight = texture.getSize().y * sprite.getScale().y * Height;

        collider.setSize(Vector2f(colliderWidth, colliderHeight));
        collider.setOrigin(colliderWidth / 2.0f, colliderHeight / 2.0f);
        collider.setFillColor(Color::Transparent);
        collider.setOutlineColor(Color::Green);
        collider.setOutlineThickness(2.0f);

    }

    Obstacle() {}

    void activate(const Vector2f& position)
    {
        sprite.setPosition(position);
        collider.setPosition(position);
        isActive = true;
        timeOffCamera = 0.0f;
        isOffCamera = false;
    }
};
Obstacle obstacles[MAX_OBSTACLES];
const float MinObstacleRadius = 200.0f; // Minimum distance from player
const float MaxObstacleRadius = 1200.0f;
const float DespawnRadius = 1800.0f;   // Distance to remove obstacles
const float ObstacleTimer = 25.0f;  // Time in seconds before respawning when off camera


Texture obstacleTextures[5];
void LoadObstacleTextures() {
    //obstacleTextures[tree].loadFromFile("Assets\\tree.png");
    obstacleTextures[rock].loadFromFile("Assets\\Rock1.png");
    obstacleTextures[statue].loadFromFile("Assets\\Statue.png");
    //obstacleTextures[objectwillbeadded].loadFromFile("Assets\\");
    obstacleTextures[wall].loadFromFile("Assets\\FirstObstacle.png");
}

void InitObstacles()
{

    LoadObstacleTextures();

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i] = Obstacle();
    }
}

bool IsInCameraView(const Vector2f& position) {
    Vector2f viewCenter = view.getCenter();
    Vector2f viewSize = view.getSize();

    float left = viewCenter.x - viewSize.x / 2;
    float right = viewCenter.x + viewSize.x / 2;
    float top = viewCenter.y - viewSize.y / 2;
    float bottom = viewCenter.y + viewSize.y / 2;


    return (position.x >= left && position.x <= right &&
        position.y >= top && position.y <= bottom);
}

// Display at a random position for obstacle outside player view
Vector2f GetRandomObstaclePosition() {
    Vector2f playerPos = shanoa.sprite.getPosition();

    float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
    float spawnRadius = MinObstacleRadius +
        static_cast<float>(rand() % static_cast<int>(MaxObstacleRadius - MinObstacleRadius));

    Vector2f position;
    position.x = playerPos.x + cos(angle) * spawnRadius;
    position.y = playerPos.y + sin(angle) * spawnRadius;


    position.x = max(-9900.0f, min(position.x, 9900.0f));
    position.y = max(-9900.0f, min(position.y, 9900.0f));

    return position;
}

// Check if a position is valid for obstacle placement (not overlapping)
bool IsValidPosition(const Vector2f& position, float radius) {

    float distToPlayer = sqrt(pow(shanoa.sprite.getPosition().x - position.x, 2) +
        pow(position.y - shanoa.sprite.getPosition().y, 2));

    if (distToPlayer < MinObstacleRadius)
        return false;

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles[i].isActive)
        {
            float distToObstacle = sqrt(pow(obstacles[i].sprite.getPosition().x - position.x, 2) +
                pow(position.y - obstacles[i].sprite.getPosition().y, 2));

            if (distToObstacle < radius * 18) return false;
        }
    }

    return true;
}

ObstacleType GetRandomObstacleType() {
    int randValue = (rand() % 100) + 1;

    //if (randValue < 35)      return tree;
    if (randValue >= 60)
        return rock;

    else if (randValue < 60 && randValue >= 10)
        return wall;

    //else if (randValue < 95) return objectwillbeadded;
    else
        return statue;
}


void SpawnObstacle()
{

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles[i].isActive == false)
        {
            Vector2f position;
            bool validPosition = false;
            int attempts = 0;

            while (validPosition == false && attempts < 20)
            {
                position = GetRandomObstaclePosition();
                validPosition = IsValidPosition(position, 65.0f); // The radius between obstacles
                attempts++;
            }

            if (validPosition && IsInCameraView(position) == false)
            {
                ObstacleType type = GetRandomObstacleType();
                obstacles[i] = Obstacle(obstacleTextures[type], type);
                obstacles[i].activate(position);
                break;
            }
        }
    }
}

// Check for despawn and spawn new ones
// Check for despawn and spawn new ones
void UpdateObstacles(float deltaTime) {
    Vector2f playerPos = shanoa.sprite.getPosition();
    int activeCount = 0;

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {

        if (obstacles[i].isActive == true)
        {

            float distToPlayer = sqrt(pow(obstacles[i].sprite.getPosition().x - playerPos.x, 2) +
                pow(obstacles[i].sprite.getPosition().y - playerPos.y, 2));

            if (distToPlayer > DespawnRadius)
                obstacles[i].isActive = false;
            else
            {
                activeCount++;
                bool inView = IsInCameraView(obstacles[i].sprite.getPosition());

                if (inView == 0)
                {
                    obstacles[i].timeOffCamera += deltaTime;

                    if (obstacles[i].timeOffCamera >= ObstacleTimer)
                        obstacles[i].isActive = false;
                }

                else
                {
                    obstacles[i].isOffCamera = false;
                    obstacles[i].timeOffCamera = 0.0f;
                }
            }
        }
    }

    while (activeCount < MAX_OBSTACLES)
    {
        SpawnObstacle();
        activeCount++;
    }
}


Gamestate gamestate = mainmenu;

Font defgamefont; // default game font


Text StartGameText, SettingsText, ExitText, LeaderboardText, CreditsText, volumeText, settingsmenuText;
Text DEV_T, TEAMNAME, NAMES, prof, teamname;
Text GameOverText, ScoreText, RestartText;
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
    "It\'s me, Mario!",
    "No Risk No Fun.",
    "Press F to Pay Respect.",
    "Nah, I'd Win."
};

struct MathEquation {
    Sprite sprite;
    Text userAnsText;
};

MathEquation SurvivalEquation;
int EquationsAns[8] = { 3,4,4,2,7,4,1,6 };
string userInput = "";

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
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape && postTransitionCooldown <= 0)
            {
                if (gamestate == gameloop)
                {
                    gamestate = paused;
                    BossTheme.pause();
                    preBossSpawnSound.stop();
                    preBossThemeIsPlayed = 0;
                    // Pause game loop music if it's playing
                    GameloopMusic.pause();
                    postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                }
                else if (gamestate == paused)
                {
                    gamestate = gameloop;
                    // Resume game loop music
                    if (!BossthemeIsPlayed)
                        GameloopMusic.play();
                    else
                        BossTheme.play();
                    
                    cout << BossthemeIsPlayed;
                    cout << " " << "done";
                    
                    postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                    selectedMenuButtonIndex = 0; // Reset pause menu selection
                }
            }
            if (event.type == Event::TextEntered)
            {
                if (gamestate == gameover && MathRevivalON) // Math Revival input (only when MathRevivalON is true)
                {

                    if (event.text.unicode == 8)
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
                            //stoi= string to integer
                            if (stoi(userInput) == EquationsAns[randIndex]) // correct answer
                            {
                                gamestate = gameloop; // Go back to gameloop
                                postTransitionCooldown = POST_TRANSITION_DELAY;  // Set cooldown
                                TheLegendaryTutor.stop();
                                GameOverSound.stop();
                                if (!BossthemeIsPlayed)
                                    GameloopMusic.play();
                                else
                                    BossTheme.play();
                                shanoa.sprite.setPosition(0, 0);
                                gameOverSoundPlayed = false;
                                shanoa.RevivalScrollAcquired = 0;
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
                                stopSounds();
                                postTransitionCooldown = POST_TRANSITION_DELAY; // Set cooldown
                                TheLegendaryTutor.stop();
                                MathRevivalON = false; // Turn off Math Revival puzzle
                                bossHasSpawned = 0;
                                shanoa.RevivalScrollAcquired = 0;
                                userInput = ""; // Clear input
                                // Game over screen is skipped, so score won't be saved from here.
                                // If you want to save score on Math Revival fail,
                                // transition back to gameover options instead of mainmenu.
                            }
                        }
                    }
                    SurvivalEquation.userAnsText.setString(userInput); // Update Math Revival text display
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

void handleNameInput(Event& event)
{
    if (event.type == Event::TextEntered)
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
        else if ((event.key.code == Keyboard::Backspace || event.text.unicode == 8) && menuInputDelay >= MENU_INPUT_COOLDOWN / 3)
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
                    finalScore = (shanoa.timeAtFirstDeath * 2.0f) + ((totalGameTime - shanoa.timeAtFirstDeath));
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
                    advance(it, MAX_LEADERBOARD_SIZE); // Move iterator to the 11th element
                    leaderboardEntriesMap.erase(it, leaderboardEntriesMap.end()); // Erase from 11th to the end
                }
                SaveLeaderboard();
            }

            gamestate = leaderboard;
            menuInputDelay = 0.f;
            playerName = ""; // Clear name input field
        }
    }
    playerNameDisplayText.setString(playerName);
}

void SaveLeaderboard()
{
    ofstream outputFile("leaderboard.txt");

    if (outputFile.is_open())
    {
        // Iterate through the map. It's already sorted by key (-score).
        for (const auto& pair : leaderboardEntriesMap) // pair.first is -score, pair.second is name
        {
            outputFile << pair.second << endl; // Write name
            outputFile << fixed << setprecision(2) << -pair.first << endl; // Write positive score (-key)
        }
        outputFile.close();
    }
    else
    {
        cerr << "Error: Unable to open leaderboard.txt" << endl;
    }
}

void LoadLeaderboard()
{
    leaderboardEntriesMap.clear(); // Clear any existing entries
    ifstream inputFile("leaderboard.txt");

    if (inputFile.is_open())
    {
        string name;
        string scoreStr;

        // Read pairs of lines (name and score)
        while (getline(inputFile, name) && getline(inputFile, scoreStr))
        {

            float score = stof(scoreStr); // Convert score string to float
            // Insert into multimap: key is negative score, value is name
            leaderboardEntriesMap.insert({ -score, name });

        }
        inputFile.close();

        const int MAX_LEADERBOARD_SIZE = 10;

        if (leaderboardEntriesMap.size() > MAX_LEADERBOARD_SIZE)
        {
            auto it = leaderboardEntriesMap.begin();
            advance(it, MAX_LEADERBOARD_SIZE);
            leaderboardEntriesMap.erase(it, leaderboardEntriesMap.end());
        }

    }
}

void NameInputInit()
{

    playerName = "";
    nameInputPromptText.setFont(defgamefont);
    nameInputPromptText.setCharacterSize(40);
    nameInputPromptText.setFillColor(sf::Color::White);
    nameInputPromptText.setString("Enter Your Name:");


    playerNameDisplayText.setFont(defgamefont);
    playerNameDisplayText.setCharacterSize(40);
    playerNameDisplayText.setFillColor(sf::Color::Yellow);
    playerNameDisplayText.setString(playerName);

    equationAnsCellBox.setFillColor(sf::Color::Black);
    equationAnsCellBox.setSize(sf::Vector2f(400, 60));
    equationAnsCellBox.setOutlineColor(sf::Color::Blue);
    equationAnsCellBox.setOutlineThickness(3);
}

void stopSounds() {
    swordsound.stop();
    BossTheme.stop();
    preBossThemeIsPlayed = 0;
    preBossSpawnSound.stop();
}
void shooting()
{
    if (shanoa.canThrowSwords) {
        shootingtime += deltaTime;

        if (shootingtime >= shootingrate)
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
            swordsound.play();
        }
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
void xpBarInit() {
    xpBarHolder.setSize(Vector2f(180, 30));
    xpBarHolder.setFillColor(Color::Black);
    xpBarHolder.setOrigin(xpBarHolder.getLocalBounds().width / 2, xpBarHolder.getLocalBounds().height / 2);
    xpBarWidth = 170;
    xpBar.setSize(Vector2f(0, 20));
    xpBar.setFillColor(Color::Red);
    xpBar.setOrigin(85, xpBar.getLocalBounds().height / 2);
    xpBarText.setString("LVL.");
    xpBarText.setOrigin(xpBarText.getLocalBounds().width / 2, xpBarText.getLocalBounds().height / 2);
}
void xpBarDraw() {
    string currentLevel = to_string(shanoa.level);
    xpBarHolder.setPosition(view.getCenter().x - 260, view.getCenter().y + 400);
    xpBar.setPosition(xpBarHolder.getPosition());
    xpBarText.setFont(defgamefont);
    xpBarText.setCharacterSize(15);
    xpBarText.setFillColor(Color::White);
    xpBarText.setPosition(xpBarHolder.getPosition().x-14 , xpBarHolder.getPosition().y -8);
    xpBarText.setString("LVl." + currentLevel);
    xpBar.setSize(Vector2f(xpBarWidth * (float(shanoa.xp) / float(shanoa.MaxXp)), 20));
}
void xpFullReset() {
    shanoa.xp = 0;
    shanoa.level = 1;
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

    ContinueButton.setFillColor(Color(100, 0, 0));
    ContinueButton.setOutlineColor(Color::Yellow);
    ContinueButton.setSize(Vector2f(250, 50));

    PauseReturnToMenuButton.setFillColor(Color(100, 0, 0));
    PauseReturnToMenuButton.setOutlineColor(Color::Yellow);
    PauseReturnToMenuButton.setSize(Vector2f(250, 50));
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
    shanoa.speed = 250;
    shanoa.sprite.setPosition(0, 0);
    shanoa.collider.setSize(Vector2f(60, 125));
    shanoa.collider.setOrigin(shanoa.collider.getLocalBounds().width / 2, shanoa.collider.getLocalBounds().height / 2);
    shanoa.collider.setFillColor(Color::Blue);
    shanoa.attackSpace.setSize(Vector2f(80, 45));
    shanoa.attackSpace.setFillColor(Color::Black);
    shanoa.attackSpace.setOrigin(0, shanoa.attackSpace.getLocalBounds().height / 2);
    shanoa.MeleeDamage = 120;
    shanoa.sprite.setOrigin(66, 74);
    shanoa.sprite.setScale(1, 1.5);
    shanoa.AnimationState = idle;
    shanoa.RevivalScrollAcquired = 0;
    healthbar.setTexture(healthbar_Texture);
    healthbar.setScale(0.84, 1.2);
}

void inRange(float& damage, shared_ptr< ENEMY>& Enemy) {
    if (Enemy->health >= damage)
    {
        Enemy->health -= damage;
        float decreaseRatio = float(Enemy->health) / float(Enemy->maxHealth);
        Enemy->healthBarOfEnemy.setSize(Vector2f(Enemy->healthBarWidth * (decreaseRatio), Enemy->healthBarHeight));
    }
    else
        Enemy->health = 0;
    //cout << damage;
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
    Map.setTextureRect(IntRect(0, 0, 160000, 160000));
    Map.setTexture(Map_Texture);
    Map.setPosition(-80000, -80000);
}

void EnemySpawn() {
    Vector2f randomspawnpoint = Vector2f(shanoa.sprite.getPosition().x + (rand() % 2001 - 1000), shanoa.sprite.getPosition().y + (rand() % 2001 - 1000));
    int currentmonstercount = 10;
        aresspawntimer += deltaTime;
    if ((int)totalGameTime % 5 == 0) {
        currentmonstercount++;
    }
    if (enemies.size() <= currentmonstercount) {
        beastspawntimer += deltaTime;
        batspawntimer += deltaTime;
        werewolfspawntimer += deltaTime;
        zombiespawntimer += deltaTime;
       if (beastspawntimer >= 4 && totalGameTime > 120 && !bossHasSpawned) {

            auto newBeast = make_unique<BEAST>();

            newBeast->start();

            newBeast->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newBeast));

            beastspawntimer = 0;
        }
        if (zombiespawntimer >= 2 && totalGameTime > 30 && !bossHasSpawned) {

            auto newZombie = make_unique<ZOMBIE>();

            newZombie->start();

            newZombie->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newZombie));

            zombiespawntimer = 0;
        }
        if (werewolfspawntimer >= 3 && totalGameTime > 60 && !bossHasSpawned) {

            auto newWerewolf = make_unique<WEREWOLF>();

            newWerewolf->start();

            newWerewolf->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newWerewolf));

            werewolfspawntimer = 0;
        }
        if (batspawntimer >= 1 && totalGameTime < 75 && !bossHasSpawned) {

            auto newBat = make_unique<BAT>();

            newBat->start();

            newBat->shape.setPosition(randomspawnpoint);

            enemies.push_back(move(newBat));

            batspawntimer = 0;
        }
        if (aresspawntimer >= 110 && totalGameTime > 120 && !bossHasSpawned) {
          
                auto newBoss = make_unique<Ares>();
                newBoss->start();
                newBoss->shape.setPosition(randomspawnpoint);
                enemies.push_back(move(newBoss));
                bossHasSpawned = 1;
                freezeTimeIsOn = 1;
                freezeTimeForBossSpawning = 0;
        }
    }
}
void EnemyHandler() {
    for (int i = 0;i < enemies.size();i++) {
        if (enemies[i]->MonsterType == Boss && freezeTimeIsOn) {
            view.setCenter(enemies[i]->shape.getPosition());
        }
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
        shanoa.isDead = true;
    }
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

    GiveUpText.setFont(defgamefont);
    GiveUpText.setString("Give Up");
    GiveUpText.setCharacterSize(20);
    GiveUpText.setFillColor(Color::White);


    GiveUpButton.setFillColor(Color(100, 0, 0));
    GiveUpButton.setOutlineColor(Color::Yellow);
    GiveUpButton.setSize(Vector2f(250, 50));

    equationSpriteSheet.loadFromFile("Assets\\equationSpriteSheet.png");
    SurvivalEquation.sprite.setTexture(equationSpriteSheet);
    SurvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));
    SurvivalEquation.sprite.setScale(0.3 / 1.5, 0.45 / 1.5);

    equationAnsCellBox.setFillColor(Color::Black);
    equationAnsCellBox.setSize(Vector2f(200, 50));
    equationAnsCellBox.setOutlineColor(Color::Red);
    equationAnsCellBox.setOutlineThickness(5);

    restartButton.setFillColor(Color(100, 0, 0));
    restartButton.setOutlineColor(Color::Yellow);

    MathRevivalButton.setFillColor(Color(100, 0, 0));
    MathRevivalButton.setOutlineColor(Color::Yellow);

    restartButton.setSize(Vector2f(250, 50));
    MathRevivalButton.setSize(Vector2f(250, 50));


    gameOverOverlay.setSize(view.getSize()); // Set size based on the view's size
    // Set its origin to the center, so position(view.getCenter()) works correctly
    gameOverOverlay.setOrigin(view.getSize().x / 2.f, view.getSize().y / 2.f);

    gameOverOverlay.setFillColor(Color(100, 0, 0, 120));


    GameOverSound_source.loadFromFile("Assets\\MainMenuMusic.ogg");
    GameOverSound.setBuffer(GameOverSound_source);

    gameOverSoundPlayed = false;

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

            //RESETTING after death for next game
            shanoa.sprite.setPosition(0, 0);
            shanoa.health = 200;
            shanoa.isDead = false;
            totalGameTime = 0.f;
            swords.clear();
            isday = true;
            DN_timer = 0.0f;
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

void HorrorModeButtonInput()
{
    HorrorModeTimerDelay += deltaTime;
    if (HorrorModeTimerDelay >= HorrorModeDelay)
    {
        if (HMbuttonbounds.contains(mouseWorldpos) && Mouse::isButtonPressed(Mouse::Left))
        {
            if (HMactive)
            {
                HMactive = 0;
                HMbutton.setTextureRect(IntRect(0, 0, 393, 144));
                HMbutton.setColor(Color::White);
                /*HMactive = true;*/
            }
            else
            {
                HMactive = 1;
                HMbutton.setTextureRect(IntRect(0, 1 * 140, 393, 144));
                HMbutton.setColor(Color::Green);

                /*HMactive = false;*/
            }
            HorrorModeTimerDelay = 0.f;
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
                bossHasSpawned = 0;
                BossthemeIsPlayed = 0;
                enemies.clear();
                shanoa.health = 200;
                shanoa.isDead = false;
                totalGameTime = 0.f;
                swords.clear();
                isday = true;
                DN_timer = 0.0f;
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

    HMbuttonTexture.loadFromFile("Assets\\HMcheckbox.png");
    HMbutton.setTexture(HMbuttonTexture);
    HMbutton.setTextureRect(IntRect(0, 0, 393, 144));
    HMbutton.setPosition(800, 20380);
    HMbutton.setScale(0.6, 0.6);
    HMbuttonbounds = HMbutton.getGlobalBounds();

    HMtext.setString("Horror mode");
    HMtext.setFont(defgamefont);
    HMtext.setPosition(520, 20400);
    HMtext.setScale(1.2, 1.2);

    HMindicator.setString("HORROR MODE");
    HMindicator.setFont(defgamefont);
    HMindicator.setFillColor(Color::Red);
    HMindicator.setOrigin(HMindicator.getLocalBounds().width / 2, HMindicator.getLocalBounds().height / 2);
    HMindicator.setScale(1.5, 1.5);








    {//volume bar init
        for (int i = 0;i < 10;i++) {
            volumebar[i].setSize(Vector2f(15, 30));
            volumebar[i].setFillColor(Color::White);
            volumebar[i].setPosition(690 + (i * 20), 20307);
        }
    }
}

void Horrormode()
{
    if (HMactive)
    {
        HMfactor = 1.5;
        //cout << "Horror mode" << endl;
    }
    else
    {
        HMfactor = 1;
        //cout << "not Horror" << endl;
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

void GetRandIndex(int& randomIndex)
{
    randomIndex = rand() % 6;
}

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
                enemies[j]->healthBarOfEnemy.setSize(Vector2f(enemies[j]->healthBarWidth * (decreaseRatio), enemies[j]->healthBarHeight));
                swords.erase(swords.begin() + i);
                SwordIsRemoved = true;
                break;
            }
        }
        if (SwordIsRemoved)
            break;
    }
}

// For All Game Collision
void globalCollsion() {

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (obstacles[i].isActive == true)
        {
            generalCollision(shanoa.collider, obstacles[i].collider, shanoa.sprite);
            for (int j = 0; j < enemies.size(); ++j)
            {
                generalCollision(enemies[j]->collider, obstacles[i].collider, enemies[j]->shape);
            }
            for (int k = 0; k < swords.size(); ++k)
            {
                if (swords[k].collider.getGlobalBounds().intersects(obstacles[i].collider.getGlobalBounds()))
                {
                    if (obstacles[i].type == statue) {
                        obstacles[i].health -= swords[k].damage;
                        if (obstacles[i].health <= 0 && !obstacles[i].hasdroppedScroll) {
                            Crystals.push_back(XPc(obstacles[i].sprite.getPosition().x, obstacles[i].sprite.getPosition().y, PowerupsTexture[mathrevivalactivator], 0, mathrevivalactivator));
                            obstacles[i].sprite.setScale(0, 0);
                            obstacles[i].collider.setScale(0, 0);
                            obstacles[i].sprite.setPosition(20000, 20000);
                            obstacles[i].collider.setScale(20000, 20000);
                            obstacles[i].hasdroppedScroll;
                        }
                    }
                    swords.erase(swords.begin() + k);
                    break;
                }
            }
            if (obstacles[i].type == statue) {
                if (shanoa.attackSpace.getGlobalBounds().intersects(obstacles[i].collider.getGlobalBounds())) {
                    obstacles[i].health -= shanoa.MeleeDamage;
                    if (obstacles[i].health <= 0 && !obstacles[i].hasdroppedScroll) {
                        Crystals.push_back(XPc(obstacles[i].sprite.getPosition().x, obstacles[i].sprite.getPosition().y, PowerupsTexture[mathrevivalactivator], 0, mathrevivalactivator));
                        obstacles[i].sprite.setScale(0, 0);
                        obstacles[i].collider.setScale(0, 0);
                        obstacles[i].sprite.setPosition(20000, 20000);
                        obstacles[i].collider.setScale(20000, 20000);
                        obstacles[i].hasdroppedScroll;
                    }
                }
            }
        }
    }
    swordFullCollisionAndDamage();
}

void SpeedBoostFunction() {
    if (SpeedBoostEffectActive) {
        if (shanoa.speed <= shanoa.originalSpeed)
            shanoa.speed = shanoa.speed * 1.5;
        if (SpeedBoostEffectTimer >= EffectDuration) {
            SpeedBoostEffectActive = false;
            shanoa.speed = shanoa.originalSpeed;
        }

    }
}
void freezeTimeFunction() {
    if (freezeTimeIsOn)
    {
        if (!preBossThemeIsPlayed)
        {
            GameloopMusic.stop();
            preBossSpawnSound.setVolume(100);
            preBossSpawnSound.play();
            preBossThemeIsPlayed = 1;
        }
        if (freezeTimeForBossSpawning >= freezeDuration) {
            freezeTimeIsOn = 0;
            preBossThemeIsPlayed = 0;
            preBossSpawnSound.stop();
            if (!BossthemeIsPlayed)
            {
                BossthemeIsPlayed = 1;
                BossTheme.setLoop(true);
                BossTheme.play();
            }

        }
    }
}

void Start()
{
    // code here is only executed at the start of the program
    // initializations of everything

    window.setFramerateLimit(30);


    //Game font initialization
    defgamefont.loadFromFile("VampireZone.ttf");
    lock.loadFromFile("Assets\\lock.png");
    BlueXP.loadFromFile("Assets\\XPc.png");
    GreenXP.loadFromFile("Assets\\GreenXPc.png");
    RedXP.loadFromFile("Assets\\RedXPc.png");
    beasttexture.loadFromFile("Assets\\beastTexture.png");
    zombieTexture.loadFromFile("Assets\\Zombie.png");
    batTexture.loadFromFile("Assets\\Bat.png");
    aresAttackTexture.loadFromFile("Assets\\Boss.png");
    aresWalkTexture.loadFromFile("Assets\\BossWalk.png");
    werewolfTexture.loadFromFile("Assets\\werewolfwhite.png");
    swordspritesheet.loadFromFile("Assets\\SWORDS.png");
    shanoadamaged_source.loadFromFile("Assets\\shanoatakingdamage.ogg");
    shanoadamaged.setBuffer(shanoadamaged_source);
    TheLegendaryTutor_voice.loadFromFile("Assets\\voiceActing.ogg");
    TheLegendaryTutor.setBuffer(TheLegendaryTutor_voice);
    shanoadamaged.setVolume(50);
    swordsound_source.loadFromFile("Assets\\swordsound.ogg");
    preBossSpawnSoundBuffer.loadFromFile("Assets\\prespawnsound.ogg");
    preBossSpawnSound.setBuffer(preBossSpawnSoundBuffer);
    BossThemeBuffer.loadFromFile("Assets\\bosstheme.ogg");
    BossTheme.setBuffer(BossThemeBuffer);
    swordsound.setBuffer(swordsound_source);
    swordsound.setVolume(30);
    levelup.loadFromFile("Assets\\level_up.png");
    levelupsprite.setTexture(levelup);
    levelupsprite.setScale(0.3, 0.5);
    MathRevivalLock.setTexture(lock);
    MathRevivalLock.setOrigin(MathRevivalLock.getLocalBounds().width / 2, MathRevivalLock.getLocalBounds().height / 2);
    MathRevivalLock.setScale(0.1, 0.1);
    DN_overlay.setSize(view.getSize());
    DN_overlay.setOrigin(view.getSize().x / 2.f, view.getSize().y / 2.f);
    DN_timer = 0.0f;
    MapInit();
    MainmenuInit();
    xpBarInit();
    GameOverInit();
    NameInputInit();
    PauseMenuInit();
    CharacterInit();
    LoadLeaderboard();
    creditsInit();
    SettingsMenuInit();
    quotesInit();
    InitObstacles();
    PowerUPTexturesInit();
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
        quoteUpdate();
        totalGameTime = 0;


    }
    else if (gamestate == gameloop)
    {
        // gameloop update

        window.setMouseCursorVisible(false); // get rid of the mouse
        SpeedBoostEffectTimer += deltaTime;
        freezeTimeForBossSpawning += deltaTime;
        totalGameTime += deltaTime; // measure survival time
        UpdateObstacles(deltaTime);
        GameOverSound.stop();
        MainMenuMusic.stop();
        shooting();


        int minutes = static_cast<int>(totalGameTime) / 60; // time calculations for final score
        int seconds = static_cast<int>(totalGameTime) % 60;

        stringstream ss; // time formatting
        ss << "Time: " << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
        ScoreText.setString(ss.str());


        for (int i = 0; i < swords.size(); i++)
        {
            swords[i].update();
            if (swords[i].deletiontimer > 10) {
                swords.erase(swords.begin() + i);
            }
        }
        if (shanoa.isDead)
        {
            //cout << "here";
            GetRandIndex(randIndex);
            SurvivalEquation.sprite.setTextureRect(IntRect(0, 156 * randIndex, 600, 156));
            stopSounds();
            if (!gameOverSoundPlayed)
            {
                //only play if it hasn't been played since the last reset
                GameOverSound.play();
                gameOverSoundPlayed = true; //set the flag so it doesn't play again immediately
            }
            GameloopMusic.stop();
            gamestate = gameover;
            swords.clear();
            selectedMenuButtonIndex = 0;
        }
        if (showLevelUp) {
            levelUpDisplayTimer += deltaTime;
            if (levelUpDisplayTimer >= 2.0f) {
                showLevelUp = false;
                levelUpDisplayTimer = 0.0f;
            }
        }
        EnemySpawn();
        EnemyHandler();
        shanoa.update();
        globalCollsion();
        healthbarhandling();
        freezeTimeFunction();
        Horrormode();
        SpeedBoostFunction();

        for (int i = 0; i < Crystals.size(); i++) {
            // Pass player position to update function
            Crystals[i].update(deltaTime, shanoa.sprite.getPosition());

            if (Crystals[i].isCollected && !Crystals[i].CrystalsRemove) {
                shanoa.xp += Crystals[i].xpValue;
                //cout << "Collected XP: " << Crystals[i].xpValue << " (Total: " << shanoa.xp << ")" << endl;
                Crystals[i].CrystalsRemove = true;
                checklevel = true;
            }
            if (checklevel) {
                shanoa.updatelevel();
            }
        }
        for (int i = Crystals.size() - 1; i >= 0; i--) {
            if (Crystals[i].CrystalsRemove) {
                Crystals.erase(Crystals.begin() + i);
            }
        }
        meleeAttack();
        if (!freezeTimeIsOn)
            view.setCenter(shanoa.sprite.getPosition());
      
        quoteUpdate();
    }

    else if (gamestate == paused)
    {
        window.setMouseCursorVisible(true);

        menuInputDelay += deltaTime;

        Vector2f viewCenter = view.getCenter();
        ContinueButton.setPosition(viewCenter.x - ContinueButton.getLocalBounds().width / 2.f, viewCenter.y - 50.f);
        PauseReturnToMenuButton.setPosition(viewCenter.x - PauseReturnToMenuButton.getLocalBounds().width / 2.f, viewCenter.y + 50.f);

        ContinueButtonBounds = ContinueButton.getGlobalBounds();
        PauseReturnToMenuButtonBounds = PauseReturnToMenuButton.getGlobalBounds();


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

                if (selectedMenuButtonIndex < 0) {
                    selectedMenuButtonIndex = 1;
                }
                else if (selectedMenuButtonIndex > 1) {
                    selectedMenuButtonIndex = 0;
                }
                menuInputDelay = 0; // Reset delay
            }

            if (ContinueButtonBounds.contains(mouseWorldpos)) {
                selectedMenuButtonIndex = 0;
            }
            else if (PauseReturnToMenuButtonBounds.contains(mouseWorldpos)) {
                selectedMenuButtonIndex = 1;
            }

            bool activateSelection = false;
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                activateSelection = true;
            }
            if (Mouse::isButtonPressed(Mouse::Left)) {
                if (selectedMenuButtonIndex == 0 && ContinueButtonBounds.contains(mouseWorldpos)) activateSelection = true;
                if (selectedMenuButtonIndex == 1 && PauseReturnToMenuButtonBounds.contains(mouseWorldpos)) activateSelection = true;
            }

            if (activateSelection && postTransitionCooldown <= 0)
            {
                if (selectedMenuButtonIndex == 0)
                {
                    gamestate = gameloop;
                    if (!BossthemeIsPlayed)
                        GameloopMusic.play();
                    else
                        BossTheme.play();
                    postTransitionCooldown = POST_TRANSITION_DELAY;
                }
                else if (selectedMenuButtonIndex == 1)
                {
                    gamestate = mainmenu;
                    MainMenuMusic.play();
                    GameloopMusic.stop();
                    BossthemeIsPlayed = 0;
                    postTransitionCooldown = POST_TRANSITION_DELAY;
                    enemies.clear();
                    swords.clear();
                    Crystals.clear();
                    shanoa.health = shanoa.Maxhp; // Reset player 
                    shanoa.isDead = false;
                    shanoa.RevivalScrollAcquired = false;
                    totalGameTime = 0.f;
                    gameOverSoundPlayed = false;
                    MathRevivalON = false;
                    userInput = "";
                    selectedMenuButtonIndex = 0; // Reset main menu selection
                    xpFullReset();
                }
                menuInputDelay = 0;
            }
        }


        Vector2f selectedOptionPosition;
        Vector2f selectedOptionSize;
        float cursorAdjust = 3.f;

        if (selectedMenuButtonIndex == 0) {
            selectedOptionPosition = ContinueButton.getPosition();
            selectedOptionSize = ContinueButton.getSize();
        }
        else if (selectedMenuButtonIndex == 1) {
            selectedOptionPosition = (Vector2f(PauseReturnToMenuButton.getPosition().x, PauseReturnToMenuButton.getPosition().y - 25.f));
            selectedOptionSize = PauseReturnToMenuButton.getSize();
        }
        menuCursor.setPosition(selectedOptionPosition.x - cursorAdjust, selectedOptionPosition.y - cursorAdjust);
        menuCursor.setSize(Vector2f(selectedOptionSize.x + cursorAdjust * 2.f, selectedOptionSize.y + cursorAdjust * 2.f));

        view.setCenter(viewCenter); // Keep view centered on pause screen
    }

    else if (gamestate == settings)
    {
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

        HorrorModeButtonInput();

        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            selectedMenuButtonIndex = 0;
        }
    }

    else if (gamestate == leaderboard)
    {
        window.setMouseCursorVisible(true);
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            gamestate = mainmenu;
            selectedMenuButtonIndex = 0;
        }
    }

    else if (gamestate == gameover)
    {
        window.setMouseCursorVisible(true);

        Vector2f viewCenter = view.getCenter();
        restartButton.setPosition(viewCenter.x - restartButton.getLocalBounds().width / 2.f, viewCenter.y + 90.f);
        MathRevivalButton.setPosition(viewCenter.x - MathRevivalButton.getLocalBounds().width / 2.f, viewCenter.y + 175.f);
        GiveUpButton.setPosition(viewCenter.x - GiveUpButton.getLocalBounds().width / 2.f, viewCenter.y + 260.f);

        RestartButtonBounds = restartButton.getGlobalBounds();
        MathRevivlaButtonBounds = MathRevivalButton.getGlobalBounds();
        GiveUpButtonBounds = GiveUpButton.getGlobalBounds();


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
                    if (selectedGameOverOptionIndex < 0) {
                        selectedGameOverOptionIndex = 2;
                    }
                    else if (selectedGameOverOptionIndex > 2) {
                        selectedGameOverOptionIndex = 0;
                    }
                    menuInputDelay = 0.f; // Reset delay after movement
                }

                bool activateSelection = false;

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
                        gamestate = gameloop;
                        MainMenuMusic.stop();
                        GameloopMusic.play();
                        gameOverSoundPlayed = false;
                        selectedMenuButtonIndex = 0;
                        enemies.clear();
                        Crystals.clear();
                        shanoa.health = 200; // Or your starting health
                        shanoa.isDead = false;
                        shanoa.RevivalScrollAcquired = 0;
                        bossHasSpawned = 0;
                        totalGameTime = 0.f;
                        swords.clear();
                        xpFullReset();
                        isday = true;
                        DN_timer = 0.0f;
                    }
                    else if (selectedGameOverOptionIndex == 1 && shanoa.RevivalScrollAcquired) // Math Revival selected
                    {
                        MathRevivalON = true; // Activate Math Revival
                        TheLegendaryTutor.play();
                        menuInputDelay = 0.f; // Reset delay
                    }
                    else if (selectedGameOverOptionIndex == 2) // Give Up selected
                    {
                        GameOverSound.stop();
                        GameOverSound.play();
                        enemies.clear();
                        bossHasSpawned = 0;
                        BossthemeIsPlayed = 0;
                        swords.clear();
                        xpFullReset(); // To Reset XP And Levels
                        gameOverSoundPlayed = false;
                        menuInputDelay = 0.f; // Reset delay BEFORE state change
                        shanoa.RevivalScrollAcquired = 0;
                        gamestate = nameinput; // Transition to name input state
                        playerName = ""; // Clear name for new input
                        // totalGameTime (score) is already stored globally
                        isday = true;
                        DN_timer = 0.0f;
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
                selectedOptionPosition = MathRevivalButton.getPosition();
                selectedOptionSize = MathRevivalButton.getSize();
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

    window.clear();




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
           /* window.draw(enemies[i]->collider);*/ // Usually don't draw colliders in final game
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
            window.draw(ScoreText);
            window.draw(xpBarHolder);
            window.draw(xpBar);
            window.draw(xpBarText);
        }
    }

    if (gamestate == mainmenu)
    {



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
    if (gamestate == paused)
    {

        gameOverOverlay.setPosition(view.getCenter());
        window.draw(gameOverOverlay);

        Vector2f viewCenter = view.getCenter();

        PauseText.setPosition(viewCenter.x - PauseText.getGlobalBounds().width / 2.f, viewCenter.y - 150.f);
        window.draw(PauseText);


        ContinueButton.setPosition(viewCenter.x - ContinueButton.getLocalBounds().width / 2.f, viewCenter.y - 50.f);
        PauseReturnToMenuButton.setPosition(viewCenter.x - PauseReturnToMenuButton.getLocalBounds().width / 2.f, viewCenter.y + 25.f);


        ContinueText.setPosition(ContinueButton.getPosition().x + (ContinueButton.getSize().x - ContinueText.getLocalBounds().width) / 2.f,
            ContinueButton.getPosition().y + (ContinueButton.getSize().y - ContinueText.getLocalBounds().height) / 2.f - 5.f); // Center text on button, adjust -5.f as needed
        PauseReturnToMenuText.setPosition(PauseReturnToMenuButton.getPosition().x + (PauseReturnToMenuButton.getSize().x - PauseReturnToMenuText.getLocalBounds().width) / 2.f,
            PauseReturnToMenuButton.getPosition().y + (PauseReturnToMenuButton.getSize().y - PauseReturnToMenuText.getLocalBounds().height) / 2.f - 5.f); // Center text on button



        Quote.setPosition(viewCenter.x, viewCenter.y + 120.f);

        window.draw(ContinueButton);
        window.draw(ContinueText);
        window.draw(PauseReturnToMenuButton);
        window.draw(PauseReturnToMenuText);
        window.draw(menuCursor);
        window.draw(Quote);
    }

    if (gamestate == gameloop)
    {
        healthbar.setPosition(view.getCenter().x - 500, view.getCenter().y + 285);
        xpBarDraw();
        window.draw(Map);
        //window.draw(shanoa.collider);
        for (int i = 0; i < swords.size(); i++)
        {
            window.draw(swords[i].shape);
        }

        /*  window.draw(shanoa.attackSpace);*/
        window.draw(shanoa.sprite);
        for (int i = 0; i < Crystals.size(); i++)
        {
            if (Crystals[i].isCollected == false) {
                window.draw(Crystals[i].sprite);
            }
        }
        for (int i = 0;i < enemies.size();i++) {
            window.draw(enemies[i]->healthBarHolder);
            window.draw(enemies[i]->healthBarOfEnemy);
            window.draw(enemies[i]->shape);
        }
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles[i].isActive) {
                window.draw(obstacles[i].sprite);

                //  window.draw(obstacles[i].collider);
            }
        }

        //For Day/Night Cycle
        if (!isday) {
            if (HMactive) {
                DN_overlay.setFillColor(Color(100, 0, 0, 50));
            }
            else {
                DN_overlay.setFillColor(Color(0, 0, 0, 60));
            }
            DN_overlay.setPosition(view.getCenter());
            window.draw(DN_overlay);
        }

        if (showLevelUp) {
            levelupsprite.setOrigin(levelupsprite.getLocalBounds().width / 2.0f, levelupsprite.getLocalBounds().height / 2.0f);
            levelupsprite.setPosition(shanoa.sprite.getPosition().x, shanoa.sprite.getPosition().y - 125);
            window.draw(levelupsprite);
        }


        ScoreText.setPosition(view.getCenter().x - 490, view.getCenter().y - 480);

        // Day/Night Cycle

        DN_timer += deltaTime;
        if (DN_timer >= DN_duration) {
            isday = !isday;
            DN_timer = 0.0f;
        }
        window.draw(ScoreText);
        window.draw(xpBarHolder);
        window.draw(xpBar);
        window.draw(xpBarText);
        window.draw(healthbar);
        if (HMactive) // Horror Mode Indicator
        {
            HMindicator.setPosition(view.getCenter().x, view.getCenter().y - 450);
            window.draw(HMindicator);
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
        window.draw(HMbutton);
        window.draw(HMtext);
        for (int i = 0;i < (volumebarcontroller * 10);i++) {
            window.draw(volumebar[i]);
        }
        Vector2f viewCenter = view.getCenter();
        Text returnText;
        returnText.setFont(defgamefont);
        returnText.setCharacterSize(24);
        returnText.setFillColor(Color::White);
        returnText.setString("Press R to return to Main Menu");
        returnText.setPosition(viewCenter.x - returnText.getGlobalBounds().width / 2.f, viewCenter.y + 300.f);
        window.draw(returnText);

    }

    if (gamestate == gameover)
    {

        gameOverOverlay.setPosition(view.getCenter());
        window.draw(gameOverOverlay);

        Vector2f viewCenter = view.getCenter();
        GameOverText.setPosition(viewCenter.x - GameOverText.getGlobalBounds().width / 2.f, viewCenter.y - 100.f);
        ScoreText.setPosition(viewCenter.x - ScoreText.getGlobalBounds().width / 2.f, viewCenter.y + 10);
        window.draw(GameOverText);
        window.draw(ScoreText);


        if (MathRevivalON)
        {
            SurvivalEquation.sprite.setPosition(viewCenter.x - SurvivalEquation.sprite.getGlobalBounds().width / 2.f, viewCenter.y + 100.f);
            equationAnsCellBox.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f, viewCenter.y + 160.f);
            SurvivalEquation.userAnsText.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f + 5, viewCenter.y + 163.f); // Adjust +5 for padding

            window.draw(SurvivalEquation.sprite);
            window.draw(equationAnsCellBox);
            window.draw(SurvivalEquation.userAnsText);
        }
        else
        {
            // Position the buttons relative to the view center
            // Use getLocalBounds() for the button size itself when centering
            restartButton.setPosition(viewCenter.x - restartButton.getLocalBounds().width / 2.f, viewCenter.y + 90.f);
            MathRevivalButton.setPosition(viewCenter.x - MathRevivalButton.getLocalBounds().width / 2.f, viewCenter.y + 175.f);
            GiveUpButton.setPosition(viewCenter.x - GiveUpButton.getLocalBounds().width / 2.f, viewCenter.y + 260.f); // Position below Math Revival button
            Quote.setPosition(viewCenter.x, viewCenter.y + 350.f);
            // Position the text on top of their corresponding buttons
            // Use getLocalBounds() for text size when centering on the button
            RestartText.setPosition(restartButton.getPosition().x + (restartButton.getSize().x - RestartText.getLocalBounds().width) / 2.f, restartButton.getPosition().y + (restartButton.getSize().y - RestartText.getLocalBounds().height) / 2.f - 5.f); // Center text on button, adjust -5.f as needed
            mathRevivalText.setPosition(MathRevivalButton.getPosition().x + (MathRevivalButton.getSize().x - mathRevivalText.getLocalBounds().width) / 2.f, MathRevivalButton.getPosition().y + (MathRevivalButton.getSize().y - mathRevivalText.getLocalBounds().height) / 2.f - 5.f); // Center text on button
            // Position Give Up Text on its button
            GiveUpText.setPosition(GiveUpButton.getPosition().x + (GiveUpButton.getSize().x - GiveUpText.getLocalBounds().width) / 2.f, GiveUpButton.getPosition().y + (GiveUpButton.getSize().y - GiveUpText.getLocalBounds().height) / 2.f - 5.f); // Center text on button

            MathRevivalLock.setPosition(viewCenter.x - MathRevivalButton.getLocalBounds().width / 2.f + 125, viewCenter.y + 200);

            window.draw(restartButton);
            window.draw(RestartText);
            window.draw(MathRevivalButton);
            if (!shanoa.RevivalScrollAcquired)
            {
                window.draw(MathRevivalLock);
            }
            if (shanoa.RevivalScrollAcquired)
            {
                window.draw(mathRevivalText);
            }
            window.draw(GiveUpButton); // Draw Give Up button
            window.draw(GiveUpText); // Draw Give Up text

            // Draw the cursor when Math Revival is NOT active
            // Cursor position is updated in the Update function
            if (!(selectedGameOverOptionIndex == 1 && !shanoa.RevivalScrollAcquired))
                window.draw(menuCursor);
        }

        window.draw(Quote);

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

    if (gamestate == nameinput) // <-- Add this block
    {
        sf::Vector2f viewCenter = view.getCenter();

        window.draw(creditback);

        nameInputPromptText.setPosition(viewCenter.x - nameInputPromptText.getGlobalBounds().width / 2.f, viewCenter.y - 100.f);
        window.draw(nameInputPromptText);

        equationAnsCellBox.setPosition(viewCenter.x - equationAnsCellBox.getGlobalBounds().width / 2.f, viewCenter.y - 20.f);
        window.draw(equationAnsCellBox);

        // Position and draw the player name text (inside the box)
        // Adjust +5.f padding as needed
        playerNameDisplayText.setPosition(equationAnsCellBox.getPosition().x + 5.f, equationAnsCellBox.getPosition().y + (equationAnsCellBox.getSize().y - playerNameDisplayText.getLocalBounds().height) / 2.f - 5.f);
        window.draw(playerNameDisplayText);

    }

    if (gamestate == leaderboard)
    {

        LeaderboardOverlay.setSize(view.getSize()); // Set size based on the view's size
        // Set its origin to the center, so position(view.getCenter()) works correctly
        LeaderboardOverlay.setOrigin(view.getSize().x / 2.f, view.getSize().y / 2.f);

        LeaderboardOverlay.setFillColor(Color(0, 0, 0, 190));


        window.draw(creditback);
        Vector2f viewCenter = view.getCenter();
        LeaderboardOverlay.setPosition(viewCenter);
        window.draw(LeaderboardOverlay);
        float startY = viewCenter.y - 180.f;
        float lineHeight = 50.f;
        int rank = 1;

        Text leaderboardTitleText;
        leaderboardTitleText.setFont(defgamefont);
        leaderboardTitleText.setCharacterSize(50);
        leaderboardTitleText.setFillColor(Color::Yellow);
        leaderboardTitleText.setString("Leaderboard");
        leaderboardTitleText.setPosition(viewCenter.x - leaderboardTitleText.getGlobalBounds().width / 2.f, viewCenter.y - 250.f);
        window.draw(leaderboardTitleText);


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
            stringstream ss;
            ss << rank << ". "
                << pair.second
                << " - "
                << fixed << setprecision(2) << -pair.first;
            entryText.setString(ss.str());


            entryText.setPosition(viewCenter.x - entryText.getGlobalBounds().width / 2.f, startY + (rank - 1) * lineHeight);
            window.draw(entryText);
            rank++;
        }
        Text returnText;
        returnText.setFont(defgamefont);
        returnText.setCharacterSize(24);
        returnText.setFillColor(Color::White);
        returnText.setString("Press R to return to Main Menu");
        returnText.setPosition(viewCenter.x - returnText.getGlobalBounds().width / 2.f, viewCenter.y + 350.f);
        window.draw(returnText);
    }


    window.display();
}
