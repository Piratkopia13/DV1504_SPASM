#include "MenuItem.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MenuItem::MenuItem()
{
	this->useColor = 1;
}

MenuItem::MenuItem(Model * model, Vector3 pos) : MenuItem()
{
	this->setModel(model);
	this->setPosition(pos);
}


void MenuItem::draw()
{
	this->getModel()->setTransform(&this->getTransform());
	if(this->useColor)
		this->getModel()->getMaterial()->setColor(this->lightColor);
	this->getModel()->draw();
}

MenuItem::~MenuItem()
{
}
