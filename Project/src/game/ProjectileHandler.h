#pragma once

#include "objects/Projectile.h"
#include "../sail/Sail.h"

class ProjectileHandler {
private:
	std::unique_ptr<FbxModel> m_projectileModel1;
	std::vector<Projectile> m_projectiles;
	
public:
	ProjectileHandler(DeferredRenderer& deferredRenderer);
	~ProjectileHandler();

	std::vector<Projectile>* getProjectiles();

	void addProjectile(Projectile newProjectile);
	void removeAt(int index);
	void update(float dt);
	void draw();
};