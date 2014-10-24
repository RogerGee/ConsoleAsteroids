//ConsoleBuffer.h
#ifndef CONSOLEBUFFER_H
#define CONSOLEBUFFER_H
#include "Windows.h"

namespace ConsoleBuf
{
    const int SCREEN_WIDTH = 80; // this is the typical width for CMD.exe
    const int SCREEN_HEIGHT = 55; // works for maximized cmd windows

    class ConsoleBuffer
    {// represents a console buffer that operates within the given constant limits SCREEN_WIDTH and SCREEN_HEIGHT
    public:
        ConsoleBuffer();
        ~ConsoleBuffer();
        void goto_cursor() const;
        void print() const;
        void set_cursor(int col,int row);
        void write_at(int col,int row,char data);
        void write_array(int col,int row,const char data[]); // data is a null-terminated string of characters
        void clear_at(int col,int row);
        void chng_color_at(int col,int row,unsigned short color);
        void scroll_up();
        void clear();
        int get_cursor_col() const { return _bufCoord.X; }
        int get_cursor_row() const { return _bufCoord.Y; }
        CHAR_INFO* operator [](int);
        const CHAR_INFO* operator [](int) const;
        operator void*() const;
        static const unsigned short Black;
        static const unsigned short ForeBlue;
        static const unsigned short ForeGreen;
        static const unsigned short ForeRed;
        static const unsigned short BoldFore;
        static const unsigned short BackBlue;
        static const unsigned short BackGreen;
        static const unsigned short BackRed;
        static const unsigned short BoldBack;
    protected:
        bool _failState;
    private:
        HANDLE _hConsole;
        CHAR_INFO _buf[SCREEN_HEIGHT][SCREEN_WIDTH];
        COORD _bufCoord;
        mutable SMALL_RECT _bufferRect;
    };

    /*
        Provides an interface for an object to render itself on
            a console buffer. This virtual buffer checks the bounds on
            all locations, meaning it will keep whatever object it supports
            within the viewable area. It makes this determination based on
            the size and location of the supported object.
            */
    class VirtualConsoleBuffer
    {
    public:
        VirtualConsoleBuffer(int width,int height,ConsoleBuffer& buffer);
        void clear();
        bool set_location(int x,int y); // adjusts location if it goes out-of-bounds; return true if adjusted
        //return status indicates if the change was blocked by a buffer boundry
        bool change_location(int x_amount,int y_amount);
        //these methods DO NOT perform bounds checking
        void write_at(int col,int row,char data);
        void chng_color_at(int col,int row,unsigned short color);
        //
        int get_width() const { return _width; }
        int get_height() const { return _height; }
        int get_x_location() const { return _x; }
        int get_y_location() const { return _y; }
        unsigned short get_color(int col,int row) const;
        char get_character(int col,int row) const;
    private:
        ConsoleBuffer* _bufPtr;
        const int _width;
        const int _height;
        int _x,_y;
    };

    struct ConsoleButton
    {
        ConsoleButton();
        bool is_clicked(int col,int row) const;
        bool set_location(int col,int row); // this method checks bounds; returns success
        void set_text(const char*); // this method will truncate text if it goes out of bounds
        void print(ConsoleBuffer& buf,unsigned short color) const;
        int get_x_location() const { return _locX; }
        int get_y_location() const { return _locY; }
        const char* get_text() const { return _text; }
        int get_text_length() const { return _len; }
    private:
        const char* _text;
        int _len;
        int _locX,_locY;
    };
}

#endif