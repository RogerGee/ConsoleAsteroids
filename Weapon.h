//Warhead.h
#ifndef WARHEAD_H
#define WARHEAD_H

extern ConsoleBuf::ConsoleBuffer* GetBuffer(void);

namespace Asteroids
{
    class Warhead // represents a missle that has to be recharged
    {
    public:
        Warhead();
        void fire(int startCol,int startRow,int vectorX,int vectorY);
        void render();
        bool is_firing() const { return _i>=0 && _i<50; }
        void reset();
        int get_location_x() const;
        int get_location_y() const;
        static const unsigned char WARHEAD_CHAR_X;
        static const unsigned char WARHEAD_CHAR_Y;
    private:
        static const unsigned short WARHEAD_COLOR;
        struct pos
        { pos() : row(0),col(0) {} int row,col; };
        ConsoleBuf::ConsoleBuffer* _buf;
        pos _vector;
        pos _trail[50];
        int _i;
        void _draw();
        void _clearAll();
    };

    class Gun
    {
    public:
        Gun();
        void fire(int startCol,int startRow,int vectorX,int vectorY);
        void render();
        void reset();
        int bullets_fired() const;
        static const unsigned char BULLET_CHAR;
        static const int BULLET_MAX;
    private:
        static const unsigned short BULLET_COLOR;
        static const int MAX_BULLET_ITERATIONS;
        struct bullet
        { 
            bullet() : row(-1),col(-1),vectorX(0),vectorY(0),iterations(0) {}
            int row,col;
            int vectorX,vectorY;
            int iterations;
        };
        ConsoleBuf::ConsoleBuffer* _buf;
        bullet _mag[20];
        void _drawMag();
    };
}

#endif