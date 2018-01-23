#include "Block.h"

Block::Block() {
	m_drawModel = nullptr;
}

Block::Block(Model *drawModel) {
	m_drawModel = drawModel;
}

Block::~Block() {

}

void Block::setModel(Model *newModel) {
	m_drawModel = newModel;
}

void Block::draw() {
	m_drawModel->setTransform(&getTransformation());
	m_drawModel->draw();
}