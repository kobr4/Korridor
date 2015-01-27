#pragma once

#include <vector>
#include <string>
#include <SDL.h>

class Renderer;
class UIWidget {
	static unsigned int elementSize;
protected :
	std::vector<UIWidget *> childs;
public :
	virtual void addChild(UIWidget * widget);
	void drawChilds(Renderer * renderer);
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y) = 0;
};

class UIHeader : public UIWidget {
protected :
	std::string text;	
public :
	virtual void setLabel(const char * text);
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y);
};

class UIBoolean : public UIHeader {
protected :
	std::string text;
public :
	void setState(bool b);
	bool getState();
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y);
};