#include "MenuItem.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MenuItem::MenuItem()
{
	
}

MenuItem::MenuItem(Model * model, Vector3 pos) : MenuItem()
{
	this->setModel(model);
	this->setPosition(pos);
}


void MenuItem::draw()
{
	this->getModel()->setTransform(&this->getTransform());
	this->getModel()->draw();
}

MenuItem::~MenuItem()
{
}
