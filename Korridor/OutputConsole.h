#pragma once


#include <stdlib.h>
class Renderer;
class OutputConsole {
private :
	bool logToFile;
	Renderer * renderer;
	static OutputConsole * outputConsole;
	OutputConsole();
	char sBuffer[4][1024];
public :
	static void setRenderer(Renderer * renderer);
	static void log(const char * message,...);
	static void render();
};