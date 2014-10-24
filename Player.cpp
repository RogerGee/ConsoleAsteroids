#include "ConsoleBuffer.h"
#include "Weapon.h"
#include "Player.h"
#include "AsteroidObj.h"
#include "Asteroids.h"
using namespace rtypes;
using namespace ConsoleBuf;
using namespace Asteroids;

const unsigned char Plyr::PLAYER_CHAR = 0xce;
const unsigned short Plyr::PLAYER_COLOR = ConsoleBuffer::ForeRed | ConsoleBuffer::ForeGreen;
Plyr::Plyr()
{
    _buf = GetBuffer();
    goto_center();
    render(0,0);
}
bool Plyr::in_dead_zone() const
{
    char ch = (*_buf)[_locY][_locX].Char.AsciiChar;
    return ch==AsteroidObj::ASTEROID_CHAR;
}
void Plyr::render(int x_change,int y_change)
{
    //clear old
    _buf->write_at(_locX,_locY,0);
    _buf->chng_color_at(_locX,_locY,
        ConsoleBuffer::ForeBlue | ConsoleBuffer::ForeRed
                | ConsoleBuffer::ForeGreen);
    //change location
    _locX += x_change;
    _locY += y_change;
    //  check bounds; wrap player loc if out-of-bounds
    if (_locX<0)
        _locX = SCREEN_WIDTH-1;
    if (_locX>=SCREEN_WIDTH)
        _locX = 0;
    if (_locY<0)
        _locY = SCREEN_HEIGHT-1;
    if (_locY>=SCREEN_HEIGHT)
        _locY = 0;
    //write new
    _buf->write_at(_locX,_locY,PLAYER_CHAR);
    _buf->chng_color_at(_locX,_locY,PLAYER_COLOR);
    //render all weapons
    _weaponMissile.render();
    _weaponGun.render();
}
void Plyr::fire_missile(int vectorX,int vectorY)
{
    // fire the warhead
    _weaponMissile.fire(_locX+vectorX,_locY+vectorY,vectorX,vectorY);
}
void Plyr::fire_gun(int vectorX,int vectorY)
{
    //fire a bullet, if able 
    _weaponGun.fire(_locX+vectorX,_locY+vectorY,vectorX,vectorY);
}
void Plyr::goto_center()
{
    _locX = SCREEN_WIDTH/2;
    _locY = SCREEN_HEIGHT/2;
}
void Plyr::clear()
{
    _buf->clear_at(_locX,_locY);
    _weaponMissile.reset();
    _weaponGun.reset();
}
void Plyr::write_debug_info(ConsoleBuffer& buffer,int atX,int atY)
{
    str msg = "Player Information: Loc(";
    msg += to_string(_locX);
    msg += ',';
    msg += to_string(_locY);
    msg += ") - Gun Clip State: ";
    msg += to_string(_weaponGun.bullets_fired());
    msg += '/';
    msg += to_string(Gun::BULLET_MAX);
    msg += " - Missile State: ";
    msg += (_weaponMissile.is_firing() ? "firing" : "inactive");
    buffer.write_array(atX,atY,msg.c_str());
}