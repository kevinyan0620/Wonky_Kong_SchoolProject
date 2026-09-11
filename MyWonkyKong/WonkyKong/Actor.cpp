#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include "GameConstants.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// --------------- Actor Class -------------------
// Constructor: Derive from GraphObject
Actor::Actor(int IID, int x, int y, StudentWorld* world, int dir)
    : GraphObject (IID, x, y, dir)
{
    m_isDead = false;
    m_world = world;
}

// Destructor
Actor::~Actor()
{
}

// Getter
bool Actor::getDead() const{
    return m_isDead;
}

// Setter: setDead automatically set actor to dead, evey actor is initially not died
void Actor::setDead(){
    m_isDead = true;
}

// Getter of the world actor is in
StudentWorld* Actor::getWorld() const{
    return m_world;
}

// Actor is defaultly to be set as non standable
bool Actor::standable() const{
    return false;
}

// Actor is defaultly to be set as passable
bool Actor::passable() const{
    return true;
}

// Actor is defaultly to be set as non climbable
bool Actor::climbable() const{
    return false;
}

// Actor is defaultly to be set as cannot burn barrel
bool Actor::burnBarrel() const{
    return false;
}

// Actor is defaultly to be set as cannot kill enemy
bool Actor::killsEnemy() const{
    return false;
}

// Check whether this actor is on the same tile as player
bool Actor::onPlayer() const {
    int currentX = getX();
    int currentY = getY();
    
    if (getWorld() -> getPlayerX() == currentX && getWorld() -> getPlayerY() == currentY){
        return true;
    }
    return false;
}

// ------------------ Enemy Class --------------------
// Constructor: Derive from Actor
Enemy::Enemy (int IID, int x, int y, StudentWorld* world, int counter, int dir)
    : Actor(IID, x, y, world, dir)
{
    m_counter = counter;
}

// Destructor
Enemy::~Enemy()
{
}

// Getter
int Enemy::getCounter() const{
    return m_counter;
}

// Setter
void Enemy::setCounter(int counter){
    m_counter = counter;
}

// Check whether the enemy is on the same block as the burp by player.
bool Enemy::attemptToKillByPlayer() {
    int currentX = getX();
    int currentY = getY();
    if (getWorld() -> isKillsEnemy(currentX, currentY)){ // If the block enemy is on have something that can kill enemy (from player) eg burp
        setDead();
        getWorld() -> increaseScore (SCORE_KILLS_ENEMY);
        getWorld() -> playSound(SOUND_ENEMY_DIE);
        return true;
    }
    return false;
}

// The horizontal movement is used by live, moveable enemy creatures (koopa and fireball), in which they will reverse direction at a cliff or wall.
void Enemy::horizontalMovement(){
    int currentX = getX();
    int currentY = getY();
    
    // If direction is left
    if (getDirection() == left){
        // If the tile to the left and down can be stand on and the tile to the left can be pass through, it can move to the left position
        if (getWorld() -> isStandable(currentX - 1, currentY - 1) && getWorld() -> isPassable(currentX - 1, currentY)){
            moveTo(currentX - 1, currentY);
        }
        
        // If cannot move left, reverse direction
        else{
            setDirection(right);
        }
    }
    else if (getDirection() == right){
        // If the tile to the right and down can be stand on and the tile to the right can be pass through, it can move to the right position
        if (getWorld() -> isStandable(currentX + 1, currentY - 1) && getWorld() -> isPassable(currentX + 1, currentY)){
            moveTo(currentX + 1, currentY);
        }
        
        // If cannot move right, reverse direction
        else{
            setDirection(left);
        }
    }
}

// ------------------ Goodie Class --------------------
// Constructor: Derived from Actor
Goodie::Goodie(int IID, int x, int y, StudentWorld* world)
    : Actor(IID, x, y, world)
{
}

// Destructor
Goodie::~Goodie()
{
}

void Goodie::doSomething(){
    // If collected by player (dead), do nothing and immediately return
    if (getDead()){
        return;
    }
    
    // If still alive and on the same tile as player, set to dead, give player stuff, and play sound.
    else{
        if (onPlayer()){
            setDead();
            givePlayerStuff();
            getWorld() -> playSound(SOUND_GOT_GOODIE);
        }
    }
}

// ---------------- Static Class ---------------------
// Constructor: Derived from actor
Static::Static(int IID, int x, int y, StudentWorld* world)
    : Actor(IID, x, y, world)
{
}

// Destructor
Static::~Static()
{
}

// doSomethinf does nothing
void Static::doSomething()
{
}

// Override: static (floor and ladder) is standable (for player, koopa, and fireball at least)
bool Static::standable() const{
    return true;
}
// ---------------- Player Class ------------------------
// Constructor
Player::Player(int IID, int x, int y, StudentWorld* world)
    : Actor(IID, x, y, world, right) // Player always faces right when created
{
    // All data members is set to 0 when a player is created.
    m_burps = 0;
    m_frozen_counter = 0;
    m_jump_counter = 0;
}

// Destructor
Player::~Player()
{
}

void Player::doSomething()
{
    // If died, do nothing and immediately return
    if (getDead()){
        return;
    }
    
    int currentX = getX();
    int currentY = getY();
    
    // If jump counter is not 0, means the player is still in a jump sequence
    if (m_jump_counter != 0){
        int currentDir = getDirection();
        
        // When meeting a ladder when jumping, stop jumping and make the graphic look like "climbed on the ladder"
        if (getWorld() -> isClimbable(currentX, currentY)){
            m_jump_counter = 0;
            return;
        }
        
        // Check in which step is the jump now.
        switch (m_jump_counter)
        {
            // Get down step: Last step
            case 1:
                if (getWorld() -> isPassable(currentX, currentY - 1)){
                    moveTo(currentX, currentY - 1);
                }
                // Else do nothing since it's already the last move of jump
                break;
            // for the middle 3 step, just move in the direction of the player
            case 2:
            case 3:
            case 4:
                // Check whether tile before the player is passable, if not, terminate the jump sequence
                if (currentDir == left){
                    if (getWorld() -> isPassable(currentX - 1, currentY)){
                        moveTo(currentX - 1, currentY);
                    }
                    else{
                        m_jump_counter = 0; // terminate jump sequence
                        return;
                    }
                }
                else if (currentDir == right){
                    if (getWorld() -> isPassable(currentX + 1, currentY)){
                        moveTo(currentX + 1, currentY);
                    }
                    else{
                        m_jump_counter = 0; // terminate jump sequence
                        return;
                    }
                }
                break;
        }
        
        // Dec the jump counter each time a tick is performed
        m_jump_counter --;
        return;
    }
    
    // If the player is frozen, dec the counter and do nothing, return immediately
    if (m_frozen_counter != 0){
        m_frozen_counter --;
        return;
    }
    
    // Simulate falling, if the block the player is on cannot be climbed or the tile below cannot be stand, fall one tile down
    if (!getWorld() -> isClimbable(currentX, currentY) && !getWorld() -> isStandable (currentX, currentY - 1)){
        moveTo(currentX, currentY - 1);
        return;
    }
    
    int keyValue;
    
    // User input
    if (getWorld() -> getKey(keyValue)){
        switch (keyValue){
            case KEY_PRESS_UP:
                // Check if the place can be climbed and if there are nothing blocking the player, climb up
                if (getWorld() -> isClimbable (currentX, currentY) && getWorld() -> isPassable (currentX, currentY + 1)){
                    moveTo (currentX, currentY + 1);
                }
                break;
            case KEY_PRESS_DOWN:
                // If the player is on a ladder and the tile below can be passed, go down
                if (getWorld() -> isClimbable (currentX, currentY) && getWorld() -> isPassable (currentX, currentY - 1)){
                    moveTo (currentX, currentY - 1);
                }
                // When at the top of the ladder, able to go down.
                else if (getWorld() -> isClimbable(currentX, currentY - 1)){
                    moveTo (currentX, currentY - 1);
                }
                break;
            case KEY_PRESS_LEFT:
                // If currently facing right, switch direction
                if (getDirection() == right){
                    setDirection(left);
                }
                
                // If facing the correct orientation, check whether the tile before player is passable, if yes, go
                else{
                    if (getWorld() -> isPassable(currentX - 1, currentY)){
                        moveTo(currentX - 1, currentY);
                    }
                }
                break;
            case KEY_PRESS_RIGHT:
                // If currently facing left, switch direction
                if (getDirection() == left){
                    setDirection(right);
                }
                
                // If facing the correct orientation, check whether the tile before player is passable, if yes, go
                else{
                    if (getWorld() -> isPassable(currentX + 1, currentY)){
                        moveTo(currentX + 1, currentY);
                    }
                }
                break;
            case KEY_PRESS_SPACE:
                // When space is pressed, play the sound
                getWorld() -> playSound(SOUND_JUMP);
                
                // If there are nothing above the player, blocking, init the jump and do the first move (go up).
                if (getWorld() -> isPassable (currentX, currentY + 1)){
                    moveTo(currentX, currentY + 1);
                    m_jump_counter = 4;
                }
                break;
            case KEY_PRESS_TAB:
                // If tab is pressed, check whether the player have remaining burp left
                if (m_burps != 0){
                    // Play sound
                    getWorld() -> playSound(SOUND_BURP);
                    
                    // Find the location that the burp should be placed at
                    int burpX;
                    int burpY;
                    getPositionInThisDirection(getDirection(), 1, burpX, burpY);
                    
                    // Let the world create a burp at correct location and the direction same to the player, and dec the burp count
                    getWorld() -> createBurp(burpX, burpY, getDirection());
                    m_burps --;
                    break;
                }
        }
    }
}

// Set player frozen for 50 ticks
void Player::setFrozen()
{
    m_frozen_counter = KOOPA_FROZEN_TIME;
}

// Getter of player's burp
int Player::getBurps() const{
    return m_burps;
}

// Sets player's burp
void Player::setBurps(int burps){
    m_burps = burps;
}

// ---------------- Floor Class ---------------------
// Constructor: derived from static
Floor::Floor(int IID, int x, int y, StudentWorld* world)
    : Static(IID, x, y, world)
{
}

// Destructor
Floor::~Floor()
{
}

// Override: Floor is not passable
bool Floor::passable() const{
    return false;
}


// ---------------- Ladder Class ---------------------
// Constructor: derived from static
Ladder::Ladder(int IID, int x, int y, StudentWorld* world)
    : Static(IID, x, y, world)
{
}

// Destructor
Ladder::~Ladder(){
    
}

// Override: ladder is climbable
bool Ladder::climbable() const{
    return true;
}

// ------------------ Extra Life Goodie -------------------
// Constructor: Derived from Goodie
Extra_Life_Goodie::Extra_Life_Goodie(int IID, int x, int y, StudentWorld* world)
    : Goodie(IID, x, y, world)
{
}

// Destructor
Extra_Life_Goodie::~Extra_Life_Goodie()
{
}

// Override: what stuff does Extra Life Goodie give player
void Extra_Life_Goodie::givePlayerStuff() {
    getWorld() -> increaseScore(SCORE_EXTRALIFEGOODIE);
    getWorld() -> incLives();
}

// ------------------- Garlic Goodie ---------------------
// Constructor: Derived from Goodie
Garlic_Goodie::Garlic_Goodie(int IID, int x, int y, StudentWorld* world)
    : Goodie(IID, x, y, world)
{
}

// Destructor
Garlic_Goodie::~Garlic_Goodie()
{
}

// Override: what stuff does Garlic Goodie give player
void Garlic_Goodie::givePlayerStuff() {
    getWorld() -> increaseScore(SCORE_GARLICGOODIE);
    getWorld() -> incBurp(BURPS_GARLICGOODIE);
}

// -------------------- Bonfire ---------------------
// Constructor: Derived from Actor
Bonfire::Bonfire(int IID, int x, int y, StudentWorld* world)
    : Actor(IID, x, y, world)
{
}

// Destructor
Bonfire::~Bonfire()
{
}

void Bonfire::doSomething(){
    // Motionalize the bonfire
    increaseAnimationNumber();

    // if on player, kills player
    if (onPlayer()){
        getWorld() -> setPlayerDead();
    }
}

// Override: bonfire can burn barrel
bool Bonfire::burnBarrel() const {
    return true;
}

// --------------------- Kong ----------------------
// Constructor: Derived from actor
Kong::Kong (int IID, int x, int y, StudentWorld* world, int dir)
    : Actor(IID, x, y, world, dir)
{
    // initiallt set to not fleeing
    m_isFleeing = false;
    
    // Flee once per 5 call to doSomething
    m_fleeingCounter = KONG_FLEE_COOLDOWN;
    
    // Correctly initialize the barrelCounter using the private helper function
    m_barrelCounter = determineBarrelCounter();
}

// Destructor
Kong::~Kong()
{
}

void Kong::doSomething(){
    // If dead, do nothing (which is not possible to kill Kong in this phase of the game)
    if (getDead()){
        return;
    }
    
    // Mave Kong look lively
    increaseAnimationNumber();
    
    int kongX = getX();
    int kongY = getY();
    int playerX = getWorld() -> getPlayerX();
    int playerY = getWorld() -> getPlayerY();
    
    // Using cmath's pow function, calculate the distance between player and kong using pythagorean theorem
    double dist = pow (pow(kongX - playerX, 2) + pow(kongY - playerY, 2), 0.5);
    
    // KONG_FLEE_DISTANCE is the max distance between Kong and player that Kong has to flee
    if (dist <= KONG_FLEE_DISTANCE){
        m_isFleeing = true;
    }
    
    // Not fleeing:
    if (!m_isFleeing){
        // If throw barrel is in cooldown, dec the counter
        if (m_barrelCounter != 0){
            m_barrelCounter --;
        }
        // If not in cooldown, throw a barrel with the correct position calculayed by the method getPositionInThisDirection
        else{
            int barrelX;
            int barrelY;
            
            getPositionInThisDirection(getDirection(), 1, barrelX, barrelY);
            
            // Ask the world to create a barrel at the certain location, same direction as kong
            getWorld() -> createBarrel(barrelX, barrelY, getDirection());
            
            // Set the counter according to the level
            m_barrelCounter = determineBarrelCounter();
        }
    }
    
    // If fleeing
    if (m_isFleeing){
        // Decrement fleeing counter
        m_fleeingCounter --;
        
        // When fleeing counter reaches 0
        if (m_fleeingCounter == 0){
            // If still in the map, move one step up
            if (getY() < VIEW_HEIGHT){
                moveTo(getX(), getY() + 1);
            }
            // If no longer in the map, award the player and tell the world that the player won
            else{
                getWorld() -> increaseScore(SCORE_PASS_LEVEL);
                getWorld() -> playSound(SOUND_FINISHED_LEVEL);
                getWorld() -> setWon();
            }
            
            // "Restock" the fleeing counter
            m_fleeingCounter = KONG_FLEE_COOLDOWN;
        }
    }
}

// Private helper method for Kong class to determine the interval to throw barrel
int Kong::determineBarrelCounter() const{
    int level = getWorld() -> getLevel();
    // Given in the spec, 0 - 3 level follow the formula 200 - 50 * level, above level stick to 50
    if (level <= 3){
        return 200 - 50 * level;
    }
    else{
        return 50;
    }
}
// ----------------------- Barrel ------------------------
// Constructor: Derived from enemy
Barrel::Barrel(int IID, int x, int y, StudentWorld* world, int dir)
    : Enemy(IID, x, y, world, ENEMY_MOVE_COOLDOWN, dir) // Counter for barrel is ENEMY_MOVE_COOLDOWN ticks / move
{
    // Barrel is initially not falling
    m_falling = false;
}

// Destructor
Barrel::~Barrel()
{
}

void Barrel::doSomething(){
    // If counter not 0 yet, dec the counter
    if (getCounter() > 0)
        setCounter(getCounter() - 1);
    
    // If dead, do nothing and return
    if (getDead()){
        return;
    }
    
    int currentX = getX();
    int currentY = getY();
    int currentDir = getDirection();
    
    // If on the same tile as player, tell the world to kill the player
    if (onPlayer()){
        getWorld() -> setPlayerDead();
        return;
    }
    
    // If one the same tile as barrel burner (bonfire), kill itself
    if (getWorld() -> isBurnBarrel(currentX, currentY)){
        setDead();
        return;
    }
    
    // Attemp to be killed by burp: See if there is burp on the same tile, if yes, kill itslef
    attemptToKillByPlayer();
    
    // Check if it's falling: If is passable under, set the falling status and move down
    if (getWorld()-> isPassable (currentX, currentY - 1)){
        m_falling = true;
        moveTo(currentX, currentY - 1);
    }
    
    // If cannot pass through the space under it
    else{
        // If previous falling, reverse its direction and set falling status to be false
        if (m_falling){
            setDirection(getDirection() + 180);
            m_falling = false;
        }
        
        // If previous not falling
        else{
            // If it's time for barrel to move (10 ticks has passed)
            if (getCounter() == 0){
                
                // If direction is left, check whether it can move to left, if yes, move, if no, reverse direction
                if (currentDir == left){
                    if (!getWorld() -> isPassable(currentX - 1, currentY)){
                        setDirection(getDirection() + 180);
                    }
                    else{
                        moveTo (currentX - 1, currentY);
                    }
                }
                // If direction is right, check whether it can move to right, if yes, move, if no, reverse direction
                else{
                    if (!getWorld() -> isPassable(currentX + 1, currentY)){
                        setDirection(getDirection() + 180);
                    }
                    else{
                        moveTo (currentX + 1, currentY);
                    }
                }
                // "Restock" the counter
                setCounter (ENEMY_MOVE_COOLDOWN);
                
                // Check one more time if it's on the same tile with player, kill the player if yes
                if (onPlayer()) {
                    getWorld() -> setPlayerDead();
                    return;
                }
            }
        }
    }
}

// ------------------------------ Burp --------------------------------
// Constructor: Derived from actor
Burp::Burp(int IID, int x, int y, StudentWorld* world, int dir)
    : Actor(IID, x, y, world, dir)
{
    // Each burp has a livespan of 5 ticks
    m_lifespan = BURP_LIFESPAN;
}

// Destructor
Burp::~Burp()
{
}

void Burp::doSomething(){
    // If dead, do nothing and immediately return
    if (getDead()){
        return;
    }
    
    // If livespan is 0, means the burp is died (should be disappeared)
    if (m_lifespan == 0){
        setDead();
        return;
    }
    
    // decrement livespan
    m_lifespan --;
}

// Override: Burp kills Enemy
bool Burp::killsEnemy() const{
    return true;
}


// ----------------------------- Koopa ---------------------------
// Constructor: Derived from enemy
Koopa::Koopa (int IID, int x, int y, StudentWorld* world, int dir)
    : Enemy(IID, x, y, world, ENEMY_MOVE_COOLDOWN, dir) // Counter for Koopa is ENEMY_MOVE_COOLDOWN ticks / move
{
    // Koopa has a freeze cooldown, originally set to 0
    m_cooldown = 0;
}

// Destructor
Koopa::~Koopa()
{
    
}

void Koopa::doSomething(){
    // If Counter is positive, decrement the counter
    if (getCounter() > 0)
        setCounter(getCounter() - 1);
    
    // If died, immediately return
    if (getDead()){
        return;
    }
    
    int currentX = getX();
    int currentY = getY();
    
    // See if the burp is on the same tile as it. If yes, create an extra life goodie (1/3 chance) and the place and kill itself
    if (attemptToKillByPlayer()){
        getWorld() -> createExtraLifeGoodie(currentX, currentY, ENEMY_CHANCE_CREATE_GOODIE); // The third argument means one in num chance that it will create the goodie
        return;
    }
    
    // If on the same tile as player and it doesn't have freezer cooldown, freeze the player
    if (onPlayer() && m_cooldown == 0){
        getWorld() -> setPlayerFrozen();
        m_cooldown = KOOPA_FROZEN_COOLDOWN;
        return;
    }
    
    // If freezing ability still in cooldown, dec the cooldown
    if (m_cooldown > 0){
        m_cooldown --;
    }
    
    // If counter = 0 (10 ticks has passed), do a horizontal movement (left and right movement that change direction when facing cliff and wall), and "restock" the counter
    if (getCounter() == 0){
        horizontalMovement();
        setCounter(ENEMY_MOVE_COOLDOWN);
        
        // Check one more time if the koopa meets the player. If yes, freeze the player
        if (onPlayer() && m_cooldown == 0){
            getWorld() -> setPlayerFrozen();
            m_cooldown = KOOPA_FROZEN_COOLDOWN;
            return;
        }
    }
}

// ------------------------ Fireball -----------------------
// Constructor: Derived from enemy
Fireball::Fireball(int IID, int x, int y, StudentWorld* world, int dir)
    : Enemy(IID, x, y, world, ENEMY_MOVE_COOLDOWN, dir) // Counter for Fireball is ENEMY_MOVE_COOLDOWN ticks / move
{
    // Initially not in climbing state
    m_climbingState = none;
}

// Destructor
Fireball::~Fireball()
{
    
}

void Fireball::doSomething(){
    // If counter is positive, decrement the counter
    if (getCounter() > 0)
        setCounter (getCounter() - 1);
    
    // if dead, do nothing and immediately return
    if (getDead()){
        return;
    }
    
    // If at the same tile as player, tell the world to kill player
    if (onPlayer()){
        getWorld() -> setPlayerDead();
        return;
    }
    
    int currentX = getX();
    int currentY = getY();
    
    // See if it's at the same tile as player. If yes, kill itself
    if (attemptToKillByPlayer()){
        // Create Gralic goodie 1/3 chance
        getWorld() -> createGarlicGoodie(currentX, currentY, ENEMY_CHANCE_CREATE_GOODIE); // The third argument means 1 in how many chance
        return;
    }
    
    // If 10 ticks has passed
    if (getCounter() == 0){
        // In climbing up state: if the tile above can be climbed to, continue climbing
        if (m_climbingState == up){
            if (getWorld() -> isClimbable (currentX, currentY) && getWorld() -> isPassable (currentX, currentY + 1)){
                moveTo (currentX, currentY + 1);
            }
            // If cannot climb to, exit the climbing state and instead horizontal move once
            else{
                m_climbingState = none;
                horizontalMovement();
            }
        }
        // In climbing down state: if the tile below can be climbed to, continue climbing
        else if (m_climbingState == down){
            if (getWorld() -> isClimbable (currentX, currentY) && getWorld() -> isClimbable (currentX, currentY - 1)){
                moveTo (currentX, currentY - 1);
            }
            // If cannot climb to, exit the climbing state and instead horizontal move once
            else{
                m_climbingState = none;
                horizontalMovement();
            }
        }
        // If not in a climbing state
        else{
            // Random generate a number
            int randomNum = randInt(1,3);
            
            // If the place can be climbed up
            if (getWorld() -> isClimbable (currentX, currentY) && getWorld() -> isPassable (currentX, currentY + 1)){
                // random number picked 1 out of 1-3 (1/3 chance), move up
                if (randomNum == 1){
                    m_climbingState = up;
                    moveTo (currentX, currentY + 1);
                }
                // The other 2/3 chance? continue horizontal movement
                else{
                    horizontalMovement();
                }
            }
            // If the place can be climbed down
            else if (getWorld() -> isClimbable(currentX, currentY - 1)){
                // random number picked 1 out of 1-3 (1/3 chance), move down
                if (randomNum == 1){
                    m_climbingState = down;
                    moveTo (currentX, currentY - 1);
                }
                // The other 2/3 chance? continue horizontal movement
                else{
                    horizontalMovement();
                }
            }
            // Cannot move up or down? Continue horizontal movement
            else{
                horizontalMovement();
            }
        }
        
        // Restock the counter
        setCounter(ENEMY_MOVE_COOLDOWN);
        
        // Check again if on the same tile as player. If yes, tell the world to kill the player
        if (onPlayer()){
            getWorld() -> setPlayerDead();
            return;
        }
    }
}
