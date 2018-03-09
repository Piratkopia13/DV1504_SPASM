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
	void reset();

	void setPosition(const DirectX::SimpleMath::Vector3& position);
	void setGrowth(const DirectX::SimpleMath::Vector3& growth);
	void setFacingDirection(const DirectX::SimpleMath::Vector3& direction);
	void setSize(float size);
	void setOnColor(const DirectX::SimpleMath::Vector4& color);
	void setOffColor(const DirectX::SimpleMath::Vector4& color);
	void setStaticSelection(bool active, size_t sides);
	void setStep(float step);
	void setOptionAt(size_t index, size_t option);
	void editing(bool active);
	bool getEditing();
	void addMenuBox(std::string text);
	void addTextItem(std::string text, std::string def = "");
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

	std::string getEditorText();

	void activate();
	void deActivate();
	bool isActive();

	void crazy(bool watafaq);

private:
	struct MenuIndex {
		MenuItem* item;
		MenuSelector* selector;
		MenuText* text;
		MenuText* extraText;
		bool editable;
		size_t index;
	};
	MenuItem* m_pointers[2];

	size_t m_activeItem;
	bool m_active;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_growth;
	DirectX::SimpleMath::Vector3 m_direction;
	DirectX::SimpleMath::Vector3 m_rotation;
	float m_offset;
	float m_size;
	float m_step;

	DirectX::SimpleMath::Vector4 m_onColor;
	DirectX::SimpleMath::Vector4 m_offColor;

	std::vector<MenuIndex> m_itemList;

	bool m_crazy;
	float m_crazyAcc;
	float m_crazyThresh;

	struct Animation {
		bool active = 0;
		float elapsed = 0.0f;
		float endTime = 1.0f;
		float div = 1.0f / endTime;
		void update(float dt) {
			if (active) {
				elapsed += dt;
				if (elapsed >= endTime) {
					active = false;
					elapsed = 0;

				}
			}
		}
		float setMax(float end) {
			endTime = max(end, 0.1f);
			div = 1.0f / endTime;
		}
		float interp() {
			return elapsed * div;
		}
	};
	Animation m_colorChange;

	void updateTransform();
	DirectX::SimpleMath::Vector4 interpolateColor(float progress ,const DirectX::SimpleMath::Vector4& from, const DirectX::SimpleMath::Vector4& to);

};

