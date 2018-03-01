#pragma once

#include "common\Object.h"

class InstancedBlocks : public Object {
public:
	InstancedBlocks(UINT maxInstances);
	InstancedBlocks();
	virtual ~InstancedBlocks();

	virtual void draw();
	// Initializes the model with instance data that correlates to the provided parameter, also reserves memory for instance data
	void init(UINT maxInstances);
	void init();
	void reserve(UINT capacity);
	DeferredInstancedGeometryShader::InstanceData& getInstanceData(UINT index);
	DeferredInstancedGeometryShader::InstanceData& addInstance(const DeferredInstancedGeometryShader::InstanceData& instanceData);

private:
	std::vector<DeferredInstancedGeometryShader::InstanceData> m_instanceData;
	DeferredInstancedGeometryShader* m_shader;

};