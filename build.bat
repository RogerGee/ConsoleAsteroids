REM Run in Visual Studio Command Prompt
@echo off
cl /EHsc /I RLibrary "asteroidobj.cpp" "asteroids.cpp" "consolebuffer.cpp" "gamesave.cpp" "player.cpp" "weapon.cpp" /c
link "asteroidobj.obj" "asteroids.obj" "consolebuffer.obj" "gamesave.obj" "player.obj" "weapon.obj" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /OUT:ConsoleAsteroids.exe
del "asteroidobj.obj" "asteroids.obj" "consolebuffer.obj" "gamesave.obj" "player.obj" "weapon.obj"