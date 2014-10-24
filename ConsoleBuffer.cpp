#include "ConsoleBuffer.h"
using namespace ConsoleBuf;

const unsigned short ConsoleBuffer::Black = 0x00;
const unsigned short ConsoleBuffer::ForeBlue = 0x01;
const unsigned short ConsoleBuffer::ForeGreen = 0x02;
const unsigned short ConsoleBuffer::ForeRed = 0x04;
const unsigned short ConsoleBuffer::BoldFore = 0x08;
const unsigned short ConsoleBuffer::BackBlue = 0x10;
const unsigned short ConsoleBuffer::BackGreen = 0x20;
const unsigned short ConsoleBuffer::BackRed = 0x40;
const unsigned short ConsoleBuffer::BoldBack = 0x80;
ConsoleBuffer::ConsoleBuffer()
{
    _failState = false;
    // get console handle
    _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!_hConsole)
        _failState = true;
    // set the buffer coordinates
    ::CONSOLE_SCREEN_BUFFER_INFO inf;
    if (GetConsoleScreenBufferInfo(_hConsole,&inf))
        set_cursor(inf.dwCursorPosition.X,inf.dwCursorPosition.Y);
    else
    {
        _failState = true;
        return;
    }
    //prepare buffer rectangle
    _bufferRect.Right = inf.dwSize.X;
    _bufferRect.Bottom = inf.dwSize.Y;
    // initialize buffer data
    clear();
}
ConsoleBuffer::~ConsoleBuffer()
{
}
ConsoleBuffer::operator void*() const
{
    return (void*) (!_failState);
}
void ConsoleBuffer::goto_cursor() const
{
    SetConsoleCursorPosition(_hConsole,_bufCoord);
}
void ConsoleBuffer::print() const
{
    if (_failState)
        return; // it's not going to work correctly anyway
    static const COORD BUFFER_SIZE = {SCREEN_WIDTH,SCREEN_HEIGHT};
    static const COORD ALWAYS_TOP_COORD = {0,0};
    WriteConsoleOutput(_hConsole,
        (CHAR_INFO*)_buf,
        BUFFER_SIZE,
        ALWAYS_TOP_COORD,
        &_bufferRect);
}
void ConsoleBuffer::set_cursor(int col,int row)
{
    _bufCoord.X = col;
    _bufCoord.Y = row;
    _bufferRect.Left = _bufCoord.X;
    _bufferRect.Top = _bufCoord.Y;
    SetConsoleCursorPosition(_hConsole,_bufCoord);
}
void ConsoleBuffer::write_at(int col,int row,char data)
{
    _buf[row][col].Char.AsciiChar = data;
}
void ConsoleBuffer::write_array(int col,int row,const char data[])
{
    for (int r = row,i = 0;r<SCREEN_HEIGHT;r++)
        for (int c = col;c<SCREEN_WIDTH && data[i]!='\0';c++,i++)
        // write the character referenced by i
            write_at(c,r,data[i]);
}
void ConsoleBuffer::clear_at(int col,int row)
{
    write_at(col,row,'\0');
    chng_color_at(col,row,ForeBlue|ForeRed|ForeGreen);
}
void ConsoleBuffer::chng_color_at(int col,int row,unsigned short color)
{
    _buf[row][col].Attributes = (unsigned short) color;
}
void ConsoleBuffer::scroll_up()
{
    for (int row = 0;row<SCREEN_HEIGHT-1;row++)
    {
        CHAR_INFO* next = _buf[row+1];
        for (int col = 0;col<SCREEN_WIDTH;col++)
            _buf[row][col] = next[col];
    }
}
void ConsoleBuffer::clear()
{
    for (int row = 0;row<SCREEN_HEIGHT;row++)
        for (int col = 0;col<SCREEN_WIDTH;col++)
        {
            chng_color_at(col,row,ForeBlue|ForeRed|ForeGreen);
            write_at(col,row,'\0');
        }
}
CHAR_INFO* ConsoleBuffer::operator[] (int i)
{
    return _buf[i];
}
const CHAR_INFO* ConsoleBuffer::operator[] (int i) const
{
    return _buf[i];
}

VirtualConsoleBuffer::VirtualConsoleBuffer(int width,int height,ConsoleBuffer& buffer)
    : _width(width),_height(height)
{
    _bufPtr = &buffer;
    _x = 0;
    _y = 0;
}
void VirtualConsoleBuffer::clear()
{
    for (int r = 0;r<_height;r++)
        for (int c = 0;c<_width;c++)
            _bufPtr->clear_at(c+_x,r+_y);
}
bool VirtualConsoleBuffer::set_location(int x,int y)
{
    bool changed = false;
    _x = x;
    if (_x<0)
    {
        _x = 0;
        changed = true;
    }
    else if (_x+_width-1>=SCREEN_WIDTH)
    {
        _x = SCREEN_WIDTH-1;
        changed = true;
    }
    _y = y;
    if (_y<0)
    {
        _y = 0;
        changed = true;
    }
    else if (_y+_height-1>=SCREEN_HEIGHT)
    {
        _y = SCREEN_HEIGHT-1;
        changed = true;
    }
    return changed;
}
bool VirtualConsoleBuffer::change_location(int x_amount,int y_amount)
{
    bool good = true;
    _x += x_amount;
    if (_x<0)
    {
        _x = 0;
        good = false;
    }
    else if (_x+_width-1>=SCREEN_WIDTH)
    {
        _x = SCREEN_WIDTH-_width;
        good = false;
    }
    _y += y_amount;
    if (_y<0)
    {
        _y = 0;
        good = false;
    }
    else if (_y+_height-1>=SCREEN_HEIGHT)
    {
        _y = SCREEN_HEIGHT-_height;
        good = false;
    }
    return good;
}
void VirtualConsoleBuffer::write_at(int col,int row,char data)
{// col and row are relative to _x and _y respectively
    col += _x;
    row += _y;
    // write char
    _bufPtr->write_at(col,row,data);
}
void VirtualConsoleBuffer::chng_color_at(int col,int row,unsigned short color)
{// col and row are relative to _x and _y respectively
    col += _x;
    row += _y;
    // write color data
    _bufPtr->chng_color_at(col,row,color);
}
unsigned short VirtualConsoleBuffer::get_color(int col,int row) const
{
    if (row+_y>=SCREEN_HEIGHT || col+_x>=SCREEN_WIDTH)
        return 0;
    return (*_bufPtr)[row+_y][col+_x].Attributes;
}
char VirtualConsoleBuffer::get_character(int col,int row) const
{
    col += _x;
    row += _y;
    if (row>=SCREEN_HEIGHT || col>=SCREEN_WIDTH)
        return 0;
    return (*_bufPtr)[row][col].Char.AsciiChar;
}

ConsoleButton::ConsoleButton()
{
    _text = "";
    _len = 0;
    _locX = 0;
    _locY = 0;
}
bool ConsoleButton::is_clicked(int col,int row) const
{
    return row==_locY && col>=_locX && col<_locX+_len;
}
bool ConsoleButton::set_location(int col,int row)
{
    if (col<0 || col>=SCREEN_WIDTH || row<0 || row>=SCREEN_HEIGHT)
        return false;
    _locX = col;
    if (_len>0 && _locX+_len-1>=SCREEN_WIDTH)
        // truncate button text so it won't go out of bounds
        _len = SCREEN_WIDTH-_locX;
    _locY = row;
    return true;
}
void ConsoleButton::set_text(const char* p)
{
    // calculate length
    for (_len = 0;_len<=SCREEN_WIDTH && p[_len]!='\0';_len++);
    // truncate if the text will go out of bounds
    if (_len>0 && _locX+_len-1>=SCREEN_WIDTH)
        _len = SCREEN_WIDTH-_locX;
    _text = p;
}
void ConsoleButton::print(ConsoleBuffer& buf,unsigned short color) const
{
    for (int i = 0;i<_len;i++)
    {
        buf.write_at(i+_locX,_locY,_text[i]);
        buf.chng_color_at(i+_locX,_locY,color);
    }
}