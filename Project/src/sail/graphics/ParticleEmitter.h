#pragma once

#include "shader/instanced/ParticleShader.h"
#include "Particle.h"

class ParticleEmitter {
public:
	enum Type {
		EXPLOSION
	};
	ParticleEmitter(Type type, const DirectX::SimpleMath::Vector3& emitPos, const DirectX::SimpleMath::Vector3& velocityVariety, float spawnsPerSecond, UINT maxParticles, float scale = 1.f,
		float lifetime = 10.f, const DirectX::SimpleMath::Vector4& color = DirectX::SimpleMath::Vector4::One, float gravityScale = 0.f, 
		const Camera* cam = nullptr, bool useAdditiveBlending = false, UINT spritesPerRow = 3.f, UINT spritesPerColumn = 3.f);
	~ParticleEmitter();

	void updateEmitPosition(const DirectX::SimpleMath::Vector3& emitPos);
	void update(float dt);
	void draw();

private:
	struct Compare {
		Compare(const ParticleEmitter& c) : myClass(c) {}
		bool operator()(const ParticleShader::InstanceData& i, const ParticleShader::InstanceData& j) {
			if (!myClass.m_cam)
				return false;
			return (DirectX::SimpleMath::Vector3::DistanceSquared(i.position, myClass.m_cam->getPosition()) > DirectX::SimpleMath::Vector3::DistanceSquared(j.position, myClass.m_cam->getPosition()));
		}
		const ParticleEmitter& myClass;
	};

	void insertionSort();
	void addParticle(Particle& particle);

private:
	std::vector<ParticleShader::InstanceData> m_instanceData;
	std::unique_ptr<Model> m_instancedModel;
	std::vector<Particle> m_particles;
	ParticleShader* m_shader;
	const Camera* m_cam;
	UINT m_spritesPerRow;
	UINT m_spritesPerColumn;
	bool m_useAdditiveBlending;

	// Particle settings
	DirectX::SimpleMath::Vector3 m_emitPosition;
	DirectX::SimpleMath::Vector3 m_velocityVariety;
	float m_spawnsPerSecond;
	float m_scale;
	float m_lifetime;
	float m_gravityScale;
	DirectX::SimpleMath::Vector4 m_color;

};