#pragma once

#include "objects/Projectile.h"
#include "../sail/Sail.h"

class ProjectileHandler {
public:
	ProjectileHandler(ParticleHandler* particleHandler);
	~ProjectileHandler();

	void addProjectile(Projectile* newProjectile);
	std::vector<Projectile*>& getProjectiles();
	void removeAt(int index);
	void update(float dt);
	void draw();

private:
	ParticleHandler* m_particleHandler;
	Model* m_projectileModel;
	std::vector<Projectile*> m_projectiles;
	std::vector<float> m_projectileLifeSpan;
};