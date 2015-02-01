#pragma once

#include <vector>
#include <string>
#include <SDL.h>

class Renderer;

typedef void (uiwidget_func_cb)(void * data);
class UIWidget {

protected :
	static unsigned int elementSize;
	static unsigned int elementWidth;
	static unsigned int elementHeight;
	bool selected;
	bool active;
	std::vector<UIWidget *> childs;
	UIWidget * parent;
	uiwidget_func_cb * clickFuncCb;
	void * clickFuncCbData;
	unsigned int x;
	unsigned int y;
	unsigned int length;
public :
	UIWidget() {
		selected = false;
		active = true;
		clickFuncCb = NULL;
		parent = NULL;
		x = 0;
		y = 0;
	}
	virtual void addChild(UIWidget * widget);
	void setParent(UIWidget * widget);
	UIWidget * getParent();
	void drawChilds(Renderer * renderer);
	void setSelected(bool b);
	bool isSelected();
	void setActive(bool b);
	bool isActive();
	unsigned int getX() { return x; };
	unsigned int getY() { return y; };
	unsigned int getLength() { return length; };
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y) = 0;
	virtual void handleEvent(SDL_Event event);
	static UIWidget * currentWidget;
	virtual void onClick();
	void setOnClickCallback(uiwidget_func_cb * func, void * data);
};

class UIHeader : public UIWidget {
protected :
	std::string text;	
public :
	UIHeader();
	virtual void setLabel(const char * text);
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y);
	virtual void onClick();
};

class UIBoolean : public UIHeader {
protected :
	bool state;
public :
	UIBoolean():UIHeader(){};
	void setState(bool b);
	bool getState();
	virtual void draw(Renderer * renderer,Uint32 x,Uint32 y);
	virtual void onClick();
};