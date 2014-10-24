#include "ConsoleBuffer.h"
#include "Weapon.h"
#include "Player.h"
#include "AsteroidObj.h"
using namespace ConsoleBuf;
using namespace Asteroids;

const char AsteroidObj::ASTEROID_CHAR = '@';
const unsigned short AsteroidObj::ASTEROID_COLOR = 0x08; // this is gray
AsteroidObj::AsteroidObj(int Size)
    : ASTEROID_DIM(Size),_vBuf( GetNewVBuffer(Size,Size) )
{
    //initialize damage byte map
    _damage = new bool*[ASTEROID_DIM];
    //damage holds whether or not a cell is destroyed
    for (int r = 0;r<ASTEROID_DIM;r++)
    {
        _damage[r] = new bool[ASTEROID_DIM];
        for (int c = 0;c<ASTEROID_DIM;c++)
            _damage[r][c] = true;
    }
    //  the corners aren't part of the asteroid
    _damage[0][0] = false;
    _damage[0][ASTEROID_DIM-1] = false;
    _damage[ASTEROID_DIM-1][0] = false;
    _damage[ASTEROID_DIM-1][ASTEROID_DIM-1] = false;
    //initialize logic
    change_course();
    update();
}
AsteroidObj::~AsteroidObj()
{
    // we're dead; clean up our buffer space
    _vBuf.clear();
    //destroy damage memory
    for (int r = 0;r<ASTEROID_DIM;r++)
        delete[] _damage[r];
    delete[] _damage;
}
void AsteroidObj::change_course(int collision_side)
{
    if (collision_side)
    {// we hit something and need to go the opposite way
        switch (collision_side)
        {
        case 1:
        case 2:
            // we've hit a y-boundry; turn around
            _vectorY *= -1;
            break;
        case 3:
        case 4:
            // we've hit an x-boundry; turn around
            _vectorX *= -1;
            break;
        default:
            break;
        }
        return;
    }
    // get new random components; make sure both aren't equal to zero
    _vectorX = 0;
    _vectorY = 0;
    while (!_vectorX || !_vectorY)
    {
        _vectorX = RandomNumber(5,-3); // a number [-2,2]
        _vectorY = RandomNumber(5,-2);
    }
    if (abs(_vectorX) == abs(_vectorY))
    {
        // make one component greater (in absolute value) than the other
        bool useX = RandomNumber(100,0)%2 == 0;
        int& dominant = (useX ? _vectorX : _vectorY);
        int& lesser = (useX ? _vectorY : _vectorX);
        switch (dominant)
        {
        case 0:
        case 1:
            dominant++;
            break;
        case -1:
            dominant--;
            break;
        }
        switch (lesser)
        {
        case 2:
            lesser--;
            break;
        case -2:
            lesser++;
            break;
        }

    }
}
void AsteroidObj::update()
{
    //we're changing locations and need to erase the old
    _vBuf.clear();
    //logic update
    if (!_vBuf.change_location(_vectorX,_vectorY))
    {
        int collision_side;
        int x = _vBuf.get_x_location()+_vectorX;
        if (x<0)
            collision_side = 3; // left collision
        else if (x+ASTEROID_DIM>=SCREEN_WIDTH)
            collision_side = 4; // right collision
        else
        {
            int y = _vBuf.get_y_location()+_vectorY;
            if (y<0)
                collision_side = 1; // upper collision
            else if (y+ASTEROID_DIM>=SCREEN_HEIGHT)
                collision_side = 2; // lower collision;
        }
        change_course(collision_side);
    }
}
void AsteroidObj::render() const
{
    // update buffer
    for (int r = 0;r<ASTEROID_DIM;r++)
        for (int c = 0;c<ASTEROID_DIM;c++)
        {
            char ch = _vBuf.get_character(c,r);
            if (ch==Warhead::WARHEAD_CHAR_X || ch==Warhead::WARHEAD_CHAR_Y ||
                    ch==Gun::BULLET_CHAR)
                    _damage[r][c] = false;
            if (_damage[r][c])
            {
                _vBuf.write_at(c,r,ASTEROID_CHAR);
                _vBuf.chng_color_at(c,r,ASTEROID_COLOR);
            }
            else
            {// draw "blown up" part
                _vBuf.write_at(c,r,'.');
                _vBuf.chng_color_at(c,r,ASTEROID_COLOR);
            }
        }
}
void AsteroidObj::set_location(int x,int y)
{
    _vBuf.clear(); // need to clear because location is changing outside of "update"
    _vBuf.set_location(x,y);
}
int AsteroidObj::health() const
{
    int intactCnt = 0, destroyedCnt = 0;
    for (int r = 0;r<ASTEROID_DIM;r++)
        for (int c = 0;c<ASTEROID_DIM;c++)
            if (_damage[r][c])
                ++intactCnt;
    return intactCnt;
}
int AsteroidObj::health_percent() const
{
    int total = (ASTEROID_DIM*ASTEROID_DIM-4);
    double fraction = (double)health() / (double)total;
    fraction *= 100.00;
    return (int) fraction;
}
bool AsteroidObj::is_dead() const
{
    // as asteroid is destroyed if half the priliminary non-damaged cells have been destroyed
    return health() <= (ASTEROID_DIM*ASTEROID_DIM-4)/2;
}
bool AsteroidObj::overlaps(int col,int row) const
{
    // make "col" and "row" relative to our location
    col -= _vBuf.get_x_location();
    if (col<0 || col>=ASTEROID_DIM)
        return false;
    row -= _vBuf.get_y_location();
    if (row<0 || row>=ASTEROID_DIM)
        return false;
    // see if the cell is occupied
    return _damage[row][col];
}

const short Enemy::ENEMY_DIM = 2;
const unsigned char Enemy::ENEMY_CHAR = 232;
const unsigned short Enemy::ENEMY_COLOR = ConsoleBuffer::ForeRed | ConsoleBuffer::BackGreen;
Enemy::Enemy(const Plyr& player)
    : _thePlayer(&player),_vBuf(GetNewVBuffer(ENEMY_DIM,ENEMY_DIM))
{
    _vectorX = 0;
    _vectorY = 0;
}