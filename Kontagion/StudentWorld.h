#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

#include "Actor.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
// contains StudentWorld class declaration
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Socrates* getMySocrates();
    void decNumOfPits(); // decreases number of pits in world when one goes away
    void decEnemies(); // decreases current number of enemies when one dies
    
    void spray(Actor *self);
    void flame(Actor *self);
    void addBacteria(Pit* self);
    void addRegularSalmonella(double x, double y);
    void addAggressiveSalmonella(double x, double y);
    void addFood(double x, double y);
    void addEColi(double x, double y);
        
    bool movementOverlap(Actor* self, double targetX, double targetY);
    bool overlap(Actor* m); 
    bool overlapInitial(Actor* m); // for initializing. checks all objects
    bool overlapsSocrates(Actor* self);
    bool overlapsFood(Actor* self);
    bool attacksSocrates(Actor* bacteria);
    
    bool doesSprayAttack(Actor* m); // checks if spray attacks
    bool doesFlameAttack(Actor* m); // checks if flame attacks
    
    bool getAngleToNearestEdible(Actor* self, int dist, int& angle);
    bool getAngleToNearbySocrates(Actor* self, int dist, int& angle);
        
    ~StudentWorld(); // calls cleanUp()

private:
    void displayGameText();
    bool allPitsGone() const; // returns true if all pits gone
    void setPitsGone();
    int getCurrPits() const;
    void deleteDeadGameObjects();
    void addEnemies();
    void addGoodie();

    Socrates* mySocrates;
    std::vector<Actor*> myActors;
    
    int myEnemies;
    bool pitGone;
    int currPits;
};

#endif // STUDENTWORLD_H_
