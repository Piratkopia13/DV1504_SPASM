#include "Block.h"

Block::Block() {
	m_drawModel = nullptr;
}

Block::Block(Model *drawModel) {
	m_drawModel = drawModel;
}

Block::~Block() {

}


void Block::draw() {
	m_drawModel->setTransform(&getTransform());
	m_drawModel->draw();
}