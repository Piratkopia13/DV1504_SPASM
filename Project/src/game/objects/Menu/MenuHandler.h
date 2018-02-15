#pragma once
#include "../common/Object.h"
#include "MenuItem.h"
#include "MenuText.h"
#include "MenuSelector.h"


class MenuHandler :
	public Object
{
public:
	MenuHandler();
	~MenuHandler();
	void draw();
	void update(float dt);

	void setPosition(const DirectX::SimpleMath::Vector3& position);
	void setGrowth(const DirectX::SimpleMath::Vector3& growth);
	void setFacingDirection(const DirectX::SimpleMath::Vector3& direction);
	void setOnColor(const DirectX::SimpleMath::Vector4& color);
	void setOffColor(const DirectX::SimpleMath::Vector4& color);
	void setStaticSelection(bool active, size_t sides);

	void addMenuBox(std::string text);
	void addTextItem(std::string text);
	void addMenuSelector(std::string text);
	void addMenuSelectorItem(std::string text);
	void addMenuSlider(std::string text, int min, int max);
	
	void next();
	void back();
	void right();
	void left();
	void up();
	void down();

	size_t getActive();
	size_t getActiveOption();
	size_t getOptionAt(size_t index);
	Object* getTarget();
	Object* getExtraTarget();


	void activate();
	void deActivate();
	bool isActive();

	void crazy(bool watafaq);

private:
	struct MenuIndex {
		MenuItem* item;
		MenuSelector* selector;
		MenuText* text;
		bool editable;
	};


	size_t m_activeItem;
	bool m_active;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_growth;
	DirectX::SimpleMath::Vector3 m_direction;
	DirectX::SimpleMath::Vector3 m_rotation;
	float m_offset;
	float m_size;

	DirectX::SimpleMath::Vector4 m_onColor;
	DirectX::SimpleMath::Vector4 m_offColor;

	std::vector<MenuIndex> m_itemList;

	bool m_crazy;


	void updateTransform();

};

