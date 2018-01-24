#pragma once

#include "common\Object.h"

class Block : public Object {
private:
	Model *m_drawModel;

public:
	Block();
	Block(Model *drawModel);
	virtual ~Block();

	void setModel(Model* newModel);

	virtual void draw();
};