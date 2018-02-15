#pragma once

#include "../sail/Sail.h"

class ParticleHandler {
public:
	ParticleHandler(const Camera* cam);
	~ParticleHandler();

	void addEmitter(std::shared_ptr<ParticleEmitter>& emitter);
	void removeEmitter(std::shared_ptr<ParticleEmitter>& emitter);
	void update(float dt);
	void draw();
	UINT getParticleCount() const;

private:
	const Camera* m_cam;
	std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;

};