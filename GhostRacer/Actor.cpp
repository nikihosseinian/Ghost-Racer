#include "Actor.h"
#include "StudentWorld.h"
#include <vector>
#include <cmath>
using namespace std;

Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, int dir, double size, unsigned int depth)
: GraphObject(imageID, startX, startY, dir, size, depth), m_studentWorld(world), m_verticalSpeed(0), m_horizontalSpeed(0), m_hitPoints(0), m_isAlive(true), LEFT_EDGE(ROAD_CENTER - ROAD_WIDTH / 2), RIGHT_EDGE(ROAD_CENTER + ROAD_WIDTH / 2)
{

}

void Actor::move()
{
    double vert_speed = getVerticalSpeed() - getStudentWorld()->getGhostRacer()->getVerticalSpeed();
    double horiz_speed = getHorizontalSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    
    // if gone off screen
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
        setNotAlive();
    }
}

bool Actor::overlap(Actor* a, Actor* b)
{
    double delta_x = abs(a->getX() - b->getX());
    double delta_y = abs(a->getY() - b->getY());
    double radius_sum = a->getRadius() + b->getRadius();
    
    if (delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6) {
        return true;
    }
    
    return false;
}

int Actor::getLane(Actor* a)
{
    // returns number associated with each lane
    if (a->getX() >= LEFT_EDGE && a->getX() < LEFT_EDGE + ROAD_WIDTH / 3) {
        return 1;
    }
    
    else if (a->getX() >= LEFT_EDGE + ROAD_WIDTH / 3 && a->getX() < RIGHT_EDGE - ROAD_WIDTH / 3) {
        return 2;
    }
    
    return 3;
}

GhostRacer::GhostRacer(StudentWorld* world)
    : Actor(world, IID_GHOST_RACER, 128, 32, 90, 4, 0), m_holyWater(10)
{
    setHitPoints(100);
}

void GhostRacer::doSomething()
{
    if (!isAlive())
        return;
    
    // if swerving off to left side of road
    if (getX() <= LEFT_EDGE) {
        if (getDirection() > 90) {
            damage(10);
        }
        
        setDirection(82);
        getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    // if swerving off to right side of road
    if (getX() >= RIGHT_EDGE) {
        if (getDirection() < 90) {
            damage(10);
        }
        
        setDirection(98);
        getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    // if player pressed a key
    int key;
    if (getStudentWorld()->getKey(key)) {
        if (key == KEY_PRESS_SPACE && m_holyWater > 0) {
            double delta_x = getX() + SPRITE_HEIGHT * cos(getDirection() * M_PI / 180);
            double delta_y = getY() + SPRITE_HEIGHT * sin(getDirection() * M_PI / 180);
            getStudentWorld()->addHolyWaterProjectile(delta_x, delta_y, getDirection());
            getStudentWorld()->playSound(SOUND_PLAYER_SPRAY);
            m_holyWater--;
        }
        
        if (key == KEY_PRESS_LEFT && getDirection() < 114) {
            setDirection(getDirection() + 8);
        }
        
        if (key == KEY_PRESS_RIGHT && getDirection() > 66) {
            setDirection(getDirection() - 8);
        }
        
        if (key == KEY_PRESS_UP && getVerticalSpeed() < 5) {
            setVerticalSpeed(getVerticalSpeed() + 1);
        }
        
        if (key == KEY_PRESS_DOWN && getVerticalSpeed() > -1) {
            setVerticalSpeed(getVerticalSpeed() - 1);
        }
    }

    int max_shift_per_tick = 4;
    int direction = getDirection();
    double delta_x = cos(direction * M_PI / 180) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::damage(int p)
{
    setHitPoints(getHitPoints() - p);
    
    if (getHitPoints() <= 0) {
        setNotAlive();
    }
}

void GhostRacer::spin()
{
    if (randInt(1, 2) == 1) {
        if (getDirection() <= 100) {
            setDirection(getDirection() + randInt(5, 20));
        }
        
        else {
            setDirection(getDirection() - randInt(5, 20));
        }
    }
    
    else {
        if (getDirection() >= 80) {
            setDirection(getDirection() - randInt(5, 20));
        }
        
        else {
            setDirection(getDirection() + randInt(5, 20));
        }
    }
}

void GhostRacer::heal()
{
    setHitPoints(getHitPoints() + 10);
}

BorderLine::BorderLine(StudentWorld* world, int imageID, double startX, double startY)
    : Actor(world, imageID, startX, startY, 0, 2, 2)
{
    setVerticalSpeed(-4);
}

void BorderLine::doSomething()
{
    move();
}

Agent::Agent(StudentWorld* world, int imageID, double startX, double startY, double size)
    : Actor(world, imageID, startX, startY, 0, size, 0), m_movementPlan(0)
{
    setVerticalSpeed(-4);
    setHitPoints(2);
}

HumanPedestrian::HumanPedestrian(StudentWorld* world, double startX, double startY)
    : Agent(world, IID_HUMAN_PED, startX, startY, 2)
{
    
}

void HumanPedestrian::doSomething()
{
    if (!isAlive()) {
        return;
    }
    
    // if overlaps with Ghost Racer
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        getStudentWorld()->getGhostRacer()->setNotAlive();
        return;
    }
    
    move();
    
    if (!isAlive()) {
        return;
    }
    
    setMovementPlan(getMovementPlan() - 1);
    
    if (getMovementPlan() > 0) {
        return;
    }
    
    // set new movement plan
    setHorizontalSpeed(randInt(randInt(-3, -1), randInt(1, 3)));
    setMovementPlan(randInt(4, 32));
    
    if (getHorizontalSpeed() < 0) {
        setDirection(180);
    }
    
    else {
        setDirection(0);
    }
}

void HumanPedestrian::damage(int p)
{
    // change direction
    setHorizontalSpeed(getHorizontalSpeed() * -1);
    
    if (getHorizontalSpeed() < 0) {
        setDirection(180);
    }
    
    else {
        setDirection(0);
    }
    
    getStudentWorld()->playSound(SOUND_PED_HURT);
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* world, double startX, double startY)
    : Agent(world, IID_ZOMBIE_PED, startX, startY, 3), m_ticksUntilGrunt(0)
{

}

void ZombiePedestrian::doSomething()
{
    if (!isAlive()) {
        return;
    }

    // if overlaps with GhostRacer
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        getStudentWorld()->getGhostRacer()->damage(5);
        damage(2);
        return;
    }

    // if close to and in front of Ghost Racer
    if (abs(getX() - getStudentWorld()->getGhostRacer()->getX()) <= 30 && getY() > getStudentWorld()->getGhostRacer()->getY()) {
        setDirection(270);

        // if to the left of Ghost Racer
        if (getX() < getStudentWorld()->getGhostRacer()->getX()) {
            setHorizontalSpeed(1);
        }

        // if to the right of Ghost Racer
        else if (getX() > getStudentWorld()->getGhostRacer()->getX()) {
            setHorizontalSpeed(-1);
        }

        // if same x coordinate
        else {
            setHorizontalSpeed(0);
        }

        m_ticksUntilGrunt--;

        if (m_ticksUntilGrunt <= 0) {
            getStudentWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticksUntilGrunt = 20;
        }
    }

    move();

    if (!isAlive()) {
        return;
    }

    if (getMovementPlan() > 0) {
        setMovementPlan(getMovementPlan() - 1);
        return;
    }

    // set new movement plan
    setHorizontalSpeed(randInt(randInt(-3, -1), randInt(1, 3)));
    setMovementPlan(randInt(4, 32));

    if (getHorizontalSpeed() < 0) {
        setDirection(180);
    }

    else {
        setDirection(0);
    }
}

void ZombiePedestrian::damage(int p)
{
    setHitPoints(getHitPoints() - p);
    
    if (getHitPoints() <= 0) {
        setNotAlive();
        getStudentWorld()->playSound(SOUND_PED_DIE);
        
        if (!overlap(this, getStudentWorld()->getGhostRacer())) {
            if (randInt(1, 5) == 1) {
                getStudentWorld()->addHealingGoodie(getX(), getY());
            }
        }
        
        getStudentWorld()->increaseScore(150);
        return;
    }
    
    getStudentWorld()->playSound(SOUND_PED_HURT);
}

ZombieCab::ZombieCab(StudentWorld* world, double startX, double startY)
    : Agent(world, IID_ZOMBIE_CAB, startX, startY, 4), m_damagedGhostRacer(false)
{
    setDirection(90);
    setHitPoints(3);
}

void ZombieCab::doSomething()
{
    if (!isAlive()) {
        return;
    }
    
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        if (!m_damagedGhostRacer) {
            getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
            getStudentWorld()->getGhostRacer()->damage(20);
            
            // if to the left of Ghost Racer or has same x coordinate
            if (getX() <= getStudentWorld()->getGhostRacer()->getX()) {
                setHorizontalSpeed(-5);
                setDirection(120 + randInt(0, 19));
            }
            
            // if to the right of Ghost Racer
            else {
                setHorizontalSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            
            m_damagedGhostRacer = true;
        }
    }
    
    move();
    
    if (!isAlive()) {
        return;
    }
    
    if (getVerticalSpeed() > getStudentWorld()->getGhostRacer()->getVerticalSpeed()) {
        if (getStudentWorld()->actorInFront(this)) {
            return;
        }
    }

    if (getVerticalSpeed() <= getStudentWorld()->getGhostRacer()->getVerticalSpeed()) {
        if (getStudentWorld()->actorBehind(this)) {
            return;
        }
    }
    
    setMovementPlan(getMovementPlan() - 1);
    
    if (getMovementPlan() > 0) {
        return;
    }
    
    // pick new movement plan
    setMovementPlan(randInt(4, 32));
    setVerticalSpeed(getVerticalSpeed() + randInt(-2, 2));
}

void ZombieCab::damage(int p)
{
    setHitPoints(getHitPoints() - p);
    
    if (getHitPoints() <= 0) {
        setNotAlive();
        getStudentWorld()->playSound(SOUND_VEHICLE_DIE);
        
        if (randInt(1, 5) == 1) {
            getStudentWorld()->addOilSlick(getX(), getY());
        }
        
        getStudentWorld()->increaseScore(200);
        return;
    }
    
    getStudentWorld()->playSound(SOUND_VEHICLE_HURT);
}

OilSlick::OilSlick(StudentWorld* world, double startX, double startY)
    : Actor(world, IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), 2)
{
    setVerticalSpeed(-4);
}

void OilSlick::doSomething()
{
    move();
    
    if (!isAlive()) {
        return;
    }
    
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        getStudentWorld()->playSound(SOUND_OIL_SLICK);
        getStudentWorld()->getGhostRacer()->spin();
    }
}

Goodie::Goodie(StudentWorld* world, int imageID, double startX, double startY, int dir, double size)
    : Actor(world, imageID, startX, startY, dir, size, 2)
{
    setVerticalSpeed(-4);
}

HealingGoodie::HealingGoodie(StudentWorld* world, double startX, double startY)
    : Goodie(world, IID_HEAL_GOODIE, startX, startY, 0, 1)
{
    
}

void HealingGoodie::doSomething()
{
    move();
    
    if (!isAlive()) {
        return;
    }
    
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        if (getStudentWorld()->getGhostRacer()->getHitPoints() <= 90) {
            getStudentWorld()->getGhostRacer()->heal();
            setNotAlive();
            getStudentWorld()->playSound(SOUND_GOT_GOODIE);
            getStudentWorld()->increaseScore(250);
        }
    }
}

void HealingGoodie::damage(int p)
{
    setNotAlive();
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* world, double startX, double startY)
    : Goodie(world, IID_HOLY_WATER_GOODIE, startX, startY, 90, 2)
{
    
}

void HolyWaterGoodie::doSomething()
{
    move();
    
    if (!isAlive()) {
        return;
    }
    
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        getStudentWorld()->getGhostRacer()->incHolyWater();
        setNotAlive();
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        getStudentWorld()->increaseScore(50);
    }
}

void HolyWaterGoodie::damage(int p)
{
    setNotAlive();
}

SoulGoodie::SoulGoodie(StudentWorld* world, double startX, double startY)
    : Goodie(world, IID_SOUL_GOODIE, startX, startY, 0, 4)
{
    
}

void SoulGoodie::doSomething()
{
    move();
    
    if (!isAlive()) {
        return;
    }
    
    if (overlap(this, getStudentWorld()->getGhostRacer())) {
        getStudentWorld()->decSoulsToSave();
        setNotAlive();
        getStudentWorld()->playSound(SOUND_GOT_SOUL);
        getStudentWorld()->increaseScore(100);
    }
    
    // rotate clockwise
    setDirection(getDirection() - 10);
}

HolyWaterProjectile::HolyWaterProjectile(StudentWorld* world, double startX, double startY, int dir)
    : Actor(world, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1, 1), m_travelDistance(160)
{
    
}

void HolyWaterProjectile::doSomething()
{
    if (!isAlive()) {
        return;
    }

    // if activated
    if (getStudentWorld()->holyWaterActivated(this)) {
        setNotAlive();
        return;
    }
            
    // move forward
    moveForward(SPRITE_HEIGHT);
    m_travelDistance -= SPRITE_HEIGHT;

    // if gone off screen
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
        setNotAlive();
        return;
    }

    // if moved max distance
    if (m_travelDistance <= 0) {
        setNotAlive();
    }
}
