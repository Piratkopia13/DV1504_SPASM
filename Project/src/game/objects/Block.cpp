#include "Block.h"

Block::Block() {
}

Block::Block(Model *drawModel) {
	setModel(drawModel);
}

Block::~Block() {

}

void Block::draw() {
	getModel()->setTransform(&getTransform());
	getModel()->draw();
}