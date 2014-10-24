#include "ConsoleBuffer.h"
#include "Weapon.h"
using namespace ConsoleBuf;
using namespace Asteroids;

const unsigned char Warhead::WARHEAD_CHAR_X = '-';
const unsigned char Warhead::WARHEAD_CHAR_Y = '|';
const unsigned short Warhead::WARHEAD_COLOR = ConsoleBuffer::ForeBlue | ConsoleBuffer::ForeGreen
                                            | ConsoleBuffer::ForeRed | ConsoleBuffer::BackRed;
Warhead::Warhead()
{
    _i = -1;
    _buf = GetBuffer();
}
void Warhead::fire(int startCol,int startRow,int vectorX,int vectorY)
{
    if (is_firing() || (!vectorX && !vectorY)) return; // let the first firing continue
    _i = 0;
    //set update vector
    _vector.col = vectorX;
    _vector.row = vectorY;
    //set start location
    // (fix input in case it's out of bounds)
    if (startCol<0)
        startCol = 0;
    if (startRow<0)
        startRow = 0;
    if (startCol>=SCREEN_WIDTH)
        startCol = SCREEN_WIDTH-1;
    if (startRow>=SCREEN_HEIGHT)
        startRow = SCREEN_HEIGHT-1;
    _trail[0].col = startCol;
    _trail[0].row = startRow;
}
void Warhead::render()
{// rendering progresses with each call to render
    if (!is_firing()) return; // the warhead isn't active
    //render up to current position; it was put in at either the last call to this function or to fire
    ++_i;
    _draw(); // render up to _trail[_i-1];
    //set the next element in the warhead trail
    //  first check to see if we're at the end of the warhead
    if (_i>=50)
    {
        reset(); // once the missle has run out it is erased
        return;
    }
    //  get the next one by adding the vector components to the last
    _trail[_i] = _trail[_i-1];
    _trail[_i].col += _vector.col;
    _trail[_i].row += _vector.row;
    // check the location
    if (_trail[_i].col<0)
        _trail[_i].col = SCREEN_WIDTH-1;
    if (_trail[_i].row<0)
        _trail[_i].row = SCREEN_HEIGHT-1;
    if (_trail[_i].col>=SCREEN_WIDTH)
        _trail[_i].col = 0;
    if (_trail[_i].row>=SCREEN_HEIGHT)
        _trail[_i].row = 0;
}
void Warhead::reset()
{
    _clearAll();
    _i = -1;
}
void Warhead::_draw()
{
    for (int i = 0, clear_ind = 0;
        i<_i;i++)
    {
        _buf->write_at(_trail[i].col,_trail[i].row,
            ( _vector.row==0 ? WARHEAD_CHAR_X : WARHEAD_CHAR_Y ));
        _buf->chng_color_at(_trail[i].col,_trail[i].row,WARHEAD_COLOR);
    }
}
void Warhead::_clearAll()
{
    for (int i = 0;i<50;i++)
    {
        _buf->clear_at(_trail[i].col,_trail[i].row);
        _trail[i] = pos();
    }
}

const int Gun::BULLET_MAX = 20;
const unsigned char Gun::BULLET_CHAR = '*';
const unsigned short Gun::BULLET_COLOR = ConsoleBuffer::ForeBlue | ConsoleBuffer::ForeRed;
const int Gun::MAX_BULLET_ITERATIONS = 60;
Gun::Gun()
{
    _buf = GetBuffer();
}
void Gun::fire(int startCol,int startRow,int vectorX,int vectorY)
{
    if (!vectorX && !vectorY)
        return;
    int i;
    for (i = 0;i<BULLET_MAX;i++)
        if (_mag[i].col<0 || _mag[i].row<0)
            break;
    if (i>=BULLET_MAX)
        // no bullets left
        return;
    // (fix input in case it's out of bounds)
    if (startCol<0)
        startCol = 0;
    if (startRow<0)
        startRow = 0;
    if (startCol>=SCREEN_WIDTH)
        startCol = SCREEN_WIDTH-1;
    if (startRow>=SCREEN_HEIGHT)
        startRow = SCREEN_HEIGHT-1;
    _mag[i].col = startCol;
    _mag[i].row = startRow;
    _mag[i].vectorX = vectorX;
    _mag[i].vectorY = vectorY;
}
void Gun::render()
{
    //render bullets that are in use
    for (int i = 0;i<BULLET_MAX;i++)
    {
        if (_mag[i].col<0 || _mag[i].row<0)
            continue;
        //render current state
        _mag[i].iterations++;
        //erase old bullet spot
        _buf->clear_at(_mag[i].col,_mag[i].row);
        //see if the bullet has run its course
        if (_mag[i].iterations>=MAX_BULLET_ITERATIONS)
            _mag[i] = bullet(); // reset bullet to null so that it's not drawn
        else
        {//change bullet location
            _mag[i].col += _mag[i].vectorX;
            _mag[i].row += _mag[i].vectorY;
            //if out of bounds, wrap to other side
            if (_mag[i].col<0)
                _mag[i].col = SCREEN_WIDTH-1;
            if (_mag[i].col>=SCREEN_WIDTH)
                _mag[i].col = 0;
            if (_mag[i].row<0)
                _mag[i].row = SCREEN_HEIGHT-1;
            if (_mag[i].row>=SCREEN_HEIGHT)
                _mag[i].row = 0;
        }
    }
    _drawMag();
}
void Gun::reset()
{
    //clear bullets and mark as unused
    for (int i = 0;i<BULLET_MAX;i++)
    {
        if (_mag[i].col<0 || _mag[i].row<0)
            continue; // can't render unfired bullets, causes errors
        _buf->clear_at(_mag[i].col,_mag[i].row);
        _mag[i] = bullet();
    }
}
void Gun::_drawMag()
{
    for (int i = 0;i<BULLET_MAX;i++)
    {
        if (_mag[i].col<0 || _mag[i].row<0)
            continue;
        _buf->write_at(_mag[i].col,_mag[i].row,BULLET_CHAR);
        _buf->chng_color_at(_mag[i].col,_mag[i].row,BULLET_COLOR);
    }
}
int Gun::bullets_fired() const
{
    int r = 0;
    for (int i = 0;i<BULLET_MAX;i++)
        if (_mag[i].col<0 || _mag[i].row<0)
            continue;
        else
            r++;
    return r;
}