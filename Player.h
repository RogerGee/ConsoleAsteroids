//Player.h - player module for Asteroids
#ifndef PLAYER_H
#define PLAYER_H

extern ConsoleBuf::ConsoleBuffer* GetBuffer(void);

namespace Asteroids
{
    class Plyr
    {
    public:
        Plyr();
        void render(int x_change,int y_change); // technically, render with a change
        void fire_missile(int vectorX,int vectorY);
        void fire_gun(int vectorX,int vectorY);
        int get_cur_col() const { return _locX; }
        int get_cur_row() const { return _locY; }
        bool in_dead_zone() const;
        void goto_center();
        void clear();
        void write_debug_info(ConsoleBuf::ConsoleBuffer&,int atX,int atY); // write debug info to the specified buffer
    private:
        static const unsigned char PLAYER_CHAR;
        static const unsigned short PLAYER_COLOR;
        ConsoleBuf::ConsoleBuffer* _buf;
        Warhead _weaponMissile;
        Gun _weaponGun;
        int _locX, _locY;
    };
}

#endif