#pragma once

#include "objects/Projectile.h"
#include "../sail/Sail.h"

class ProjectileHandler {
public:
	ProjectileHandler();
	~ProjectileHandler();

	void addProjectile(Projectile* newProjectile);
	std::vector<Projectile*>& getProjectiles();
	void removeAt(int index);
	void update(float dt);
	void draw();

private:
	Model* m_projectileModel;
	std::vector<Projectile*> m_projectiles;
	std::vector<float> m_projectileLifeSpan;
};