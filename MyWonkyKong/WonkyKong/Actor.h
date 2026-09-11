#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// ------------------ Global Constants ---------------------
const int SCORE_KILLS_ENEMY = 100;
const int SCORE_PASS_LEVEL = 1000;
const int SCORE_EXTRALIFEGOODIE = 50;
const int SCORE_GARLICGOODIE = 25;
const int BURPS_GARLICGOODIE = 5;
const int KOOPA_FROZEN_TIME = 50;
const int KOOPA_FROZEN_COOLDOWN = 50;

const int ENEMY_MOVE_COOLDOWN = 10;
const int KONG_FLEE_COOLDOWN = 5;
const int BURP_LIFESPAN = 5;
const int ENEMY_CHANCE_CREATE_GOODIE = 3;
const int KONG_FLEE_DISTANCE = 2;


// --------------- Actor Class -------------------
class Actor : public GraphObject{
public:
    Actor(int IID, int x, int y, StudentWorld* world, int dir = -1); // Actor direction set to -1 (none) if not specificed.
    virtual ~Actor();
    virtual void doSomething() = 0;
    
    // Getter and Setter function
    bool getDead() const;
    void setDead();
    StudentWorld* getWorld() const;
    
    // Helper function to classify actors into diffenent behaviors
    virtual bool passable() const; // All Actors are passable, except floors
    virtual bool standable() const; // All Actors are not standable, except static objects
    virtual bool climbable() const; // All Actors are not climbable, except ladders.
    virtual bool killsEnemy() const; // All Actors cannot kill enemies, except burp
    virtual bool burnBarrel() const; // All Actors cannot burn barrel, except bonfire
    
    bool onPlayer() const; // Check whether the Actor is on the same tile as player
    
private:
    bool m_isDead;
    StudentWorld* m_world;
};

// --------------- Enemy Class -------------------
// Include Koopa, Fireball, and barrel

class Enemy : public Actor{
public:
    Enemy (int IID, int x, int y, StudentWorld* world, int counter, int dir); // Each enemy has a counter a.k.a. how many ticks of doSomething can the enemy move, this is stored in m_counter.
    virtual ~Enemy();
    virtual void doSomething() = 0;
    
    // Getter and Setter
    int getCounter() const;
    void setCounter(int counter);
    
    bool attemptToKillByPlayer (); // Kill the enemy if the enemy is on the same tile as Burp.
    void horizontalMovement(); // Regular horizontal movement by koopa and fireball
private:
    int m_counter; // Every enemy moves in a certain calls of doSomething
};

// --------------- Goodie Class -------------------
class Goodie : public Actor{
public:
    Goodie (int IID, int x, int y, StudentWorld* world); // Goodie has no direction
    virtual ~Goodie();
    virtual void doSomething();
    
    // pure virtual function, each goodie override this to give player different things
    virtual void givePlayerStuff() = 0;
};

// --------------- Static Class -------------------
// Static class is defined to be not-moving objects with no particular directions that can be stand on. e.g. ladders, floors

class Static : public Actor{
public:
    Static(int IID, int x, int y, StudentWorld* world); // Static has no direction
    virtual ~Static();
    virtual void doSomething();
    virtual bool standable() const; // Override: Static is standable
};

// ---------------- Player Class ------------------------
class Player : public Actor{
public:
    Player(int IID, int x, int y, StudentWorld* world); // Player has fixed direction (no need to pass as a parameter)
    virtual ~Player();
    virtual void doSomething();
    
    // Getter and Setter
    int getBurps() const;
    void setBurps(int burps);
    void setFrozen();
private:
    int m_burps;
    int m_frozen_counter; // 50 ticks of frozen
    int m_jump_counter; // 0 means not jumping, 1-4 each mean different phase in jumping
};

// ---------------- Floor Class ---------------------
class Floor : public Static{
public:
    Floor(int IID, int x, int y, StudentWorld* world);
    virtual ~Floor();
    
    virtual bool passable() const; // Overrides: Floor is NOT passable
};

// ---------------- Ladder Class ---------------------
class Ladder : public Static{
public:
    Ladder(int IID, int x, int y, StudentWorld* world);
    virtual ~Ladder();
    
    virtual bool climbable() const; // Overrides: Ladder is climbable
};
// ---------------- Burp Class ---------------------
class Burp : public Actor{
public:
    Burp(int IID, int x, int y, StudentWorld* world, int dir);
    virtual ~Burp();
    virtual void doSomething();
    
    virtual bool killsEnemy() const;
private:
    int m_lifespan; // Each burp lives 5 ticks
};
 

// ---------------- Bonfire Class ---------------------
class Bonfire : public Actor{
public:
    Bonfire(int IID, int x, int y, StudentWorld* world);
    virtual ~Bonfire();
    virtual void doSomething();
    
    virtual bool burnBarrel() const; // Override: Bonfire burns barrel
    
};

// ---------------- Extra Life Goodie Class ---------------------
class Extra_Life_Goodie : public Goodie{
public:
    Extra_Life_Goodie(int IID, int x, int y, StudentWorld* world);
    virtual ~Extra_Life_Goodie();
    virtual void givePlayerStuff();
};

// ---------------- Garlic Goodie Class ---------------------
class Garlic_Goodie : public Goodie{
public:
    Garlic_Goodie(int IID, int x, int y, StudentWorld* world);
    virtual ~Garlic_Goodie();
    virtual void givePlayerStuff();
};

// ---------------- Fireball Class ---------------------
class Fireball : public Enemy{
public:
    Fireball(int IID, int x, int y, StudentWorld* world, int dir);
    virtual ~Fireball();
    void doSomething();
private:
    int m_climbingState; // Climbing state is none if not climbing, up/down (constant) when climbing up and down
};

// ---------------- Koopa Class ---------------------
class Koopa : public Enemy{
public:
    Koopa(int IID, int x, int y, StudentWorld* world, int dir);
    virtual ~Koopa();
    virtual void doSomething();
private:
    int m_cooldown; // Cooldown to freeze the player, set to 50 ticks initially
};

// ---------------- Barrel Class ---------------------
class Barrel : public Enemy{
public:
    Barrel(int IID, int x, int y, StudentWorld* world, int dir);
    virtual ~Barrel();
    virtual void doSomething();
private:
    bool m_falling; // find out if the barrel is falling or not
};

// ----------------- Kong Class ---------------------
class Kong : public Actor{
public:
    Kong(int IID, int x, int y, StudentWorld* world, int dir);
    virtual ~Kong();
    virtual void doSomething();
private:
    bool m_isFleeing; // Condition whether the kong is fleeing
    int m_barrelCounter; // Counter to throw a barrel
    int m_fleeingCounter; // Counter to flee one tile up
    int determineBarrelCounter() const; // private helper function in determining what is the interval to throw barrel depending on the level
};

#endif // ACTOR_H_
