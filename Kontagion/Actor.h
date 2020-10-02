#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// contains base, Socrates, and dirt class declarations as well as constants required by these classes

class StudentWorld; 

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* myStudentWorld);
    virtual void doSomething() = 0;
    virtual ~Actor() {};
    
    bool isDead() const;
    void setDead();
    int getHealth() const;
    void setHealth(int x);
    void addHealth(int x);
    
    virtual bool isDamageableObject() const;
    virtual bool canBlock() const;
    virtual bool canConsumeGoodie() const;
    virtual bool canBeConsumed() const;
    virtual bool canDamageSocrates() const;
    
    StudentWorld* getStudentWorld() const; // gets pointer to studentworld
    
    double calculateX(Direction d) const ; // for moving socrates five degrees
    double calculateY(Direction d) const; // for moving socrates five degrees
    double getDistance(double x1, double y1, double x2, double y2) const; // gets distance between two points
        
    
private:
    StudentWorld* myStudentWorld;
    bool amDead;
    Direction curr;
    int s_health;
};

class Socrates : public Actor {
public:
    Socrates(StudentWorld* myStudentWorld);
    virtual ~Socrates() {};
    
    int getSpray() const;
    int getFlameThrower() const;
    void addFlameThrower(int x);
    virtual void doSomething();
    
    virtual bool canConsumeGoodie() const;
            
private:
    int sprayCharges;
    int flameThrower;
};

class Dirt : public Actor {
public:
    Dirt(double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~Dirt() {};
    virtual void doSomething();
    void attacked();
    
    virtual bool canBlock() const;
};

class Projectile : public Actor {
public:
    Projectile(int imageID, double startX, double startY, Direction dir, double mTD, StudentWorld* myStudentWorld);
    virtual ~Projectile() {};
    virtual void doSomething() = 0;
    double getMaxTravelDist() const;
    void setMaxTravelDist(double m);
    virtual bool isDamageableObject() const;
    virtual bool canConsumeGoodie() const;
    void moveProjectile();
    
private:
    double maxTDist;
};

class Flame : public Projectile {
public:
    Flame(double startX, double startY, Direction dir, StudentWorld* myStudentWorld);
    virtual ~Flame(){};
    virtual void doSomething();
};

class Spray : public Projectile {
public:
    Spray(double startX, double startY, Direction dir, StudentWorld* myStudentWorld);
    virtual ~Spray(){};
    virtual void doSomething();
};

class Food : public Actor {
public:
    Food(double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~Food() {};
    virtual void doSomething();
    
    virtual bool isDamageableObject() const;
    virtual bool canBeConsumed() const;
};

class Goodie : public Actor {
public:
    Goodie(int imageID, double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~Goodie() {};
    virtual void doSomething();
    
    int getHealth();
    void decHealth();
    
private:
    int lifeTime;
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(double startX, double startY, StudentWorld *myStudentWorld);
    virtual ~RestoreHealthGoodie() {};
    virtual void doSomething();
};

class FlameThrowerGoodie : public Goodie {
public:
    FlameThrowerGoodie(double startX, double startY, StudentWorld *myStudentWorld);
    virtual ~FlameThrowerGoodie() {};
    virtual void doSomething();
};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(double startX, double startY, StudentWorld *myStudentWorld);
    virtual ~ExtraLifeGoodie() {};
    virtual void doSomething();

};

class Fungus : public Goodie {
public:
    Fungus(double startX, double startY, StudentWorld *myStudentWorld);
    virtual ~Fungus() {};
    virtual void doSomething();
    virtual bool canDamageSocrates() const;
};

class Pit: public Actor {
public:
    Pit(double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~Pit() {};
    
    virtual void doSomething();
    int getRegSalmon() const;
    int getAggSalmon() const;
    int getEColi() const;
    
    void decRegSalmon();
    void decAggSalmon();
    void decEColi();
    
    int getEnemies() const;
    void decEnemies();
    
    void checkIfEmpty();
    void setPitEmpty();
    bool isPitEmpty() const;

private:
    int regS;
    int aggS;
    int eColi;
    bool isEmpty;
};

class Bacteria: public Actor {
public:
    Bacteria(int imageID, double startX, double startY, int h, StudentWorld* myStudentWorld);
    virtual ~Bacteria() {};
    virtual void doSomething();

    int getDistToMove() const;
    int getFoodEaten() const;
    void eatFood();
    void resetFoodEaten();
    void resetDistToMove(int x);
    void decDistToMove();
    bool canMove(Actor* self, int distance, double& x, double& y);
    virtual bool canDamageSocrates() const;
    void calculateLocation(double& x, double& y);
 
private:
    int distToMove;
    int foodEaten;
};

class Salmonella : public Bacteria {
public:
    Salmonella(int imageID, double startX, double startY, int h, StudentWorld* myStudentWorld);
    virtual ~Salmonella() {};
    virtual void doSomething();
    void move();
};


class RegularSalmonella : public Salmonella {
public:
    RegularSalmonella(double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~RegularSalmonella() {};
    
    virtual void doSomething();
};

class AggressiveSalmonella : public Salmonella {
public:
    AggressiveSalmonella(double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~AggressiveSalmonella() {};
    
    virtual void doSomething();
    
};

class EColi : public Bacteria {
public:
    EColi(double startX, double startY, StudentWorld* myStudentWorld);
    virtual ~EColi() {};
    
    virtual void doSomething();
};

#endif // ACTOR_H_
