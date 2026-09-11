#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

// Constructor: Derived from GameWorld
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    // player pointer is initially set to nullptr and levelWon to false
    m_player = nullptr;
    m_levelWon = false;
}

int StudentWorld::init()
{
    // Read the level
    Level l(assetPath());
    int int_level = getLevel();
    string curLevel = "levelxx.txt";
    
    // get the correct string ".txt" file
    if (int_level != 100){ // Lavel 99 is max
        // Find first char by integer divide 10
        char first_char = '0' + (int_level / 10);
        // Find second char by mod 10
        char second_char = '0' + (int_level % 10);
        curLevel[5] = first_char;
        curLevel[6] = second_char;
    }
    else{
        // level in 100, means the player won
        return GWSTATUS_PLAYER_WON;
    }
    
    // Load the txt fire and according to the file status, return corresponding message
    Level::LoadResult result = l.loadLevel(curLevel);
    if (result == Level::load_fail_bad_format){
        return GWSTATUS_LEVEL_ERROR;
    }
    
    if (result == Level::load_fail_file_not_found){ // ?What if there is no level 00?
        return GWSTATUS_PLAYER_WON;
    }
    
    // Read each char in the file and push the actor into the actor list
    for (int x = 0; x < VIEW_WIDTH; x++){
        for (int y = 0; y < VIEW_WIDTH; y++){
            Level::MazeEntry item = l.getContentsOf(x, y);
            switch (item){
                case Level::floor:
                    m_actors.push_back(new Floor(IID_FLOOR, x, y, this));
                    break;
                case Level::ladder:
                    m_actors.push_back(new Ladder(IID_LADDER, x, y, this));
                    break;
                case Level::left_kong:
                    m_actors.push_back(new Kong(IID_KONG, x, y, this, 180));
                    break;
                case Level::right_kong:
                    m_actors.push_back(new Kong(IID_KONG, x, y, this, 0));
                    break;
                case Level::fireball: {
                    // Since fireball has no particular direction, random pick one fire left (180) or right (0)
                    int dir = randInt(0, 1);
                    dir *= 180;
                    m_actors.push_back(new Fireball(IID_FIREBALL, x, y, this, dir));
                    break;
                }
                case Level::koopa: {
                    // Since fireball has no particular direction, random pick one fire left (180) or right (0)
                    int dir = randInt(0, 1);
                    dir *= 180;
                    m_actors.push_back(new Koopa(IID_KOOPA, x, y, this, dir));
                    break;
                }
                case Level::bonfire:
                    m_actors.push_back(new Bonfire(IID_BONFIRE, x, y, this));
                    break;
                case Level::extra_life:
                    m_actors.push_back(new Extra_Life_Goodie(IID_EXTRA_LIFE_GOODIE, x, y, this));
                    break;
                case Level::garlic:
                    m_actors.push_back(new Garlic_Goodie(IID_GARLIC_GOODIE, x, y, this));
                    break;
                case Level::player:
                    // For player, don't store in the actor list. Instead, let the m_player pointer point to it.
                    m_player = new Player(IID_PLAYER, x, y, this);
                    break;
                case Level::empty:
                    break;
            }
        }
    }
    
    // File read done: continue game
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Get the information to be displayed on the screen
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int burps = m_player -> getBurps();
    
    // using output stream, display the message on the screen with correct format
    ostringstream oss;
    oss.fill('0');
    oss << "Score: ";
    oss << setw(7) << score;
    oss << "  Level: ";
    oss << setw(2) << level;
    oss << "  Lives: ";
    oss << setw(2) << lives;
    oss << "  Burps: ";
    oss << setw(2) << burps;
    string gameText = oss.str();
    setGameStatText(gameText);
    
    // Traverse the actor list and give every actor a chance to do something
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++){
        (*i) -> doSomething();
    }
    // Give the player a chance to do something
    m_player -> doSomething();
    
    // Traverse the actor list and delete dead actor
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++){
        if ((*i) -> getDead()){
            delete *i;
            m_actors.erase(i);
            i--;
        }
    }
    
    // If player dead, dec player live, play sound and return the correct status
    if (m_player -> getDead()){
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    
    // If levelWon, set the levelWon back to false and return the correct status
    if (m_levelWon){
        m_levelWon = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // If nothing important happens, continue the game
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // Traverse all actors in the list and delete all actors
    vector<Actor*>::iterator i = m_actors.begin();
    while (i != m_actors.end()){
        delete *i;
        m_actors.erase(i);
    }
    
    // Delete player
    delete m_player;
    
    m_player = nullptr; // Able to check whether m_player is already deleted.
}

// For a give coord, check in the actor list if all actors is passable. If at least one of them at that tile is not passable, return false. If all are passable, return true.
bool StudentWorld::isPassable (int x, int y) const{
    for (vector<Actor*>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++){
        if ((*i) -> getX() == x && (*i) -> getY() == y && !(*i) -> passable()){
            return false;
        }
    }
    
    // This is account for the ladder on the ceiling, in which we don't want player to move out of the map
    if (y >= VIEW_HEIGHT){
        return false;
    }
    return true;
}

// For a given coord, check in the actor list if at least one actor can be stand on. If yes, return true, if no, return false
bool StudentWorld::isStandable (int x, int y) const{
    for (vector<Actor*>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++){
        if ((*i) -> getX() == x && (*i) -> getY() == y && (*i) -> standable()){
            return true;
        }
    }
    return false;
}

// For a given coord, check in the actor list if at least one actor can be climb on. If yes, return true, if no, return false
bool StudentWorld::isClimbable (int x, int y) const{
    for (vector<Actor*>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++){
        if ((*i) -> getX() == x && (*i) -> getY() == y && (*i) -> climbable()){
            return true;
        }
    }
    return false;
}

// For a given coord, check in the actor list if at least one actor can kills the enemy. If yes, return true, if no, return false
bool StudentWorld::isKillsEnemy (int x, int y) const{
    for (vector<Actor*>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++){
        if ((*i) -> getX() == x && (*i) -> getY() == y && (*i) -> killsEnemy()){
            return true;
        }
    }
    return false;
}

// For a given coord, check in the actor list if at least one actor can burns barrels. If yes, return true, if no, return false
bool StudentWorld::isBurnBarrel (int x, int y) const{
    for (vector<Actor*>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++){
        if ((*i) -> getX() == x && (*i) -> getY() == y && (*i) -> burnBarrel()){
            return true;
        }
    }
    return false;
}

// Set the player to be died
void StudentWorld::setPlayerDead(){
    m_player -> setDead();
}

// Set the player to be frozen
void StudentWorld::setPlayerFrozen(){
    m_player -> setFrozen();
}

// Get player location
int StudentWorld::getPlayerX() const{
    return m_player -> getX();
}

int StudentWorld::getPlayerY() const{
    return m_player -> getY();
}

// Increase player's burp
void StudentWorld::incBurp (int howMuch){
    m_player -> setBurps (m_player -> getBurps() + howMuch);
}

// Set the level to be be won to indicate the studentworld to return correct status
void StudentWorld::setWon(){
    m_levelWon = true;
}

// Tell the world to create a barrel at specific location and direction
void StudentWorld::createBarrel(int x, int y, int dir){
    m_actors.push_back(new Barrel(IID_BARREL, x, y, this, dir));
}

// Tell the world to create a burp at specific location and direction
void StudentWorld::createBurp(int x, int y, int dir){
    m_actors.push_back(new Burp(IID_BURP, x, y, this, dir));
}

// Tell the world to create a extra life goodie at specific location and direction with a chance of "oneOutOfHowMany"
void StudentWorld::createExtraLifeGoodie(int x, int y, int oneOutOfHowMany){
    int randomNum = randInt(1, oneOutOfHowMany);
    
    if (randomNum == 1)
        m_actors.push_back(new Extra_Life_Goodie(IID_EXTRA_LIFE_GOODIE, x, y, this));
}

// Tell the world to create a garlic goodie at specific location and direction with a chance of "oneOutOfHowMany"
void StudentWorld::createGarlicGoodie(int x, int y, int oneOutOfHowMany){
    int randomNum = randInt(1, oneOutOfHowMany);
    
    if (randomNum == 1)
        m_actors.push_back(new Garlic_Goodie(IID_GARLIC_GOODIE, x, y, this));
}

// Destructor
StudentWorld::~StudentWorld(){
    // If the system doesn't call cleanUp for us, call cleanUp ourself
    if (m_player != nullptr){
        cleanUp();
    }
}
