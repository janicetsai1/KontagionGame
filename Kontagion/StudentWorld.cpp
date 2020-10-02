// contains StudentWorld class implementation

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath) {
    myEnemies = 0; // initially start off with 0 enemies
    pitGone = false; // initially false
    currPits = getLevel(); // initially 0 pits in studentWorld
}

void StudentWorld::setPitsGone() {
    pitGone = true; // all pits in world set as gone
}

bool StudentWorld::allPitsGone() const { // returns true if all pits gone
    return pitGone;
}

Socrates* StudentWorld::getMySocrates() {
    return mySocrates;
}

void StudentWorld::addEnemies() {
    myEnemies++;
}

void StudentWorld::decEnemies() {
    myEnemies--;
}

// might not need!!
int StudentWorld::getCurrPits() const {
    return currPits;
}

void StudentWorld::decNumOfPits() {
    currPits--;
}

bool StudentWorld::attacksSocrates(Actor* bacteria) { // can also check if goodie overlaps with socrates
    double dist = bacteria->getDistance(bacteria->getX(), bacteria->getY(), mySocrates->getX(), mySocrates->getY());
    if (dist <= SPRITE_WIDTH) {
        if (bacteria->canDamageSocrates()) {
            playSound(SOUND_PLAYER_HURT);
        }
        return true;
    }
    return false;
}

bool StudentWorld::doesSprayAttack(Actor *m) { // checks if spray hits something damageable, and if it can damage socrates, decrease health by 2
    for (int i = 0; i < myActors.size(); i++) {
        double dist = m->getDistance(m->getX(), m->getY(), myActors[i]->getX(), myActors[i]->getY());
        if (dist <= SPRITE_WIDTH) {
            // found dirt here
            if (myActors[i]->canBlock()) {
                return true;
            }
            if (myActors[i]->canDamageSocrates()) { // if fungus or baceteria
                myActors[i]->addHealth(-2);
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::doesFlameAttack(Actor *m) { // checks if flame hits something damageable, and if it can damage socrates, decrease health by 5
    for (int i = 0; i < myActors.size(); i++) {
        double dist = m->getDistance(m->getX(), m->getY(), myActors[i]->getX(), myActors[i]->getY());
        if (dist <= SPRITE_WIDTH) {
            // found dirt here
            if (myActors[i]->canBlock()) {
                return true;
            }
            if (myActors[i]->canDamageSocrates()) { // if fungus or baceteria
                myActors[i]->addHealth(-5);
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::overlapInitial(Actor *m) {
    for (int i = 0; i < myActors.size(); i++) {
        double dist = m->getDistance(m->getX(), m->getY(), myActors[i]->getX(), myActors[i]->getY());
        if (dist <= SPRITE_WIDTH) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::overlap(Actor *m) {
    for (int i = 0; i < myActors.size(); i++) {
        double dist = m->getDistance(m->getX(), m->getY(), myActors[i]->getX(), myActors[i]->getY());
        if (myActors[i] != m) {
            if (dist <= SPRITE_WIDTH) {
                if(!myActors[i]->isDamageableObject()) {
                    return true;
                }
                if (m->canBlock() || myActors[i]->canBlock()) { // no overlap so just continue
                    continue;
                }
            }
        }
    }
    return false; // no overlap after going through entire for loop (i.e. iterating through all actors
}

bool StudentWorld::movementOverlap(Actor* self, double targetX, double targetY) {
    for (int i = 0; i < myActors.size(); i++) {
        double dist = self->getDistance(targetX, targetY, myActors[i]->getX(), myActors[i]->getY());
         if (myActors[i]->canBlock() && myActors[i] != self) {
             if (dist <= SPRITE_WIDTH/2) {
                 return true; // there's no overlap because it can block
             }
         }
     }
     return false; // no overlap
}

bool StudentWorld::overlapsSocrates(Actor* self) {
    for (int i = 0; i < myActors.size(); i++) {
        double dist = self->getDistance(self->getX(), self->getY(), myActors[i]->getX(), myActors[i]->getY());
        if (dist <= SPRITE_WIDTH) {
            if (myActors[i]->canDamageSocrates()) { // if fungus or bacteria
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::overlapsFood(Actor *self) {
    for (int i = 0; i < myActors.size(); i++) {
        if (!myActors[i]->isDamageableObject()) { 
            if (self->getDistance(myActors[i]->getX(), myActors[i]->getY(), self->getX(), self->getY()) <= SPRITE_WIDTH) {
                if (myActors[i]->canBeConsumed()) { // if a food, set it as dead here
                    myActors[i]->setDead();
                    return true;
                }
            }
        }
    }
    return false;
}

bool StudentWorld::getAngleToNearestEdible(Actor* self, int dist, int& angle) {
    const double PI = 4 * atan(1);
    int currMin = VIEW_HEIGHT; // initialize curr min dist to this
    for (int i = 0; i < myActors.size(); i++) {
        if (myActors[i]->canBeConsumed()) {
            int currDist = getMySocrates()->getDistance(myActors[i]->getX(), myActors[i]->getY(), self->getX(), self->getY());
            if (currDist <= dist && currDist < currMin) { // DOUBLE CHECK
                currMin = currDist;
                double dx = myActors[i]->getX() - self->getX();
                double dy = myActors[i]->getY() - self->getY();
                angle = atan2(dy, dx)*(180.0/PI);
            }
        }
    }
    if (currMin == VIEW_HEIGHT) { // means that no nearest edible within dist was found
        return false;
    }
    return true;
}

bool StudentWorld::getAngleToNearbySocrates(Actor* self, int dist, int& angle) { // finds if socrates is within dist and returns true; updates angle to be the angle an object must be in to get to socrates
    const double PI = 4 * atan(1);
    int currDist = getMySocrates()->getDistance(self->getX(), self->getY(), getMySocrates()->getX(), getMySocrates()->getY());
    if (currDist <= dist) {
        double dx = getMySocrates()->getX() - self->getX();
        double dy = getMySocrates()->getY() - self->getY();
        angle = atan2(dy, dx)*(180.0/PI);
        return true;
    }
    return false;
}

void StudentWorld::deleteDeadGameObjects() { // iterates through vector of objects
    if (mySocrates == nullptr && mySocrates->isDead()) {
        delete mySocrates;
    }
    vector<Actor*>::iterator it = myActors.begin();
    for (; it != myActors.end(); ) {
        if((*it)!= nullptr && (*it)->isDead()) {
            delete (*it);
            it = myActors.erase(it);
        } else {
            it++;
        }
    }
}

int StudentWorld::init() { // initializes the student world with socrates, dirt, food, and pit objects
    int currentLevel = getLevel();
    if (getLives() <= 0) {
        return GWSTATUS_PLAYER_DIED;
    }
    mySocrates = new Socrates(this); // add Socrates
    
    // add pit objects
    for (int i = 0; i < currentLevel; i++) {
        int x = randInt(0, 255);
        int y = randInt(0, 255);
        Pit* p;
        double distFromCenter = p->getDistance(VIEW_RADIUS, VIEW_RADIUS, x, y);
        if (distFromCenter > 120) {
            i--;
            continue;
        } else {
            p = new Pit(x, y, this);
            myActors.push_back(p);
        }
    }
    // add food objects
    int fl = min(5*currentLevel, 25);
    for (int i = 0; i < fl; i++) {
        int x = randInt(0, 255);
        int y = randInt(0, 255);
        Food *f = new Food(x, y, this);
        double dist = f->getDistance(VIEW_RADIUS, VIEW_RADIUS, x, y);
        if (dist > 120 || overlapInitial(f)) {
            delete f;
            i--;
            continue;
        } else {
            myActors.push_back(f);
        }
        
    }
    
    // add dirt objects
    int n = max(180 - 20*currentLevel, 20);
    for (int i = 0; i < n; i++) {
        int x = randInt(0, 255);
        int y = randInt(0, 255);
        Dirt* d = new Dirt(x, y, this);
        double dist = d->getDistance(VIEW_RADIUS, VIEW_RADIUS, x, y);
        if (dist > 120 || overlapInitial(d)) {
            delete d;
            i--;
            continue;
        } else {
            myActors.push_back(d);
        }
    }
    displayGameText();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    displayGameText();
    mySocrates->doSomething();
    if (mySocrates->isDead()) { // check if socrates died
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (getCurrPits() == 0 && myEnemies == 0) { // no pits or enemies left, so level completed
        currPits = getLevel() + 1;
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }

    for (int i = 0; i < myActors.size(); i++) {
        if (!myActors[i]->isDead()) {
            myActors[i]->doSomething();
            if (mySocrates->isDead()) {
                return GWSTATUS_PLAYER_DIED;
            }
        }
    }
    deleteDeadGameObjects(); // deletes dead game objects
    addGoodie(); // add new actors
    if(mySocrates->isDead()) {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        if(getLives() == 0) {
            return GWSTATUS_PLAYER_DIED;
        }
    }
    displayGameText();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() { // deletes pointers to free dynamicically allocated memory
    delete mySocrates;
    mySocrates = nullptr;
    vector<Actor*>::iterator it = myActors.begin();
    for(; it != myActors.end(); ) {
        Actor* del = *it;
        delete del;
        it = myActors.erase(it);
    }
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::displayGameText() { // displays the game text at top of screen
    int currentScore = getScore();
    int currentLevel = getLevel();
    int currentLives = getLives();
    int currentHealth = mySocrates->getHealth();
    int currentSprays = mySocrates->getSpray();
    int currentFlames = mySocrates->getFlameThrower();
    
    ostringstream oss;
    oss << "Score: ";
    if (currentScore >= 0) {
        oss.fill('0');
        oss << setw(6) << currentScore << "  ";
    } else { // negative score
        currentScore *= -1; // currentScore is negative so change it to be positive so that it's displayed correctly
        oss.fill('0');
        oss << "-" << setw(5) << currentScore << "  ";
    }
   
    oss << "Level: ";
    oss << setw(2) << currentLevel << "  ";
    oss << "Lives: ";
    oss.fill(' ');
    oss << setw(2) << currentLives << "  ";
    oss << "Health: ";
    oss << setw(3) << currentHealth << "  ";
    oss << "Sprays: ";
    oss << setw(3) << currentSprays << "  ";
    oss << "Flames: ";
    oss << setw(3) << currentFlames << "  ";
    
    string s = oss.str();
    setGameStatText(s);
}

void StudentWorld::spray(Actor *self) { // adds a spray and initializes it to have socrates' direction, x coordinate, and coordinate
    Spray* s = new Spray(self->getX(), self->getY(), self->getDirection(), this);
    myActors.push_back(s);
}

void StudentWorld::flame(Actor* self) { // adds 16 flames around socrates, setting them in the correct direction
    Flame *f;
    Direction dir = self->getDirection(); // starting direction
    for (int i = 0; i < 16; i++) {
        f = new Flame(self->getX(), self->getY(), dir, this);
        dir += 22;
        myActors.push_back(f);
    }
}

void StudentWorld::addGoodie() { // randomly adds a goodie
    int chanceFungus = min(510 - getLevel() * 10, 250);
    int a = randInt(0, chanceFungus);
    if (a == 0) {
        double x = randInt(0, VIEW_WIDTH);
        double y = randInt(0, VIEW_HEIGHT);
        double xDistance = abs(x- (VIEW_WIDTH/2));
        double yDistance = abs(y-(VIEW_HEIGHT/2));
        double dist = sqrt(pow(xDistance, 2)+pow(yDistance, 2));
        while (dist != VIEW_RADIUS) {
            x = randInt(0, VIEW_WIDTH);
            y = randInt(0, VIEW_HEIGHT);
            xDistance = abs(x- (VIEW_WIDTH/2));
            yDistance = abs(y-(VIEW_HEIGHT/2));
            dist = sqrt(pow(xDistance, 2)+pow(yDistance, 2));
        }
        Fungus* f = new Fungus(x, y, this);
        myActors.push_back(f);
    }
    
    int chanceGoodie = min(510 - getLevel() * 10, 250);
    int x = randInt(0, chanceGoodie);
    if (x == 0) {
        double x = randInt(0, VIEW_WIDTH);
        double y = randInt(0, VIEW_HEIGHT);
        double xDistance = abs(x- (VIEW_WIDTH/2));
        double yDistance = abs(y-(VIEW_HEIGHT/2));
        double dist = sqrt(pow(xDistance, 2)+pow(yDistance, 2));
        while (dist != VIEW_RADIUS) {
            x = randInt(0, VIEW_WIDTH);
            y = randInt(0, VIEW_HEIGHT);
            xDistance = abs(x- (VIEW_WIDTH/2));
            yDistance = abs(y-(VIEW_HEIGHT/2));
            dist = sqrt(pow(xDistance, 2)+pow(yDistance, 2));
        }
        int goodie = randInt(1, 10);
        if (goodie < 7) {
            RestoreHealthGoodie* r = new RestoreHealthGoodie(x, y, this);
            myActors.push_back(r);
        } else if (goodie >= 7 && goodie < 10) {
            FlameThrowerGoodie* f = new FlameThrowerGoodie(x, y, this);
            myActors.push_back(f);
        } else {
            ExtraLifeGoodie* g = new ExtraLifeGoodie(x, y, this);
            myActors.push_back(g);
        }
    }
}

void StudentWorld::addBacteria(Pit *self) { // adds a bacteria. Randomly chooses which one if there are bacteria left to add
    // 1 means regular Salmon, 2 means aggressive salmon, 3 means ecoli
    addEnemies();
    playSound(SOUND_BACTERIUM_BORN);
    int x = 0;
    while (x == 0) {
        x = randInt(1, 3);
        if (x == 1) {
            if (self->getRegSalmon() > 0) {
                break;
            }
            x = 0; // set back to 0 if no more salmon left
        } else if (x == 2) {
            if (self->getAggSalmon() > 0) {
                break;
            }
            x = 0;
        } else if (x == 3) {
            if (self->getEColi() > 0) {
                break;
            }
            x = 0;
        }
    }
    if (x == 1) { // add regular salmon
        RegularSalmonella* r = new RegularSalmonella(self->getX(), self->getY(), this);
        self->decRegSalmon();
        myActors.push_back(r);
    } else if (x == 2) { // add aggressive salmon
        AggressiveSalmonella* r = new AggressiveSalmonella(self->getX(), self->getY(), this);
        self->decAggSalmon();
        myActors.push_back(r);
    } else if (x == 3) { // add e coli
        EColi* r = new EColi(self->getX(), self->getY(), this);
        self->decEColi();
        myActors.push_back(r);
    }
}

void StudentWorld::addRegularSalmonella(double x, double y) { // adds regular salmonella at specified location
    RegularSalmonella* r = new RegularSalmonella(x, y, this);
    myActors.push_back(r);
    addEnemies();
    playSound(SOUND_BACTERIUM_BORN);
}

void StudentWorld::addAggressiveSalmonella(double x, double y) { // adds aggressive salmonella at specified location
    AggressiveSalmonella* a = new AggressiveSalmonella(x, y, this);
    myActors.push_back(a);
    addEnemies();
    playSound(SOUND_BACTERIUM_BORN);
}

void StudentWorld::addFood(double x, double y) { // adds food at specified location
    Food* f = new Food(x, y, this);
    myActors.push_back(f);
}

void StudentWorld::addEColi(double x, double y) { // adds eColi at specified location
    EColi* e = new EColi(x, y, this);
    myActors.push_back(e);
    addEnemies();
    playSound(SOUND_BACTERIUM_BORN);
}
