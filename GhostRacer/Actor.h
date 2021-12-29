#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

class Actor : public GraphObject
{
  public:
    Actor(StudentWorld* world, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual void doSomething() = 0;
    virtual void damage(int p) { }
    virtual bool collisionAvoidanceWorthy() const { return true; }
    virtual bool affectedByHolyWater() const { return true; }
    bool overlap(Actor* a, Actor* b);
    int getLane(Actor* a);
    
    // getters
    StudentWorld* getStudentWorld() const { return m_studentWorld; }
    double getVerticalSpeed() const { return m_verticalSpeed; }
    double getHorizontalSpeed() const { return m_horizontalSpeed; }
    int getHitPoints() const { return m_hitPoints; }
    bool isAlive() const { return m_isAlive; }
    
    // setters
    void setVerticalSpeed(double v) { m_verticalSpeed = v; }
    void setHorizontalSpeed(double h) { m_horizontalSpeed = h; }
    void setHitPoints(int h) { m_hitPoints = h; }
    void setNotAlive() { m_isAlive = false; }
    
  protected:
    void move();
    const double LEFT_EDGE;
    const double RIGHT_EDGE;
    
  private:
    StudentWorld* m_studentWorld;
    double m_verticalSpeed;
    double m_horizontalSpeed;
    int m_hitPoints;
    bool m_isAlive;
};

class GhostRacer : public Actor
{
  public:
    GhostRacer(StudentWorld* world);
    virtual void doSomething();
    virtual void damage(int p);
    virtual bool affectedByHolyWater() const { return false; }
    void spin();
    void heal();
    int getHolyWater() const { return m_holyWater; }
    void incHolyWater() { m_holyWater += 10; }
    
  private:
    int m_holyWater;
};

class BorderLine : public Actor
{
  public:
    BorderLine(StudentWorld* world, int imageID, double startX, double startY);
    virtual void doSomething();
    virtual bool collisionAvoidanceWorthy() const { return false; }
    virtual bool affectedByHolyWater() const { return false; }
};

class Agent : public Actor
{
  public:
    Agent(StudentWorld* world, int imageID, double startX, double startY, double size);
    
  protected:
    int getMovementPlan() const { return m_movementPlan; }
    void setMovementPlan(int m) { m_movementPlan = m; }
    
  private:
    int m_movementPlan;
};

class HumanPedestrian : public Agent
{
  public:
    HumanPedestrian(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual void damage(int p);
};

class ZombiePedestrian : public Agent
{
  public:
    ZombiePedestrian(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual void damage(int p);
    
  private:
    int m_ticksUntilGrunt;
};

class ZombieCab : public Agent
{
  public:
    ZombieCab(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual void damage(int p);
    
  private:
    bool m_damagedGhostRacer;
};

class OilSlick : public Actor
{
  public:
    OilSlick(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual bool collisionAvoidanceWorthy() const { return false; }
    virtual bool affectedByHolyWater() const { return false; }
};

class Goodie : public Actor
{
  public:
    Goodie(StudentWorld* world, int imageID, double startX, double startY, int dir, double size);
    virtual bool collisionAvoidanceWorthy() const { return false; }
};

class HealingGoodie : public Goodie
{
  public:
    HealingGoodie(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual void damage(int p);
};

class HolyWaterGoodie : public Goodie
{
  public:
    HolyWaterGoodie(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual void damage(int p);
};

class SoulGoodie : public Goodie
{
  public:
    SoulGoodie(StudentWorld* world, double startX, double startY);
    virtual void doSomething();
    virtual bool affectedByHolyWater() const { return false; }
};

class HolyWaterProjectile : public Actor
{
  public:
    HolyWaterProjectile(StudentWorld* world, double startX, double startY, int dir);
    virtual void doSomething();
    virtual bool collisionAvoidanceWorthy() const { return false; }
    virtual bool affectedByHolyWater() const { return false; }
    
  private:
    int m_travelDistance;
};

#endif // ACTOR_H_
