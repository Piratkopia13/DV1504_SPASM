#include "MenuHandler.h"


using namespace DirectX::SimpleMath;

MenuHandler::MenuHandler() {
	m_offset = 0.12f;
	m_size = 1.0f;
	m_activeItem = 0;
	m_active = false;
	m_position = Vector3(0, 0, 0);
	m_growth = Vector3(0, -1.0f, 0);
	m_direction = Vector3(0, 0, -1.0f);
	m_onColor = Vector4(1, 1, 1, 1);
	m_offColor = Vector4(0.4f, 0.4f, 0.4f, 1);

	m_crazy = false;
	updateTransform();
}


MenuHandler::~MenuHandler() {

	for (size_t i = 0; i < m_itemList.size(); i++) {
		Memory::safeDelete(m_itemList[i].item);
		Memory::safeDelete(m_itemList[i].text);
		Memory::safeDelete(m_itemList[i].selector);
	}
}

void MenuHandler::draw() {
	for (size_t i = 0; i < m_itemList.size(); i++) {
		if (m_itemList[i].item) {
			m_itemList[i].item->draw();
		}
		if (m_itemList[i].text) {
			m_itemList[i].text->draw();
		}
		if (m_itemList[i].selector) {
			m_itemList[i].selector->draw();
		}
	}
}

void MenuHandler::update(float dt) {
	if (m_crazy) {
		// wtf
	}


}

void MenuHandler::setPosition(const DirectX::SimpleMath::Vector3 & position) {
	m_position = position;
	Object::setPosition(position);
	updateTransform();
}

void MenuHandler::setGrowth(const DirectX::SimpleMath::Vector3 & growth) {
	m_growth = growth;
	updateTransform();
}

void MenuHandler::setFacingDirection(const DirectX::SimpleMath::Vector3 & direction) {
	m_direction = direction;
	updateTransform();
}

void MenuHandler::setOnColor(const DirectX::SimpleMath::Vector4 & color) {
	m_onColor = color;
}

void MenuHandler::setOffColor(const DirectX::SimpleMath::Vector4 & color) {
	m_offColor = color;
	for (size_t i = 0; i < m_itemList.size(); i++) {
		m_itemList[i].item->setLightColor(color);

		if (m_itemList[m_activeItem].text)
			m_itemList[i].text->setColor(color);
	}
}

void MenuHandler::setStaticSelection(bool active, size_t sides) {
	if(m_itemList.back().selector)
		m_itemList.back().selector->setStaticSelection(active, sides);
}

void MenuHandler::addMenuBox(std::string text) {
	MenuIndex temp;

	temp.item = new MenuItem(Application::getInstance()->getResourceManager().getFBXModel("buttons/emptyButtonFlat").getModel(), Vector3(0,0,0));
	temp.item->setLightColor(m_offColor);
	if (text.size() > 0) {
		temp.text = new MenuText(text);
		temp.text->setColor(m_offColor);
	}
	else 
		temp.text = nullptr;
	temp.selector = nullptr;

	temp.editable = false;

	m_itemList.push_back(temp);
	updateTransform();
}

void MenuHandler::addTextItem(std::string text) {

}

void MenuHandler::addMenuSelector(std::string text) {
	MenuIndex temp;

	temp.item = nullptr;
	if (text.size() > 0) {
		temp.text = new MenuText(text);
		temp.text->setColor(m_offColor);
	}
	else
		temp.text = nullptr;
	temp.selector = new MenuSelector();
	temp.editable = false;

	m_itemList.push_back(temp);
	updateTransform();
}

void MenuHandler::addMenuSelectorItem(std::string text) {
	if (m_itemList.back().selector) {
		m_itemList.back().selector->addOption(text);
	}
}

void MenuHandler::addMenuSlider(std::string text, int min, int max) {

}

void MenuHandler::next() {
	MenuItem* item = m_itemList[m_activeItem].item;
	MenuText* text = m_itemList[m_activeItem].text;
	MenuSelector* selector = m_itemList[m_activeItem].selector;

	if(item)
		item->setLightColor(m_offColor);
	if(text)
		m_itemList[m_activeItem].text->setColor(m_offColor);

	m_activeItem++;
	if (m_activeItem == m_itemList.size())
		m_activeItem = 0;

	item = m_itemList[m_activeItem].item;
	text = m_itemList[m_activeItem].text;
	selector = m_itemList[m_activeItem].selector;

	if(item)
		item->setLightColor(m_onColor);
	if (text)
		text->setColor(m_onColor);



}

void MenuHandler::back() {
	MenuItem* item = m_itemList[m_activeItem].item;
	MenuText* text = m_itemList[m_activeItem].text;
	MenuSelector* selector = m_itemList[m_activeItem].selector;

	if(item)
		item->setLightColor(m_offColor);
	if (text)
		text->setColor(m_offColor);


	if (m_activeItem == 0) {
		m_activeItem = m_itemList.size() - 1;
	}
	else {
		m_activeItem--;
	}

	item = m_itemList[m_activeItem].item;
	text = m_itemList[m_activeItem].text;
	selector = m_itemList[m_activeItem].selector;

	if(item)
		item->setLightColor(m_onColor);
	if (text)
		text->setColor(m_onColor);


}

void MenuHandler::right() {
	if (m_itemList[m_activeItem].selector) {
		m_itemList[m_activeItem].selector->next();
	}


}

void MenuHandler::left() {
	if (m_itemList[m_activeItem].selector) {
		m_itemList[m_activeItem].selector->back();
	}
}

void MenuHandler::up() {
	if (m_itemList[m_activeItem].editable) {

	}
}

void MenuHandler::down() {
	if (m_itemList[m_activeItem].editable) {

	}
}





size_t MenuHandler::getActive()
{
	return m_activeItem;
}

size_t MenuHandler::getActiveOption()
{
	if (m_itemList[m_activeItem].selector) {
		return m_itemList[m_activeItem].selector->getOption();
	}
	return 0;
}

size_t MenuHandler::getOptionAt(size_t index)
{
	if (m_itemList[index].selector) {
		return m_itemList[index].selector->getOption();
	}
	return 0;
}


Object* MenuHandler::getTarget()
{
	if (m_itemList[m_activeItem].selector) {
		return m_itemList[m_activeItem].text;
	}
	else if(m_itemList[m_activeItem].item)
		return m_itemList[m_activeItem].item;
	else 
		return nullptr;
}

Object * MenuHandler::getExtraTarget() {
	if (m_itemList[m_activeItem].selector) {
		return m_itemList[m_activeItem].selector->getTarget();
	}
	else 
		return nullptr;
}

void MenuHandler::activate() {
	m_active = true;
	if(m_itemList[m_activeItem].item)
		m_itemList[m_activeItem].item->setLightColor(m_onColor);
	if (m_itemList[m_activeItem].text)
		m_itemList[m_activeItem].text->setColor(m_onColor);
}

void MenuHandler::deActivate() {
	m_active = false;
	if(m_itemList[m_activeItem].item)
		m_itemList[m_activeItem].item->setLightColor(m_offColor);
	if (m_itemList[m_activeItem].text)
		m_itemList[m_activeItem].text->setColor(m_offColor);
}

bool MenuHandler::isActive() {

	return m_active;
}

void MenuHandler::crazy(bool watafaq) {
	m_crazy = watafaq;
}

void MenuHandler::updateTransform() {
	Vector3 extraHeight(0,0,0);

	for (size_t i = 0; i < m_itemList.size(); i++) {
		if (m_itemList[i].item) {
			Vector3 position = m_position + m_growth * ((float)i - (float)m_itemList.size()*0.5f) * (m_size + m_offset);
			Vector3 right = m_growth.Cross(m_direction);
			m_itemList[i].item->setPosition(position + extraHeight);
			m_itemList[i].item->getTransform().setRotations(Vector3(0, -atan2(m_direction.z, m_direction.x) + 1.55f, 0));
			if (m_itemList[m_activeItem].text) {
				m_itemList[i].text->setPosition(position + m_direction*m_size*0.2f - m_growth*0.3f*m_size + extraHeight);
				m_itemList[i].text->setDirection(right);
				m_itemList[i].text->setSize(m_size*4);
				m_itemList[i].text->setFacingDirection(m_direction);

			}
		}
		if (m_itemList[i].selector) {

			Vector3 position = m_position + m_growth * ((float)i - (float)m_itemList.size()*0.5f)*(m_size + m_offset);
			Vector3 right = m_growth.Cross(m_direction);
			m_itemList[i].selector->setPosition(position + extraHeight);
			m_itemList[i].selector->setSize(m_size*3);
			m_itemList[i].selector->setgrowth(right);
			m_itemList[i].selector->setDirection(m_direction);
			extraHeight += m_growth * 0.1f;

			if (m_itemList[i].text) {
				m_itemList[i].text->setPosition(position - m_growth * 0.4f*m_size + extraHeight);
				m_itemList[i].text->setDirection(right);
				m_itemList[i].text->setSize(m_size*3);
				m_itemList[i].text->setFacingDirection(m_direction);

			}



		}
	}

}
