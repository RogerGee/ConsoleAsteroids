#include <iostream>
#include "ConsoleBuffer.h"
#include "Weapon.h"
#include "Player.h"
#include "AsteroidObj.h"
#include "Asteroids.h"
#include <time.h>
using namespace std;
using namespace rtypes;
using namespace ConsoleBuf;
using namespace Asteroids;

//memory leak stuff
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

namespace
{// global objects
    ConsoleBuffer Buffer, debugScreen;
    Plyr Player; const char* PlayerName;
    container<AsteroidObj*> asteroidObjects; // use ptrs to better control dynamic memory management

    //welcome screen buttons and flags
    ConsoleButton btnPlay, btnGameStats, btnExit, btnDebug;
    bool debugFlag = false; // if true then render debug info

    //chars and colors
    const byte SEL_COLOR = ConsoleBuffer::ForeGreen | ConsoleBuffer::ForeRed;
    const byte SEL_CHAR = 178;

    //misc.
    str ConsoleAsteroidsVersionString("Console Asteroids version 1.10 - Roger Gee");
}

// game object control
AsteroidObj* CreateNewAsteroid(int);
void UpdateAsteroids();
void RenderAsteroids();
int CheckAsteroidDeath(); // returns the score earned from the deaths
void DestroyAllAsteroids();
int AsteroidCount();

// gameplay control functions
int GameLoop();
int KeySel();
void ScreenWarmUp(); // just a cool little intro
void prepare_welcome_screen();
int quit_screen(const char*,bool);
void create_stage(int&); // takes the current stage number as an argument

//other functions
void write_debug_info(); // writes debug info to the debug screen
void write_game_info(int,int); // writes the game info to the debug screen

int length(const char s[])
{
    int l = 0;
    while (s[l]) 
        l++;
    return l;
}

// functions from Asteroids.h
str Asteroids::to_string(int n)
{
    str r;
    int copy;
    bool is_neg = false;
    //handle negatives
    if (n<0)
    {
        is_neg = true;
        n *= -1;
    }
    // get digit cnt
    copy = n;
    while (copy>0)
    {
        r.push_back(' ');
        copy /= 10;
    }
    if (!r.size())
        return "00";
    //handle negative sign
    if (is_neg)
    {
        r.push_back(' ');
        r[0] = '-';
    }
    //get digits
    for (int i = r.size()-1;i>=0 && n>0;i--,n/=10)
        r[i] = '0'+n%10;
    if (r.size()<2)
    {//format number width to be at least two characters for console display purposes
        str hold = r;
        r = "0";
        r += hold;
    }
    return r;
}

//these functions are used in other modules as well
ConsoleBuffer* GetBuffer()
{
    return &Buffer;
}
VirtualConsoleBuffer GetNewVBuffer(int width,int height) // AsteroidObj class requires this function
{// get a virtual buffer that routes to the screen buffer for the specified size
    return VirtualConsoleBuffer(width,height,Buffer);
}
int RandomNumber(int max,int offset)
{
    /*
        Expect RandomNumber(max,offset) to give some n, such that n>=1+offset && n<=max+offset
            e.g. RandomNumber(5,-5) will produce some n, such tath -4 <= n <= 0
            */
    return rand()%max + 1 + offset;
}

int main(int argc,const char* args[])
{
    //check for errors with buffer creation
    if (!Buffer || !debugScreen)
    {
        cout << "Error - could not access console resources\n";
        return 1;
    }

    // load up player stats
    //LoadSaveGameData();

    //seed the puesdo-random number generator
    srand( (unsigned int) time(NULL) );

    //init buttons
    btnPlay.set_text("Play");
    btnPlay.set_location(5,1);
    btnGameStats.set_text("Stats");
    btnGameStats.set_location(btnPlay.get_x_location()+btnPlay.get_text_length()+1,1);
    btnExit.set_text("Exit");
    btnExit.set_location(btnGameStats.get_x_location()+btnGameStats.get_text_length()+1,1);
    btnDebug.set_text("Display Debug Info");
    btnDebug.set_location(btnExit.get_x_location()+btnExit.get_text_length()+1,1);

    // set up the debug screen right under the main buffer screen
    debugScreen.set_cursor(0,SCREEN_HEIGHT+Buffer.get_cursor_row());

    // seek to main buffer cursor to move output to game rendering region
    Buffer.goto_cursor();

    int usrKeySel = 0;
    do
    {
        // show a cool intro. screen
        ScreenWarmUp();

        // main menu (key selection) screen
        usrKeySel = KeySel();

        if (usrKeySel)
        {
            FlushConsoleInputBuffer( GetStdHandle(STD_INPUT_HANDLE) ); // there's a newline in the input buffer (plus any other keys the use typed)!!
            unsigned int playerID = PromptName(); // get the player id
            int playerScore = GameLoop(); // play the game, get a score
            PutScore(playerID,playerScore); // record player score
        }
        
    } while (usrKeySel);

    // ** WIN32 API **
    //flush any input from the input buffer
    FlushConsoleInputBuffer( GetStdHandle(STD_INPUT_HANDLE) );
    //

    //the cursor is correctly positioned at the end of the main game output area

    //do this so memory leak testing will have minimal offset
    //  deallocate as much memory as possible from global objects that
    //  dynamically allocate memory
    asteroidObjects.delete_elements(); // leaves 4 bytes unallocated until implicit destruction by compiler
    ConsoleAsteroidsVersionString.clear(); // leaves 2 bytes unallocated until implicit destruction by compiler

    // save player stats
    //SaveSaveGameData();

    //_CrtDumpMemoryLeaks(); // display memory leak information in the output window

    return 0;
}
int quit_screen(const char* msg,bool timeDelayQuit)
{// timeDelayQuit is used to show a message to users
    int msgLen = length(msg);
    ConsoleBuffer quitScreen;
    ConsoleButton yesButton, noButton;
    int selX = SCREEN_WIDTH/2, selY = SCREEN_HEIGHT/2+3;

    if (!quitScreen)
        return 1; // return to main menu -- application error

    quitScreen.set_cursor(Buffer.get_cursor_col(),
                            Buffer.get_cursor_row());

    if (!timeDelayQuit)
    {
        //init buttons
        yesButton.set_text("YES");
        noButton.set_text("NO");
        yesButton.set_location(SCREEN_WIDTH/2-yesButton.get_text_length()-1,SCREEN_HEIGHT/2);
        noButton.set_location(SCREEN_WIDTH/2+noButton.get_text_length()+1,SCREEN_HEIGHT/2);
    }
    int SleepTime = 3; // in seconds
    while ( (timeDelayQuit ? SleepTime>0 : true) )
    {
        bool keyEnter;
        int oldX = selX, oldY = selY;
        keyEnter = ( GetAsyncKeyState(VK_RETURN) ? true : false );
        if (GetAsyncKeyState(VK_LEFT))
        {
            selX--;
            if (selX<0)
                selX = 0;
        }
        if (GetAsyncKeyState(VK_RIGHT))
        {
            selX++;
            if (selX>=SCREEN_WIDTH)
                selX = SCREEN_WIDTH-1;
        }
        if (GetAsyncKeyState(VK_UP))
        {
            selY--;
            if (selY<0)
                selY = 0;
        }
        if (GetAsyncKeyState(VK_DOWN))
        {
            selY++;
            if (selY>=SCREEN_HEIGHT)
                selY = SCREEN_HEIGHT-1;
        }
        //draw message
        quitScreen.write_array(SCREEN_WIDTH/2-msgLen/2,SCREEN_HEIGHT/2-1,&msg[0]);
        //render the buttons
        yesButton.print(quitScreen,ConsoleBuffer::BackBlue | ConsoleBuffer::ForeGreen);
        noButton.print(quitScreen,ConsoleBuffer::BackGreen | ConsoleBuffer::ForeBlue);
        if (!timeDelayQuit)
        {
            //clear sel
            quitScreen.clear_at(oldX,oldY);
            //draw sel
            quitScreen.write_at(selX,selY,SEL_CHAR);
            quitScreen.chng_color_at(selX,selY,SEL_COLOR);
        }
        //render the buffer
        quitScreen.print();
        //test input
        if (keyEnter && !timeDelayQuit)
        {
            if (yesButton.is_clicked(selX,selY))
                return 1;
            else if (noButton.is_clicked(selX,selY))
                break;
        }
        if (timeDelayQuit)
        {
            SleepTime--;
            Sleep(1000);
        }
        else
            Sleep(50);
    }
    return 0; // no quit (default)
}
int GameLoop()
{// begin a game, play the game, return the player's score
    unsigned int iterations = 0;
    unsigned int lives = 2;

    Buffer.clear();

    int stage = 0; // also represents the number of asteroids to put in each corner each new stage
    int score = 0; // total score, not counting points gained on current stage
    int stage_score = 0; // only the points gained on the current stage

    bool playerDied = false; // if true, the player died on the last iteration

    int oldVectorX = 0, oldVectorY = 0;
    while (true)
    {
        int keyLeft, keyRight, keyUp, keyDown, keyQuit, keySpace, keyMissile;

        if (!AsteroidCount())
        {// no asteroids, create new stage
            score += stage_score;
            create_stage(stage); // this will increment stage to the current stage value
            //reset player location to center screen
            Player.clear();
            Player.goto_center();
            stage_score = 0;
            // give the player an extra life
            if (!playerDied)
                lives++;
            else
                playerDied = false;
        }

        //if (GetAsyncKeyState(VK_ESCAPE))
        //  // quit key
        //  break;
        
        //test key states
        keyLeft = ( GetAsyncKeyState(VK_LEFT) ? -1 : 0 );
        keyRight = ( GetAsyncKeyState(VK_RIGHT) ? 1 : 0 );
        keyUp = ( GetAsyncKeyState(VK_UP) ? -1 : 0 );
        keyDown = ( GetAsyncKeyState(VK_DOWN) ? 1 : 0 );
        keySpace = ( GetAsyncKeyState(VK_SPACE) );
        keyMissile = ( GetAsyncKeyState(0x56 /*V_KEY*/) );
        keyQuit = ( GetAsyncKeyState(0x51 /*Q_KEY*/) );

        if (keyQuit)
            if (quit_screen("Do you want to quit?",false))
                // user wants to quit
                break;

        if (keySpace) // 
            Player.fire_gun(oldVectorX,oldVectorY);
        
        if (keyMissile)
            Player.fire_missile(oldVectorX,oldVectorY);

        //update asteroids
        if (iterations%5==0)
            UpdateAsteroids();

        //print asteroids (must do this before rendering the player)
        RenderAsteroids();

        //see if the player has destroyed any asteroids
        stage_score += CheckAsteroidDeath();

        if (Player.in_dead_zone() && !debugFlag) // debug flag makes the player invincible, otherwise, how could you debug
        {//the player died - an asteroid collided with the player
            lives--; // lose a life
            if (!lives)
            {
                quit_screen("Game Over! You're out of lives! Of course, you already knew that!",true);
                break;
            }
            str msg = "You died. Lives left: ";
            msg += to_string(lives);
            quit_screen(msg.c_str(),true);
            DestroyAllAsteroids(); // reset stage
            stage_score = 0; // player doesn't get any of the points because they died
            stage--; // so it will get incremented back up to current stage during the next iteration
            playerDied = true;
        }

        //update player location and print the player
        Player.render(keyLeft+keyRight,keyUp+keyDown);

        //update screen
        Buffer.print();

        //update debug information if turned on OR display score
        if (debugFlag)
            write_debug_info();
        else // write score to debug screen
            write_game_info(score+stage_score,lives);
        debugScreen.print();

        //sleep the thread
        Sleep(35);

        //update old vector
        if (keyLeft || keyRight || keyUp || keyDown)
        {
            oldVectorX = keyLeft+keyRight;
            oldVectorY = keyUp+keyDown;
        }

        //update iteration count
        iterations++;
    }

    //delete any left over asteroids
    DestroyAllAsteroids();

    //clear buffer for nice transition
    Buffer.clear();
    debugScreen.clear();
    //write the debug buffer to clear it
    debugScreen.print();

    return score;
}
int KeySel()
{
    int result = 0;
    int selX = 0, selY = 0;
    unsigned int iterations = 0;
    AsteroidObj demos[4] = {AsteroidObj(3),AsteroidObj(3),AsteroidObj(3),AsteroidObj(3)};
    // (the first demo is already at 0,0)
    demos[1].set_location(70,0);
    demos[2].set_location(0,40);
    demos[3].set_location(70,40);
    //move cursor to end
    /*// move to cursor end so if user ^C's the program the cursor will be
            in a somewhat reasonable position
                */
    debugScreen.goto_cursor();
    //get user input
    while (true)
    {
        bool keyEnter;
        int oldX = selX, oldY = selY;
        keyEnter = ( GetAsyncKeyState(VK_RETURN) ? true : false );
        if (GetAsyncKeyState(VK_LEFT))
        {
            selX--;
            if (selX<0)
                selX = 0;
        }
        if (GetAsyncKeyState(VK_RIGHT))
        {
            selX++;
            if (selX>=SCREEN_WIDTH)
                selX = SCREEN_WIDTH-1;
        }
        if (GetAsyncKeyState(VK_UP))
        {
            selY--;
            if (selY<0)
                selY = 0;
        }
        if (GetAsyncKeyState(VK_DOWN))
        {
            selY++;
            if (selY>=SCREEN_HEIGHT)
                selY = SCREEN_HEIGHT-1;
        }
        //update asteroid demo objects
        bool updateDemos = iterations%2 == 0;
        if (updateDemos)
            for (int i = 0;i<4;i++)
                demos[i].update();
        //print demo objects
        for (int i = 0;i<4;i++)
            demos[i].render();
        //  clear sel
        Buffer.clear_at(oldX,oldY);
        //draw welcome screen
        prepare_welcome_screen();
        //  draw sel
        Buffer.write_at(selX,selY,SEL_CHAR);
        Buffer.chng_color_at(selX,selY,SEL_COLOR);
        //present buffer
        Buffer.print();
        //test action
        if (keyEnter)
        {
            if (btnPlay.is_clicked(selX,selY))
            {
                result = 1;
                break;
            }
            else if (btnGameStats.is_clicked(selX,selY))
                ;
            else if (btnExit.is_clicked(selX,selY))
                break; // leave result at 0
            else if (btnDebug.is_clicked(selX,selY))
                debugFlag = !debugFlag;
        }
        //sleep thread
        Sleep(50);
        iterations++;
    }
    //clear buffer
    Buffer.clear();
    //return result
    return result;
}
void prepare_welcome_screen()
{
    Buffer.write_array(0,0,"Welcome to Console Asteroids - Move to an option below and press <ENTER>.");
    btnPlay.print(Buffer,ConsoleBuffer::ForeBlue | ConsoleBuffer::BackGreen);
    btnGameStats.print(Buffer,ConsoleBuffer::ForeGreen | ConsoleBuffer::BackRed);
    btnExit.print(Buffer,ConsoleBuffer::ForeRed | ConsoleBuffer::BackBlue);
    btnDebug.print(Buffer,ConsoleBuffer::ForeBlue | ( debugFlag ? ConsoleBuffer::BackGreen|ConsoleBuffer::BackRed : 0x0000 ));
}
void ScreenWarmUp()
{
    for (int r = 0;r<SCREEN_HEIGHT-3;r++) // subtract two to leave top two lines blank
    {
        for (int c = 0;c<SCREEN_WIDTH;c++)
        {
            Buffer.write_at(c,SCREEN_HEIGHT-1,(char)254);
            Buffer.chng_color_at(c,SCREEN_HEIGHT-1,
                RandomNumber(5,0));
        }
        Buffer.write_array(SCREEN_WIDTH/2-9,SCREEN_HEIGHT-1,"CONSOLE ASTEROIDS!");
        Buffer.print();
        Buffer.scroll_up();
        Sleep(20);
    }
}
void write_debug_info()
{
    str s;
    debugScreen.clear();
    // there is some constant text on the first row; start at the second
    int col = 0; int row = 0;
    s = "Debug Information: ";
    s += ConsoleAsteroidsVersionString;
    debugScreen.write_array(col,row++,s.c_str());
    Player.write_debug_info(debugScreen,col,row++); // write player debug info
    s = "Number of Asteroid Objects: ";
    s += to_string(AsteroidCount());
    debugScreen.write_array(col,row++,s.c_str());
    for (dword i = 0;i<asteroidObjects.size();i++)
    {
        AsteroidObj* p = asteroidObjects[i];
        if (!p)
            continue;
        str line = "Asteroid";
        line += to_string(i); // we want debug info to contain the index within asteroidObjects
        line += ": Loc(";
        line += to_string(p->get_x_location());
        line.push_back(',');
        line += to_string(p->get_y_location());
        line += ") - Vector(";
        line += to_string(p->get_vectorX());
        line.push_back(',');
        line += to_string(p->get_vectorY());
        line += ") - Health: ";
        line += to_string(p->health_percent());
        line.push_back('%');
        debugScreen.write_array(col,row++,line.c_str());
    }
}
void write_game_info(int score,int lives)
{
    int col_sel_start, col_sel_end; // inclusive
    str msg = "YOUR SCORE <"; str s_score = to_string(score);
    col_sel_start = msg.size();
    col_sel_end = col_sel_start+s_score.size()-1;
    msg += s_score;
    msg += "> Lives: ";
    msg += to_string(lives);
    debugScreen.write_array(0,0,msg.c_str());
    // wipe line chars to default fore color
    for (int i = 0;i<SCREEN_WIDTH;i++)
        debugScreen.chng_color_at(col_sel_start,0,ConsoleBuffer::ForeRed|ConsoleBuffer::ForeBlue|ConsoleBuffer::ForeGreen);
    // apply special color formatting
    for (;col_sel_start<=col_sel_end;col_sel_start++)
        debugScreen.chng_color_at(col_sel_start,0,ConsoleBuffer::ForeRed);
    for (dword sel = msg.size()-2;sel<msg.size();sel++)
        debugScreen.chng_color_at((int)sel,0,ConsoleBuffer::ForeBlue);
}
AsteroidObj* CreateNewAsteroid(int Sz)
{
    dword i;
    for (i = 0;i<asteroidObjects.size();i++)
        if (!asteroidObjects[i])
            break;
    if (i>=asteroidObjects.size())
        // create a new ptr to AsteroidObj
        ++asteroidObjects;
    //create new asteroid
    asteroidObjects[i] = new AsteroidObj(Sz);
    return asteroidObjects[i];
}
void UpdateAsteroids()
{
    for (dword i = 0;i<asteroidObjects.size();i++)
        if (asteroidObjects[i])
            asteroidObjects[i]->update();
}
void RenderAsteroids()
{
    for (dword i = 0;i<asteroidObjects.size();i++)
        if (asteroidObjects[i])
            asteroidObjects[i]->render();
}
int CheckAsteroidDeath()
{
    int r_score = 0;
    for (dword i = 0;i<asteroidObjects.size();i++)
        if (asteroidObjects[i] && asteroidObjects[i]->is_dead())
        {
            r_score += asteroidObjects[i]->points(); // add the points the asteroid is worth
            delete asteroidObjects[i];
            asteroidObjects[i] = 0; // mark that the ptr data is not being used and can be used again
        }
    return r_score;
}
void DestroyAllAsteroids()
{
    for (dword i = 0;i<asteroidObjects.size();i++)
        if (asteroidObjects[i])
        {
            delete asteroidObjects[i];
            asteroidObjects[i] = 0;
        }
}
int AsteroidCount()
{
    int cnt = 0;
    for (dword i = 0;i<asteroidObjects.size();i++)
        if (asteroidObjects[i])
            cnt++;
    return cnt;
}
void create_stage(int& stage)
{
    str msg = "STAGE ";
    stage++;
    msg += to_string(stage);
    quit_screen(msg.c_str(),true);
    //create asteroids and enemies for the stage
    //put 'stage' number of asteroids in each corner
    int locX = 0, locY = 0;
    for (int i = 0;i<stage;i++)
    {
        int Sz = RandomNumber(3,2);
        if (RandomNumber(10,0)==10) Sz = Sz;//21;
        AsteroidObj* newAsteroid = CreateNewAsteroid(Sz);
        newAsteroid->set_location(locX-Sz,locY-Sz);
    }
    locX = SCREEN_WIDTH-1;
    for (int i = 0;i<stage;i++)
    {
        int Sz = RandomNumber(3,2);
        if (RandomNumber(10,0)==10) Sz = Sz;//21;
        AsteroidObj* newAsteroid = CreateNewAsteroid(Sz);
        newAsteroid->set_location(locX-Sz,locY-Sz);
    }
    locY = SCREEN_HEIGHT-1;
    for (int i = 0;i<stage;i++)
    {
        int Sz = RandomNumber(3,2);
        if (RandomNumber(10,0)==10) Sz = Sz;//21;
        AsteroidObj* newAsteroid = CreateNewAsteroid(Sz);
        newAsteroid->set_location(locX-Sz,locY-Sz);
    }
    locX = 0;
    for (int i = 0;i<stage;i++)
    {
        int Sz = RandomNumber(3,2);
        if (RandomNumber(10,0)==10) Sz = Sz;//21;
        AsteroidObj* newAsteroid = CreateNewAsteroid(Sz);
        newAsteroid->set_location(locX-Sz,locY-Sz);
    }
}