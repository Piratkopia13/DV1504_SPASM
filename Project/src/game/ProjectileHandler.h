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
	void projectileHitLevel(const DirectX::SimpleMath::Vector3& hitPos, Projectile* proj);
	void projectileHitSomething(Projectile* proj, const DirectX::SimpleMath::Vector3& hitPos, float dt);

private:
	void spawnTracer(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float dt, unsigned int team);

private:
	ParticleHandler* m_particleHandler;
	std::vector<std::shared_ptr<ParticleEmitter>> m_traceEmitter;
	Model* m_projectileModel;
	std::vector<Projectile*> m_projectiles;
	std::vector<float> m_projectileLifeSpan;
};