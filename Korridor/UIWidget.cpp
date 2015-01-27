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

void UIHeader::setLabel(const char * text){
	this->text.assign(text);
}

void UIHeader::draw(Renderer * renderer,Uint32 x,Uint32 y){
	renderer->drawMessage(this->text.c_str(),x,y);
}