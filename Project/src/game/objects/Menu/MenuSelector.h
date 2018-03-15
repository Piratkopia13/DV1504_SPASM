#pragma once
#include "../common/Object.h"
#include "MenuText.h"
class MenuSelector :
	public Object
{
public:
	MenuSelector();
	~MenuSelector();
	void draw();
	void update();


	void addOption(std::string text);
	size_t getOption();
	Object* getTarget();
	void setSelector(size_t selection);
	void setStaticSelection(bool stat, size_t show = 0);
	void setPosition(const DirectX::SimpleMath::Vector3& position);
	void setgrowth(const DirectX::SimpleMath::Vector3& growth);
	void setDirection(const DirectX::SimpleMath::Vector3& direction);
	void setSize(float size);
	void setActiveOption(size_t option);
	void setSelectionColor(const DirectX::SimpleMath::Vector4& color);


	void next();
	void back();
	



private:

	std::vector<MenuText*> m_options;
	size_t m_selector;

	DirectX::SimpleMath::Vector4 m_onColor;
	DirectX::SimpleMath::Vector4 m_selectionColor;
	DirectX::SimpleMath::Vector4 m_offColor;

	bool m_stat;
	size_t m_statShow;
	float m_size;
	float m_targetSize;
	float m_offset;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_growth;
	DirectX::SimpleMath::Vector3 m_facing;


private:
	void updatePositions();
};

