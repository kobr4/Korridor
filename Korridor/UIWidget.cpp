#include "UIWidget.h"
#include "Renderer.h"

unsigned int UIWidget::elementSize = 20; 

void UIWidget::addChild(UIWidget * widget){
	this->childs.push_back(widget);
}

void UIWidget::drawChilds(Renderer * renderer){
	unsigned int y = renderer->getScreenHeight() / 2 - (this->childs.size() * UIWidget::elementSize) / 2;
	for (int i = 0;i < this->childs.size();i++) {
		this->childs[i]->draw(renderer,renderer->getScreenWidth()/2,y + i * UIWidget::elementSize);
	}
}

void UIWidget::setSelected(bool b){
	this->selected = b;
}

bool UIWidget::isSelected(){
	return this->selected;
}

void UIWidget::setActive(bool b){
	this->active = b;
}

bool UIWidget::isActive(){
	return this->active;
}

void UIWidget::handleEvent(SDL_Event event) {
	//switch (event.type) 

	int selected = 0;

	if (this->childs.size() == 0) {
		return;
	}

	for (int i = 0;i < this->childs.size();i++) {
		if (this->childs[i]->isSelected()) {
			selected = i;
		}
	}

	switch (event.type)  {
		case SDL_KEYDOWN:
			switch( event.key.keysym.sym )
			{
				case SDLK_UP:
					this->childs[selected]->setSelected(false);
					selected = (++selected)%this->childs.size();
					this->childs[selected]->setSelected(true);
					break;
				case SDLK_DOWN:
					this->childs[selected]->setSelected(false);
					(--selected < 0) ? selected = selected + this->childs.size() : selected;
					this->childs[selected]->setSelected(true);
					break;
				case SDLK_LEFT:
					this->childs[selected]->handleEvent(event);
					break;
				case SDLK_RIGHT:
					this->childs[selected]->handleEvent(event);
					break;
				case SDLK_ESCAPE :
					this->setActive(false);
					break;
				default:
					break;
			}
		break;		
	}
}

void UIHeader::setLabel(const char * text){
	this->text.assign(text);
}

void UIHeader::draw(Renderer * renderer,Uint32 x,Uint32 y){
	std::string s;
	if (selected) {
		s = ">"+this->text;
	} else {
		s = this->text;
	}

	renderer->drawMessage(s.c_str(),x,y);
}