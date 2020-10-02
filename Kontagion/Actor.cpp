#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
// implementation of classes in Actor.h

// ACTOR
Actor::Actor(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* m) : GraphObject(imageID, startX, startY, dir, depth, 1.0) {
    myStudentWorld = m;
    amDead = false;
    curr = dir;
}

bool Actor::isDead() const {
    return amDead;
}

void Actor::setDead() {
    amDead = true;
}

StudentWorld* Actor::getStudentWorld() const {
    return myStudentWorld;
}

int Actor::getHealth() const {
    return s_health;
}

void Actor::setHealth(int x) {
    s_health = x;
}

void Actor::addHealth(int x) {
    s_health += x;
}

double Actor::getDistance(double x1, double y1, double x2, double y2) const {
    double xDistance = abs(x2-x1);
    double yDistance = abs(y2-y1);
    double dist = sqrt(pow(xDistance, 2)+pow(yDistance, 2));
    return dist;
}

double Actor::calculateX(Direction d) const {
    const double PI = 4 * atan(1);
    return ((VIEW_WIDTH/2.0)*cos((d+180)*PI/180.0)+VIEW_WIDTH/2);
}
double Actor::calculateY(Direction d) const {
    const double PI = 4 * atan(1);
    return ((VIEW_HEIGHT/2.0)*sin((d+180)*PI/180.0)+VIEW_HEIGHT/2);
}

bool Actor::isDamageableObject() const { // most actors are damageable
    return true;
}

bool Actor::canBlock() const { // most actors cannot block
    return false;
}

bool Actor::canConsumeGoodie() const { // most actors cannot consume goodies
    return false;
}

bool Actor::canBeConsumed() const { // most actors cannot be consumed
    return false;
}

bool Actor::canDamageSocrates() const { // most actors cannot damage Socrates
    return false;
}

// SOCRATES
Socrates::Socrates(StudentWorld* myStudentWorld) : Actor(IID_PLAYER, 0, VIEW_RADIUS, 0, 0, myStudentWorld) {
    setHealth(100); // health points
    sprayCharges = 20; // charges of disinfectant
    flameThrower = 5; // flame-thrower charges
}
    
void Socrates::doSomething() {
    if (isDead()) {
        return;
    }
    if (getHealth() <= 0) { // socrates is dead
        getStudentWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
        return;
    }
    int c;
    double x;
    double y;
    if (getStudentWorld()->getKey(c)) {
        switch(c) {
            case KEY_PRESS_LEFT:
            case 'a':
                x = calculateX(getDirection()+5);
                y = calculateY(getDirection()+5);
                moveTo(x, y);
                setDirection(getDirection()+5);
                break;
            case KEY_PRESS_RIGHT:
            case 'd':
                x = calculateX(getDirection()-5);
                y = calculateY(getDirection()-5);
                moveTo(x, y);
                setDirection(getDirection()-5);
                break;
            case KEY_PRESS_SPACE:
                if (getSpray() > 0) {
                    getStudentWorld()->playSound(SOUND_PLAYER_SPRAY);
                    getStudentWorld()->spray(this);
                    sprayCharges--;
                }
                break;
            case KEY_PRESS_ENTER:
                if (flameThrower > 0) {
                    getStudentWorld()->playSound(SOUND_PLAYER_FIRE);
                    getStudentWorld()->flame(this);
                    flameThrower--;
                }
                break;
        }
    } else { // player did NOT press key
        if (sprayCharges < 20) {
            sprayCharges++;
        }
    }
    if (getHealth() <= 0) { // check again if socrates is dead
        getStudentWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
        return;
    }
    if (getStudentWorld()->overlapsSocrates(this)) {
        getStudentWorld()->playSound(SOUND_PLAYER_HURT);
    }
}

int Socrates::getSpray() const {
    return sprayCharges;
}

int Socrates::getFlameThrower() const {
    return flameThrower;
}

void Socrates::addFlameThrower(int x) {
    flameThrower += x;
}

bool Socrates::canConsumeGoodie() const {
    return true;
}

// DIRT
Dirt::Dirt(double startX, double startY, StudentWorld* myStudentWorld) : Actor(IID_DIRT, startX, startY, 0, 1, myStudentWorld) {}

void Dirt::doSomething() {
    if (isDead()) {
        return;
    }
    if(getStudentWorld()->overlap(this)) {
        setDead();
        return;
    }
}

void Dirt::attacked() {
    setDead();
}

bool Dirt::canBlock() const {
    return true;
}

// PROJECTILE
Projectile::Projectile(int imageID, double startX, double startY, Direction dir, double mTD, StudentWorld* myStudentWorld) : Actor(imageID, startX, startY, dir, 1, myStudentWorld) {
    maxTDist = mTD;
}
 
double Projectile::getMaxTravelDist() const {
    return maxTDist;
}

void Projectile::setMaxTravelDist(double m) {
    maxTDist = m;
}

bool Projectile::isDamageableObject() const {
    return false;
}

bool Projectile::canConsumeGoodie() const {
    return true;
}

void Projectile::moveProjectile() {
    moveForward(SPRITE_WIDTH);
    double distLeft = getMaxTravelDist() - SPRITE_WIDTH;
    setMaxTravelDist(distLeft);
    if (distLeft <= 0) {
        setDead();
        return;
    }
}

// FLAME
Flame::Flame(double startX, double startY, Direction dir, StudentWorld* myStudentWorld) : Projectile(IID_FLAME, startX, startY, dir, 32, myStudentWorld) {}

void Flame::doSomething() {
    if (isDead()){
        return;
    }
    if (getStudentWorld()->doesFlameAttack(this)) {
        setDead();
        return;
    }
    Projectile::moveProjectile();
}

// SPRAY
Spray::Spray(double startX, double startY, Direction dir, StudentWorld* myStudentWorld) : Projectile(IID_SPRAY, startX, startY, dir, 112, myStudentWorld) {}

void Spray::doSomething() {
    if (isDead()) {
        return;
    }
    if(getStudentWorld()->doesSprayAttack(this)) {
        setDead();
        return;
    }
    Projectile::moveProjectile();
}

// FOOD
Food::Food(double startX, double startY, StudentWorld* myStudentWorld) : Actor(IID_FOOD, startX, startY, 90, 1, myStudentWorld){}

void Food::doSomething() {
    if(isDead()) {
        return;
    }
}

bool Food::isDamageableObject() const {
    return false;
}

bool Food::canBeConsumed() const {
    return true;
}

// GOODIE
Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* myStudentWorld) : Actor(imageID, startX, startY, 0, 1, myStudentWorld) {
    lifeTime = std::max(rand() % (300 - 10 * myStudentWorld->getLevel()), 50);
}

int Goodie::getHealth() {
    return lifeTime;
}

void Goodie::decHealth() {
    lifeTime--;
}

void Goodie::doSomething() {
    if (isDead()) {
        return;
    }
    // if overlap with flame, or spray, it goes away
    if (getStudentWorld()->overlap(this)) {
        setDead();
        return;
    }
    decHealth();
    if(getHealth() <= 0) {
        setDead();
    }
}

// RestoreHealthGoodie
RestoreHealthGoodie::RestoreHealthGoodie(double startX, double startY, StudentWorld *myStudentWorld) : Goodie(IID_RESTORE_HEALTH_GOODIE, startX, startY, myStudentWorld) {}

void RestoreHealthGoodie::doSomething() {
    Goodie::doSomething();
    if (getStudentWorld()->attacksSocrates(this)) {
        getStudentWorld()->increaseScore(250);
        setDead();
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        getStudentWorld()->getMySocrates()->setHealth(100);
        return;
    }
}

FlameThrowerGoodie::FlameThrowerGoodie(double startX, double startY, StudentWorld *myStudentWorld) : Goodie (IID_FLAME_THROWER_GOODIE, startX, startY, myStudentWorld) {}

void FlameThrowerGoodie::doSomething() {
    Goodie::doSomething();
    if (getStudentWorld()->attacksSocrates(this)) {
        getStudentWorld()->increaseScore(300);
        setDead();
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        getStudentWorld()->getMySocrates()->addFlameThrower(5);
        return;
    }
}

// ExtraLifeGoodie
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld *myStudentWorld) : Goodie (IID_EXTRA_LIFE_GOODIE, startX, startY, myStudentWorld) {}

void ExtraLifeGoodie::doSomething() {
    Goodie::doSomething();
    if (getStudentWorld()->attacksSocrates(this)) {
        getStudentWorld()->increaseScore(500);
        setDead();
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        getStudentWorld()->incLives();
        return;
    }
}

// Fungus
Fungus::Fungus(double startX, double startY, StudentWorld *myStudentWorld) : Goodie (IID_FUNGUS, startX, startY, myStudentWorld) {}

void Fungus::doSomething() {
    Goodie::doSomething();
    if (getStudentWorld()->attacksSocrates(this)) { 
        getStudentWorld()->increaseScore(-50);
        setDead();
        getStudentWorld()->getMySocrates()->addHealth(-20);
        return;
    }
}
    
bool Fungus::canDamageSocrates() const {
    return true;
}

// Pit
Pit::Pit(double startX, double startY, StudentWorld* myStudentWorld) : Actor(IID_PIT, startX, startY, 0, 1, myStudentWorld) {
    regS = 5;
    aggS = 3;
    eColi = 2;
    isEmpty = false; // initially not empty
}

int Pit::getRegSalmon() const { // returns number of regularSalmon in pit's inventory
    return regS;
}

int Pit::getAggSalmon() const { // returns number of aggressiveSalmon in pit's inventory
    return aggS;
}

int Pit::getEColi() const { // returns number of current eColi in pit's inventory
    return eColi;
}

void Pit::decRegSalmon() { // decreases number of regularSalmon in pit's inventory
    regS--;
}

void Pit::decAggSalmon() { // decreases number of aggressiveSalmon in pit's inventory
    aggS--;
}

void Pit::decEColi() { // decreases number of eColi in pit's inventory
    eColi--;
}

void Pit::setPitEmpty() { // sets pit to be empty
    isEmpty = true;
}

bool Pit::isPitEmpty() const { // checks if pit is empty
    return isEmpty;
}

void Pit::checkIfEmpty() { // checks if current pit is empty and sets it as empty if so
    if (getRegSalmon() == 0 && getAggSalmon() == 0 && getEColi() == 0) {
        setPitEmpty();
    }
}

void Pit::doSomething() { // releases bacteria if pit is not empty
    checkIfEmpty();
    if (isPitEmpty()) {
        getStudentWorld()->decNumOfPits();
        setDead();
        return;
    }
    if (!isPitEmpty()) {
        int x = randInt(1, 50);
        if (x == 1) { // 1 in 50 chance
            getStudentWorld()->addBacteria(this); // randomly choose bacteria to emit based on pit's inventory
        }
    }
}

// Bacteria
Bacteria::Bacteria(int imageID, double startX, double startY, int h, StudentWorld* myStudentWorld) : Actor (imageID, startX, startY, 90, 0, myStudentWorld) {
    setHealth(h);
    distToMove = 0;
    foodEaten = 0;
}

// put the similar implementations here!!
void Bacteria::doSomething() {
    if (isDead()) {
        return;
    }
    // check if attacked by spray/flame
    if (getStudentWorld()->overlap(this)) {
        if(!isDead()) {
            getStudentWorld()->playSound(SOUND_SALMONELLA_HURT);
        }
        if (getHealth() <= 0) {
            getStudentWorld()->decEnemies();
            setDead();
            getStudentWorld()->playSound(SOUND_SALMONELLA_DIE);
            getStudentWorld()->increaseScore(100);
            int x = randInt(1, 2);
            if (x == 1) { // 50% chance turns into food
                getStudentWorld()->addFood(getX(), getY());
            }
        }
    }
}

int Bacteria::getDistToMove() const { // gets distToMove
    return distToMove;
}

void Bacteria::decDistToMove() { // decreases distToMove
    distToMove--;
}

int Bacteria::getFoodEaten() const { // gets number of food currently eaten
    return foodEaten;
}

void Bacteria::eatFood() { // increases number of food eaten
    foodEaten++;
}

void Bacteria::resetFoodEaten() { // resets foodEaten back to 0
    foodEaten = 0;
}

void Bacteria::resetDistToMove(int x) { // resets the distToMove
    distToMove = x;
}
    
bool Bacteria::canDamageSocrates() const { // bacteria can damage socrates
    return true;
}

// checks if bacteria can move to newX and newY location. returns true if can, and false if blocked by something
bool Bacteria::canMove(Actor* self, int dist, double& x, double& y) { // self is the bacteria itself
    getPositionInThisDirection(self->getDirection(), dist, x, y);
    if (getStudentWorld()->movementOverlap(this, x, y)) {
        return false;
    } else if (getDistance(VIEW_WIDTH/2, VIEW_HEIGHT/2, x, y) >= VIEW_RADIUS) {
        return false;
    }
    return true;
}

// calculates new location for bacteria to be added if bacteria has eaten 3 foods
void Bacteria::calculateLocation(double& x, double& y) {
    double newX = getX();
    if (getX() < VIEW_WIDTH/2) {
        newX += SPRITE_WIDTH/2;
    } else if (getX() > VIEW_WIDTH/2) {
        newX -= SPRITE_WIDTH/2;
    } else {
        newX = getX();
    }
    double newY = getY();
    if (getY() < VIEW_HEIGHT/2) {
        newY += SPRITE_WIDTH/2;
    } else if (getX() > VIEW_HEIGHT/2) {
        newY -= SPRITE_WIDTH/2;
    } else {
        newY = getY();
    }
    x = newX;
    y = newY;
}

// Salmonella
Salmonella::Salmonella(int imageID, double startX, double startY, int h, StudentWorld* myStudentWorld) : Bacteria(imageID, startX, startY, h, myStudentWorld) {}

void Salmonella::doSomething() {
    if (isDead()) {
        return;
    }
    Bacteria::doSomething();
}
void Salmonella::move() {
    if (getDistToMove() > 0) {
        decDistToMove();
        double x = getX();
        double y = getY();
        if (canMove(this, 3, x, y)) {
            moveTo(x, y);
        } else {
            int randDirection = randInt(0, 359);
            setDirection(randDirection);
            resetDistToMove(10);
        }
        return;
    } else {
        int angle;
        if (getStudentWorld()->getAngleToNearestEdible(this, 128, angle)) { // food within 128 pixels from regular salmonella
            setDirection(angle);
            double dx, dy;
            getPositionInThisDirection(angle, 3, dx, dy);
            if (!canMove(this, 3, dx, dy)) {
                int angle = randInt(0, 359);
                setDirection(angle);
                resetDistToMove(10);
            } else {
                moveTo(dx, dy);
            }
            return;
        } else { // if no food found
            int angle = randInt(0, 359);
            setDirection(angle);
            resetDistToMove(10);
        }
    }
}

// RegularSalmonella
RegularSalmonella::RegularSalmonella(double startX, double startY, StudentWorld* myStudentWorld) : Salmonella(IID_SALMONELLA, startX, startY, 4, myStudentWorld) {}

void RegularSalmonella::doSomething() {
    Salmonella::doSomething();
    // step 2
    if (getStudentWorld()->attacksSocrates(this)) {
        getStudentWorld()->getMySocrates()->addHealth(-1);
    } else if (getFoodEaten() == 3) { // step 3
        double newX, newY;
        Bacteria::calculateLocation(newX, newY);
        getStudentWorld()->addRegularSalmonella(newX, newY);
        resetFoodEaten();
    } else if (getStudentWorld()->overlapsFood(this)) { // step 4 : check if bacteria overlaps food (
        eatFood();
    }
    // step 5 - 6
    Salmonella::move();
}

// AggressiveSalmonella
AggressiveSalmonella::AggressiveSalmonella(double startX, double startY, StudentWorld* myStudentWorld) : Salmonella(IID_SALMONELLA, startX, startY, 10, myStudentWorld) {}

void AggressiveSalmonella::doSomething() {
    Salmonella::doSomething();
    bool closeToSocrates = false;
    int a;
    if (getStudentWorld()->getAngleToNearbySocrates(this, 72, a)) {
        double dx, dy;
        getPositionInThisDirection(a, 3, dx, dy);
        setDirection(a);
        if (canMove(this, 3, dx, dy)) {
            moveTo(dx, dy);
        }
        closeToSocrates = true;
    }
    // steps 3-5
    if (getStudentWorld()->attacksSocrates(this)) { // overlaps with Socrates
        getStudentWorld()->getMySocrates()->addHealth(-2);
    } else if (getFoodEaten() == 3) { // step 4
        double newX, newY;
        Bacteria::calculateLocation(newX, newY);
        getStudentWorld()->addAggressiveSalmonella(newX, newY);
        resetFoodEaten();
    } else if (getStudentWorld()->overlapsFood(this)) { // step 5
        eatFood();
    }
    if (closeToSocrates) { // double check that step 6 and beyond is not done if closeToSocrates = true
        return;
    }
        
    // step 6 - 7
    Salmonella::move();
}

// EColi 
EColi::EColi(double startX, double startY, StudentWorld* myStudentWorld) : Bacteria(IID_ECOLI, startX, startY, 5, myStudentWorld){}

void EColi::doSomething() {
    Bacteria::doSomething();
    // step 2 - 4
    if (getStudentWorld()->attacksSocrates(this)) {
        getStudentWorld()->getMySocrates()->addHealth(-4);
    } else if (getFoodEaten() == 3) {
        double newX, newY;
        Bacteria::calculateLocation(newX, newY);
        getStudentWorld()->addEColi(newX, newY);
        resetFoodEaten();
    } else if (getStudentWorld()->overlapsFood(this)) { // check if bacteria overlaps food
        eatFood();
    }
    // step 5
    int angle;
    if (getStudentWorld()->getAngleToNearbySocrates(this, 256, angle)) {
        setDirection(angle);
        int i = 0;
        while (i < 10) {
            double myX = getX();
            double myY = getY();
            if (canMove(this, 2, myX, myY)) {
                moveAngle(angle, 2);
                return;
            } else {
                i++;
                setDirection(angle + 10);
            }
        }
    }
}
