#include "InstancedBlocks.h"
#include "../../sail/graphics/geometry/Material.h"

InstancedBlocks::InstancedBlocks(UINT maxInstances) {
	auto* app = Application::getInstance();

	// Store a pointer to the shader used in rendering
	m_shader = &app->getResourceManager().getShaderSet<DeferredInstancedGeometryShader>();

	init(maxInstances);
}

InstancedBlocks::InstancedBlocks() {
	auto* app = Application::getInstance();

	// Store a pointer to the shader used in rendering
	m_shader = &app->getResourceManager().getShaderSet<DeferredInstancedGeometryShader>();
}

InstancedBlocks::~InstancedBlocks() {
	delete model;
}

void InstancedBlocks::init(UINT maxInstances) {
	auto* app = Application::getInstance();
	// Use the data from the fbx version
	auto* blockModel = app->getResourceManager().getFBXModel("block").getModel();
	// Add instancing to the build data
	Model::Data buildData;
	buildData.deepCopy(blockModel->getBuildData());
	buildData.numInstances = maxInstances;
	// Create a new model
	model = new Model(buildData);
	model->buildBufferForShader(m_shader);
	// Steal textures from the fbx model
	UINT numTextures;
	model->getMaterial()->setDiffuseTexture(*blockModel->getMaterial()->getTextures(numTextures));

	if (m_instanceData.size() < maxInstances) {
		// Reserve memory to fit max particles
		m_instanceData.reserve(maxInstances);
	}
}


void InstancedBlocks::init() {
	init(m_instanceData.size());
}

void InstancedBlocks::reserve(UINT capacity) {
	m_instanceData.reserve(capacity);
}

void InstancedBlocks::draw() {

	m_shader->updateInstanceData(&m_instanceData[0], m_instanceData.size() * sizeof(m_instanceData[0]), getModel()->getInstanceBuffer());
	getModel()->draw();
	//m_shader->draw(*m_instancedModel, true, m_particles.size());

}

DeferredInstancedGeometryShader::InstanceData& InstancedBlocks::getInstanceData(UINT index) {
	return m_instanceData[index];
}

DeferredInstancedGeometryShader::InstanceData& InstancedBlocks::addInstance(const DeferredInstancedGeometryShader::InstanceData& instanceData) {
	//Logger::log(std::to_string(m_instanceData.size()));
	assert(m_instanceData.size() != m_instanceData.capacity());
	m_instanceData.push_back(instanceData);
	return m_instanceData.back();
}
