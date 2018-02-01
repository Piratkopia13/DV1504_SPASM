#pragma once

#include "common\Object.h"

class Block : public Object {
private:

public:
	Block();
	Block(Model *drawModel);
	virtual ~Block();

	virtual void draw();
};