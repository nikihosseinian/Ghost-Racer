#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), LEFT_EDGE(ROAD_CENTER - ROAD_WIDTH / 2), RIGHT_EDGE(ROAD_CENTER + ROAD_WIDTH / 2)
{
    
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_soulsToSave = 2 * getLevel() + 5;
    m_bonus = 5000;
    
    // create a Ghost Racer
    m_ghostRacer = new GhostRacer(this);
    
    // add yellow border lines
    const int N = VIEW_HEIGHT / SPRITE_HEIGHT;

    for (int i = 0; i < N; i++) {
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT));
    }
    
    // add white border lines
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);

    for (int i = 0; i < M; i++) {
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, i * 4 * SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, i * 4 * SPRITE_HEIGHT));
    }
    
    m_coordinate = m_actors[m_actors.size() - 1]->getY();
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_ghostRacer->isAlive()) {
        m_ghostRacer->doSomething();
    }
    
    vector<Actor*>::iterator it;
    
    // let each actor do something
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->isAlive()) {
            (*it)->doSomething();

            if (!m_ghostRacer->isAlive()) {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            
            // if finished level
            if (m_soulsToSave == 0) {
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    // remove dead actors
    for (it = m_actors.begin(); it != m_actors.end(); ) {
        if (!(*it)->isAlive()) {
            delete *it;
            m_actors.erase(it);
        }
        
        else {
            it++;
        }
    }
    
    m_coordinate -= 4 + m_ghostRacer->getVerticalSpeed();
    const double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    const double delta_y = new_border_y - m_coordinate;
    
    // add new yellow border lines
    if (delta_y >= SPRITE_HEIGHT) {
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));
    }

    // add new white border lines
    if (delta_y >= 4 * SPRITE_HEIGHT) {
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, new_border_y));
        m_coordinate = m_actors[m_actors.size() - 1]->getY();
    }
    
    // add human pedestrians
    int chanceHumanPed = max(200 - getLevel() * 10, 30);
    
    if (randInt(0, chanceHumanPed - 1) == 0) {
        m_actors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    
    // add zombie pedestrians
    int chanceZombiePed = max(100 - getLevel() * 10, 20);
    
    if (randInt(0, chanceZombiePed - 1) == 0) {
        m_actors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    
    // add zombie cabs
    int chanceVehicle = max(100 - getLevel() * 10, 20);
    
    if (randInt(0, chanceVehicle - 1) == 0) {
        addZombieCab();
    }
    
    // add oil slicks
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    
    if (randInt(0, chanceOilSlick - 1) == 0) {
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE + SPRITE_WIDTH, RIGHT_EDGE - SPRITE_WIDTH), VIEW_HEIGHT));
    }
    
    // add holy water goodies
    int chanceHolyWater = 100 + 10 * getLevel();
    
    if (randInt(0, chanceHolyWater - 1) == 0) {
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE + SPRITE_WIDTH, RIGHT_EDGE - SPRITE_WIDTH), VIEW_HEIGHT));
    }
    
    // add soul goodies
    int chanceSoul = 100;
    
    if (randInt(0, chanceSoul - 1) == 0) {
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE + SPRITE_WIDTH, RIGHT_EDGE - SPRITE_WIDTH), VIEW_HEIGHT));
    }
    
    // decrease bonus points
    m_bonus--;
    
    string stat = "Score: " + to_string(getScore()) + "  Lvl: " + to_string(getLevel()) + "  Souls2Save: " + to_string(getSoulsToSave()) + "  Lives: " + to_string(getLives()) + "  Health: " + to_string(m_ghostRacer->getHitPoints()) + "  Sprays: " + to_string(m_ghostRacer->getHolyWater()) + "  Bonus: " + to_string(m_bonus);
    
    // update status line
    setGameStatText(stat);
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_ghostRacer;
    
    vector<Actor*>::iterator it;
    
    for (it = m_actors.begin(); it != m_actors.end(); ) {
        delete *it;
        it = m_actors.erase(it);
    }
}

int StudentWorld::max(int a, int b)
{
    if (a > b) {
        return a;
    }
    
    return b;
}

void StudentWorld::addZombieCab()
{
    int cur_lane = randInt(1, 3);
    setLane(cur_lane);
    
    double cur_y;
    double start_y;
    int start_vert;
    
    vector<Actor*>::iterator it;
    
    for (int i = 0; i < 3; i++) {
        cur_y = VIEW_HEIGHT;
        
        // find closest actor to the bottom of candidate lane
        if (m_ghostRacer->getX() >= m_leftBound && m_ghostRacer->getX() < m_rightBound) {
            if (cur_y > m_ghostRacer->getY()) {
                cur_y = m_ghostRacer->getY();
            }
        }
        
        for (it = m_actors.begin(); it != m_actors.end(); it++) {
            if ((*it)->collisionAvoidanceWorthy() && (*it)->getX() >= m_leftBound && (*it)->getX() < m_rightBound) {
                if (cur_y > (*it)->getY()) {
                    cur_y = (*it)->getY();
                }
            }
        }

        // if no such actor in candidate lane
        if (cur_y > VIEW_HEIGHT / 3) {
            start_y = SPRITE_HEIGHT / 2;
            start_vert = m_ghostRacer->getVerticalSpeed() + randInt(2, 4);
            m_actors.push_back(new ZombieCab(this, m_startX, start_y));
            m_actors[m_actors.size() - 1]->setVerticalSpeed(start_vert);
            return;
        }
        
        cur_y = 0;
        
        // find closest actor to the top of candidate lane
        if (m_ghostRacer->getX() >= m_leftBound && m_ghostRacer->getX() < m_rightBound) {
            if (cur_y < m_ghostRacer->getY()) {
                cur_y = m_ghostRacer->getY();
            }
        }
        
        for (it = m_actors.begin(); it != m_actors.end(); it++) {
            if ((*it)->collisionAvoidanceWorthy() && (*it)->getX() >= m_leftBound && (*it)->getX() < m_rightBound) {
                if (cur_y < (*it)->getY()) {
                    cur_y = (*it)->getY();
                }
            }
        }
        
        // if no such actor in candidate lane
        if (cur_y < VIEW_HEIGHT * 2 / 3) {
            start_y = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
            start_vert = m_ghostRacer->getVerticalSpeed() - randInt(2, 4);
            m_actors.push_back(new ZombieCab(this, m_startX, start_y));
            m_actors[m_actors.size() - 1]->setVerticalSpeed(start_vert);
            return;
        }
        
        // try a new lane
        if (cur_lane == 1) {
            cur_lane = 2;
        }
        
        else if (cur_lane == 2) {
            cur_lane = 3;
        }
        
        else {
            cur_lane = 1;
        }
        
        setLane(cur_lane);
    }
}

void StudentWorld::setLane(int l) {
    // if left lane
    if (l == 1) {
        m_leftBound = LEFT_EDGE;
        m_rightBound = LEFT_EDGE + ROAD_WIDTH / 3;
        m_startX = ROAD_CENTER - ROAD_WIDTH / 3;
    }
    
    // if middle lane
    if (l == 2) {
        m_leftBound = LEFT_EDGE + ROAD_WIDTH / 3;
        m_rightBound = RIGHT_EDGE - ROAD_WIDTH / 3;
        m_startX = ROAD_CENTER;
    }
    
    // if right lane
    else {
        m_leftBound = RIGHT_EDGE - ROAD_WIDTH / 3;
        m_rightBound = RIGHT_EDGE;
        m_startX = ROAD_CENTER + ROAD_WIDTH / 3;
    }
}

bool StudentWorld::actorInFront(Actor* a)
{
    if (a->getLane(a) == m_ghostRacer->getLane(m_ghostRacer) && a->getY() < m_ghostRacer->getY()) {
        if (m_ghostRacer->getY() - a->getY() < 96) {
            a->setVerticalSpeed(a->getVerticalSpeed() - 0.5);
            return true;
        }
    }
    
    vector<Actor*>::iterator it;
    
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->collisionAvoidanceWorthy() && a->getLane(a) == (*it)->getLane(*it) && a->getY() < (*it)->getY()) {
            if ((*it)->getY() - a->getY() < 96) {
                a->setVerticalSpeed(a->getVerticalSpeed() - 0.5);
                return true;
            }
        }
    }
    
    return false;
}

bool StudentWorld::actorBehind(Actor* a)
{
    vector<Actor*>::iterator it;
    
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->collisionAvoidanceWorthy() && a->getLane(a) == (*it)->getLane(*it) && a->getY() > (*it)->getY()) {
            if (a->getY() - (*it)->getY() < 96) {
                a->setVerticalSpeed(a->getVerticalSpeed() + 0.5);
                return true;
            }
        }
    }
    
    return false;
}

void StudentWorld::addOilSlick(double startX, double startY)
{
    m_actors.push_back(new OilSlick(this, startX, startY));
}

void StudentWorld::addHealingGoodie(double startX, double startY)
{
    m_actors.push_back(new HealingGoodie(this, startX, startY));
}

void StudentWorld::addHolyWaterProjectile(double startX, double startY, int dir)
{
    m_actors.push_back(new HolyWaterProjectile(this, startX, startY, dir));
}

bool StudentWorld::holyWaterActivated(Actor* a)
{
    vector<Actor*>::iterator it;
    
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->affectedByHolyWater() && a->overlap(a, *it)) {
            (*it)->damage(1);
            return true;
        }
    }
    
    return false;
}
