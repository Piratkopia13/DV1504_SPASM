#include "ParticleEmitter.h"
#include "geometry/factory/InstancedTestModel.h"
#include <algorithm>
#include <math.h>

using namespace DirectX;
using namespace SimpleMath;

ParticleEmitter::ParticleEmitter(Type type, const DirectX::SimpleMath::Vector3& emitPos, const Vector3& velocityVariety, float spawnsPerSecond, UINT maxParticles, float scale,
	float lifetime, const Vector4& color, float gravityScale, const Camera* cam, bool useAdditiveBlending, UINT spritesPerRow, UINT spritesPerColumn)
	: m_cam(cam)
	, m_spritesPerRow(spritesPerRow)
	, m_spritesPerColumn(spritesPerColumn)
	, m_useAdditiveBlending(useAdditiveBlending)
	, m_velocityVariety(velocityVariety)
	, m_spawnsPerSecond(spawnsPerSecond)
	, m_scale(scale)
	, m_color(color)
	, m_gravityScale(gravityScale)
	, m_lifetime(lifetime)
	, m_emitPosition(emitPos)
{

	auto* m_app = Application::getInstance();

	std::string particleSpritesheet;
	switch (type) {
	case ParticleEmitter::EXPLOSION:
		particleSpritesheet = "particles/explosion.tga";
		break;
	default:
		particleSpritesheet = "particles/explosion.tga";
		break;
	}

	m_app->getResourceManager().LoadDXTexture(particleSpritesheet);

	m_shader = &m_app->getResourceManager().getShaderSet<ParticleShader>();

	m_instancedModel = ModelFactory::InstancedTestModel::Create(maxParticles);
	m_instancedModel->buildBufferForShader(m_shader);
	m_instancedModel->getMaterial()->setDiffuseTexture(particleSpritesheet);

	m_instanceData.resize(maxParticles);

	//for (UINT i = 0; i < m_instancedModel->getNumInstances(); i++) {
	//	ParticleShader::InstanceData data;
	//	data.position = Vector3(Utils::rnd() * 5, Utils::rnd() * 5, Utils::rnd() * 5);
	//	data.color = Utils::getRandomColor();
	//	data.blendFactor = 0.5f;
	//	data.textureOffset1 = Vector2(0.f);
	//	data.textureOffset2 = Vector2(0.f);
	//	m_instanceData.push_back(data);

	//	//m_particles.push_back(Particle(data.position, Vector3(Utils::rnd() - 0.5f, Utils::rnd() - 0.5f, Utils::rnd() - 0.5f), 1.f, 0.f, 10.f));
	//	m_particles.push_back(Particle(data.position, Vector3::Zero, 1.f, 0.f, 10.f));
	//}

	//m_shader->updateInstanceData(&m_instanceData[0], m_instanceData.size() * sizeof(m_instanceData[0]), m_instancedModel->getInstanceBuffer());

}

ParticleEmitter::~ParticleEmitter() {
}

void ParticleEmitter::update(float dt) {

	static float spawnTimer = 0.f;
	spawnTimer += dt;
	if (spawnTimer >= 1.f / m_spawnsPerSecond) {
		while (spawnTimer > 0.f) {
			// Spawn a new particle
			addParticle(Particle(m_emitPosition, 
				Vector3((Utils::rnd() - 0.5f) * m_velocityVariety.x, (Utils::rnd() /*- 0.5f*/) * m_velocityVariety.y, (Utils::rnd() - 0.5f) * m_velocityVariety.z),
				1.f, m_gravityScale, m_lifetime));
			spawnTimer -= 1.f / m_spawnsPerSecond;
		}
		spawnTimer = 0.f;
	}

	int i = 0;
	auto& it = m_particles.begin();
	while (it != m_particles.end()) {

		Particle& particle = *it;

		if (particle.isDead()) {
			it = m_particles.erase(it);
		} else {

			auto& data = m_instanceData[i];
			particle.update(dt);
			data.position = particle.getPosition();

			// Update texture offsets
			float lifePercent = particle.getLifePercentage();
			UINT numSprites = m_spritesPerColumn * m_spritesPerRow;
			UINT spriteNum = (UINT)floorf(lifePercent * (float)numSprites);
			spriteNum = (spriteNum == numSprites) ? numSprites - 1 : spriteNum;

			data.textureOffset1.x = fmod(spriteNum / (float)m_spritesPerRow, 1.f);
			data.textureOffset1.y = (spriteNum / m_spritesPerRow) / (float)m_spritesPerRow;

			(spriteNum < numSprites - 1) ? spriteNum++ : spriteNum = spriteNum;
			data.textureOffset2.x = fmod(spriteNum / (float)m_spritesPerRow, 1.f);
			data.textureOffset2.y = (spriteNum / m_spritesPerRow) / (float)m_spritesPerRow;
			data.color = m_color;
			data.color.w = 1.f - lifePercent;

			// Update blend factor
			data.blendFactor = fmod(lifePercent * numSprites, 1.f);
			i++;

			++it;
		}
	}

	// Only sort if neccessary
	if (!m_useAdditiveBlending)
		// TOOD: try different sorting algorithms
		std::sort(m_instanceData.begin(), m_instanceData.begin() + m_particles.size(), Compare(*this));
	//insertionSort();

}

void ParticleEmitter::insertionSort() {
	unsigned int i, j;
	ParticleShader::InstanceData key;
	for (i = 1; i < m_instanceData.size(); i++) {
		key = m_instanceData[i];
		j = i - 1;

		while (j >= 0 && Vector3::DistanceSquared(m_instanceData[j].position, m_cam->getPosition()) < Vector3::DistanceSquared(key.position, m_cam->getPosition())) {
			m_instanceData[j + 1] = m_instanceData[j];
			j = j - 1;
		}
		m_instanceData[j + 1] = key;
	}
}

void ParticleEmitter::addParticle(Particle& particle) {
	if (m_particles.size() < m_instanceData.size())
		m_particles.push_back(particle);
}

void ParticleEmitter::updateEmitPosition(const DirectX::SimpleMath::Vector3& emitPos) {
	m_emitPosition = emitPos;
}


void ParticleEmitter::draw() {
	m_shader->updateInstanceData(&m_instanceData[0], m_instanceData.size() * sizeof(m_instanceData[0]), m_instancedModel->getInstanceBuffer());
	if (m_useAdditiveBlending)
		Application::getInstance()->getDXManager()->enableAdditiveBlending();
	else
		Application::getInstance()->getDXManager()->enableAlphaBlending();
	m_shader->draw(*m_instancedModel, true, m_particles.size());
}
