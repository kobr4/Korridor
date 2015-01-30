#pragma once

#include <vector>
#include <string>
#include <SDL.h>

class Renderer;
class UIWidget {
	static unsigned int elementSize;
protected :
	bool selected;
	bool active;
	std::vector<UIWidget *> childs;
public :
	UIWidget() {
		selected = false;
		active = true;
	}
	virtual void addChild(UIWidget * widget);
	void drawChilds(Renderer * renderer);
	void setSelected(bool b);
	bool isSelected();
	void setActive(bool b);
	bool isActive();
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y) = 0;
	virtual void handleEvent(SDL_Event event);
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