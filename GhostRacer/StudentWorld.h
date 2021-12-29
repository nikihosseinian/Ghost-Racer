#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>

class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
  public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool actorInFront(Actor* a);
    bool actorBehind(Actor* a);
    bool holyWaterActivated(Actor* a);
    int getSoulsToSave() { return m_soulsToSave; }
    void decSoulsToSave() { m_soulsToSave--; }
    GhostRacer* getGhostRacer() { return m_ghostRacer; }
    
    // add actors
    void addZombieCab();
    void addOilSlick(double startX, double startY);
    void addHealingGoodie(double startX, double startY);
    void addHolyWaterProjectile(double startX, double startY, int dir);

  private:
    // functions
    int max(int a, int b);
    void setLane(int l);
    
    // data members
    GhostRacer* m_ghostRacer;
    std::vector<Actor*> m_actors;
    int m_soulsToSave;
    int m_bonus;
    double m_coordinate;
    int m_leftBound;
    int m_rightBound;
    double m_startX;
    
    const double LEFT_EDGE;
    const double RIGHT_EDGE;
};

#endif // STUDENTWORLD_H_
