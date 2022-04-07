#pragma once
#include "gameStructs.h"
#include <string>

//called when entering console to set the y padding
void resetConsole(GameWindowBuffer* window, Console* console);

void drawConsole(GameWindowBuffer* window, Console *console, GameInput *input, SerializedVariabels* serializedVars, int &reload);

void tokenizeCommand(std::string msg, Console* console, SerializedVariabels* serializedVars, int &reload);

void processCommand(std::string& firstToken, std::vector<std::string>& params, Console* console,
	SerializedVariabels* serializedVars, int &reload);

