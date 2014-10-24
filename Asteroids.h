//Asteroids.h
#ifndef ASTEROIDS_H
#define ASTEROIDS_H

// use 'byte' from windows.h
#define _BYTE_TDEF_DEF_
#include <Windows.h>
#include "RContainer.h"
//#include "RBinaryFileReader.h"
#include "RString.h"

namespace Asteroids
{
	rtypes::str to_string(int);

	/*// save game functions
	void LoadSaveGameData();
	void SaveSaveGameData();*/
	rtypes::dword PromptName(); // returns a player ID (check for int errors thrown)
	int GetScore(rtypes::dword PlayerID);
	void PutScore(rtypes::dword PlayerID,int Score);
}

#endif