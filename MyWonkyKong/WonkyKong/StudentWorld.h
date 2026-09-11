#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Helper function that tell what a given coord consists
    bool isPassable(int x, int y) const; // If the coord is passable (doesn't contain unpassable actor)
    bool isStandable(int x, int y) const; // If the coord is standable (contain standable actor)
    bool isClimbable(int x, int y) const; // If the coord is climbable (contain climbable actor)
    bool isBurnBarrel(int x, int y) const; // If the coord can burn barrel (contain barrel burner actor)
    bool isKillsEnemy (int x, int y) const; // If the coord can kill enemy (contain enemy killer)
    
    void setPlayerDead(); // Set the player to be dead
    void setPlayerFrozen(); // Set the player to be frozen
    
    // Get Player coord
    int getPlayerX() const;
    int getPlayerY() const;
    void incBurp(int howMuch); // Inc player's burp bu howmuch
    void setWon(); // The player won the level
    
    void createBarrel(int x, int y, int dir); // Create a barrel at (x,y) with direction dir
    void createExtraLifeGoodie (int x, int y, int oneOutOfHowMany); // create Extra Life Goodie with a chance of "oneOutOfHowMany"
    void createGarlicGoodie (int x, int y, int oneOutOfHowMany); // create Garlic Goodie with a chance of "oneOutOfHowMany"
    void createBurp (int x, int y, int dir); // Create a burp at (x,y) with direction dir

private:
    std::vector<Actor*> m_actors; // STL Vector to store all actors beside player
    Player* m_player; // Player pointer to address player
    bool m_levelWon; // data member to temp store whether the player won the certain level
};

#endif // STUDENTWORLD_H_
