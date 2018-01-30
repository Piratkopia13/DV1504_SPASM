#pragma once

#include "Projectile.h"
#include "../../sail/Sail.h"
#include "common/Object.h"

class Hook : public Object{
private:
	std::unique_ptr<FbxModel> m_hookModel;

public:
	Hook(Model *drawModel);
	~Hook();

	void update(float dt);

	void draw();

};