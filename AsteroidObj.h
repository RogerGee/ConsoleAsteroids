//Asteroids.h
#ifndef ASTEROIDSOBJ_H
#define ASTEROIDSOBJ_H

// the Asteroids::AsteroidObj class requires these two global functions
// implement this function such that it returns a new virtual console buffer to your main buffer, given the specified dimensions
extern ConsoleBuf::VirtualConsoleBuffer GetNewVBuffer(int,int);
extern int RandomNumber(int max,int offset);
//

namespace Asteroids
{
    class AsteroidObj
    {
    public:
        AsteroidObj(int Size);
        ~AsteroidObj();
        void change_course(int collision_side = 0); // sides: 1==upper,2==lower,3==left,4==right
        void update(); // general update message
        void render() const;
        void set_location(int x,int y);
        bool is_dead() const;
        int health() const;
        int health_percent() const;
        bool overlaps(int col,int row) const; // row and col are relative to the screen
        int get_x_location() const { return _vBuf.get_x_location(); }
        int get_y_location() const { return _vBuf.get_y_location(); }
        int get_vectorX() const { return _vectorX; }
        int get_vectorY() const { return _vectorY; }
        int points() const { return ASTEROID_DIM; }
        static const char ASTEROID_CHAR;
    private:
        static const unsigned short ASTEROID_COLOR;
        mutable ConsoleBuf::VirtualConsoleBuffer _vBuf; // keep track of location through the virtual buffer
        int _vectorX; // units: cells per update
        int _vectorY;
        const int ASTEROID_DIM;
        bool **_damage;
    };

    class Enemy
    {
    public:
        Enemy(const Plyr&);

    private:
        static const short ENEMY_DIM;
        static const unsigned char ENEMY_CHAR;
        static const unsigned short ENEMY_COLOR;
        const Plyr* _thePlayer;
        ConsoleBuf::VirtualConsoleBuffer _vBuf;
        int _vectorX, _vectorY;
    };
}

#endif