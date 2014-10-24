#include "Asteroids.h"
#include <fstream>
using namespace std;
using namespace rtypes;
using namespace Asteroids;

namespace
{// save game data
    const char* save_game_file = "asteroids_data.txt";
    container<str> player_names;
    container<int> player_scores;

    ifstream& operator >>(ifstream& _stream,str& _str)
    {
        _str.clear();
        char c;
        while (_stream>>c)
            _str.push_back(c);
        return _stream;
    }
}

/*void Asteroids::LoadSaveGameData()
{
    ifstream file(save_game_file);
    str s;
    int i;
    while (file>>s>>i)
    {
        player_names.push_back(s);
        player_scores.push_back(i);
    }
}*/
/*void Asteroids::SaveSaveGameData()
{
    // yes, I'm saving this in a plain text format that will
    // let users manually edit the settings
    ofstream file(save_game_file);
    for (dword i = 0;i<player_names.size() && i<player_scores.size();i++)
    {
        file << player_names[i].c_str();
        file.put(' ');
        file << player_scores[i] << endl;
    }
}*/
dword Asteroids::PromptName()
{
    HANDLE hInput, hOutput;
    DWORD dummy;
    CHAR buf[9]; // limit 8 bytes for player name +1 for the terminator +2 for the newline that the user will enter
    LPSTR bufPtr = &buf[0];
    LPSTR message = "Enter your name: ";
    hInput = GetStdHandle(STD_INPUT_HANDLE);
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hInput==INVALID_HANDLE_VALUE || hOutput==INVALID_HANDLE_VALUE)
        throw 1;
    WriteFile(hOutput,message,strlen(message),&dummy,NULL);
    ReadFile(hInput,bufPtr,8,&dummy,NULL);
    dummy -= 2; // back up to the newline ("\r\n")
    bufPtr[dummy] = '\0'; // add null term.
    // search through names to see if player score exists
    for (dword i = 0;i<player_names.size();i++)
        if (player_names[i]==str(bufPtr))
            return i;
    player_names.push_back(bufPtr);
    player_scores.push_back(0);
    return player_names.size()-1;
}
int Asteroids::GetScore(dword PlayerID)
{
    if (PlayerID>=player_scores.size())
        return -1;
    return player_scores[PlayerID];
}
void Asteroids::PutScore(dword PlayerID,int Score)
{
    if (PlayerID<player_scores.size())
        player_scores[PlayerID] = Score;
}