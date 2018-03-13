#include "MenuHandler.h"
#include "../../../sail/graphics/shader/deferred/DynBlockDeferredInstancedGeometryShader.h"

using namespace DirectX::SimpleMath;

MenuHandler::MenuHandler() {
	m_offset = 0.12f;
	m_size = 1.0f;
	m_step = 0;
	m_activeItem = 0;
	m_active = false;
	m_visible = false;
	m_position = Vector3(0, 0, 0);
	m_growth = Vector3(0, -1.0f, 0);
	m_direction = Vector3(0, 0, -1.0f);
	m_onColor = Vector4(0.9f, 0.9f, 0.9f, 1);
	m_offColor = Vector4(0.4f, 0.4f, 0.4f, 1);
	//m_targetColor = Vector4(1, 1, 1, 1);
	m_currentColor = Vector4(0, 0, 0, 1);



	m_crazy = false;
	m_crazyAcc = 0;
	m_crazyThresh = 1.0f / 3.0f;

	m_pointers[0] = new MenuItem(Application::getInstance()->getResourceManager().getFBXModel("arrow").getModel(),Vector3(0,0,0));
	m_pointers[0]->setLightColor(m_onColor);
	m_pointers[0]->getTransform().setRotations(Vector3(0, 0,1.57f));


	m_pointers[1] = new MenuItem(Application::getInstance()->getResourceManager().getFBXModel("arrow").getModel(),Vector3(0,0,0));
	m_pointers[1]->setLightColor(m_onColor);
	m_pointers[1]->getTransform().setRotations(Vector3(3.14f, 0, -1.57f));
	updateTransform();
}
 

MenuHandler::~MenuHandler() {

	for (size_t i = 0; i < m_itemList.size(); i++) {
		Memory::safeDelete(m_itemList[i].item);
		Memory::safeDelete(m_itemList[i].text);
		Memory::safeDelete(m_itemList[i].selector);
		Memory::safeDelete(m_itemList[i].extraText);
	}
	Memory::safeDelete(m_pointers[0]);
	Memory::safeDelete(m_pointers[1]);
}

void MenuHandler::draw() {
	if (m_visible) {
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
			if (m_itemList[i].extraText){
				m_itemList[i].extraText->draw();
				if (m_itemList[i].editable) {
					if (m_pointers[0])
						m_pointers[0]->draw();
					if (m_pointers[1])
						m_pointers[1]->draw();
				}
			
			}
		}
		
	}
	
}

void MenuHandler::update(float dt) {

	Vector4 bright = m_onColor;
	Vector4 dim = m_offColor;

	if (!m_active) {
		bright = Vector4(0, 0, 0, 0);
		dim = Vector4(0, 0, 0, 0);
	}


	if (m_colorChange.active) {
		if (m_active) {

			bright = interpolateColor(m_colorChange.interp(), Vector4(0,0,0,1), m_onColor);
			dim = interpolateColor(m_colorChange.interp(), Vector4(0,0,0,1), m_offColor);


		}
		else {
			bright = interpolateColor(m_colorChange.interp(), m_onColor, Vector4(0, 0, 0, 1));
			dim = interpolateColor(m_colorChange.interp(), m_offColor, Vector4(0, 0, 0, 1));


		}

		m_colorChange.update(dt);





		
	}

	for (size_t i = 0; i < m_itemList.size(); i++) {
		MenuIndex* index = &m_itemList[i];
		if (index->text) {
			index->text->setColor(i == m_activeItem ? bright : dim);
		}
		if (index->selector) {
			index->selector->setLightColor(i == m_activeItem ? bright : dim);
			index->selector->setSelectionColor(bright);
		}
		if (index->extraText) {
			index->extraText->setColor(bright);
			m_pointers[0]->setLightColor(bright);
			m_pointers[1]->setLightColor(bright);


		}
	
	}
	if (!m_colorChange.active && !m_active)
		m_visible = false;


}

void MenuHandler::reset() {
	for (size_t i = 0; i < m_itemList.size(); i++) {
		Memory::safeDelete(m_itemList[i].item);
		Memory::safeDelete(m_itemList[i].text);
		Memory::safeDelete(m_itemList[i].selector);
	}
	m_itemList.clear();
	m_activeItem = 0;

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
	this->getTransform().setRotations(Vector3(0, -atan2(m_direction.z, m_direction.x), 0));
	updateTransform();
}

void MenuHandler::setSize(float size) {
	m_size = size;
	updateTransform();
}



void MenuHandler::setOnColor(const DirectX::SimpleMath::Vector4 & color) {
	m_onColor = color;
}

void MenuHandler::setOffColor(const DirectX::SimpleMath::Vector4 & color) {
	m_offColor = color;
	for (size_t i = 0; i < m_itemList.size(); i++) {
		if(m_itemList[i].item)
			m_itemList[i].item->setLightColor(color);

		if (m_itemList[i].text)
			m_itemList[i].text->setColor(color);
	}
}

void MenuHandler::setStaticSelection(bool active, size_t sides) {
	if(m_itemList.back().selector)
		m_itemList.back().selector->setStaticSelection(active, sides);
}

void MenuHandler::setStep(float step) {
	m_step = step;
	updateTransform();
}

void MenuHandler::setOptionAt(size_t index, size_t option) {
	if(index < m_itemList.size())
		if (m_itemList[index].selector) {
			m_itemList[index].selector->setActiveOption(option);



		}

}

void MenuHandler::editing(bool active) {
	if (m_itemList[m_activeItem].extraText) {
		m_itemList[m_activeItem].editable = active;
		updateTransform();
	}
}

bool MenuHandler::getEditing()
{
	return m_itemList[m_activeItem].editable;
}

void MenuHandler::addMenuBox(std::string text) {
	MenuIndex temp;

	//temp.item = new MenuItem(Application::getInstance()->getResourceManager().getFBXModel("buttons/emptyButtonFlat").getModel(), Vector3(0,0,0));
	temp.item = new MenuItem(nullptr, Vector3(0,0,0));
	temp.item->setLightColor(m_offColor);
	//temp.item = nullptr;
	

	if (text.size() > 0) {
		temp.text = new MenuText(text);
		temp.text->setColor(m_offColor);
	}
	else 
		temp.text = nullptr;
	temp.selector = nullptr;
	temp.extraText = nullptr;
	temp.editable = false;
	temp.index = 0;

	m_itemList.push_back(temp);
	updateTransform();
}

void MenuHandler::addTextItem(std::string text, std::string def) {
	MenuIndex temp;

	temp.item = nullptr;
	temp.selector = nullptr;
	temp.text = new MenuText(text);
	temp.extraText = new MenuText(def.substr(0,8));
	temp.editable = false;
	temp.index = 0;
	
	m_itemList.push_back(temp);
	updateTransform();
}

void MenuHandler::addMenuSelector(std::string text) {
	MenuIndex temp;

	temp.item = nullptr;
	temp.selector = new MenuSelector();
	if (text.size() > 0) {
		temp.text = new MenuText(text);
		temp.text->setColor(m_offColor);
	}
	else
		temp.text = nullptr;
	temp.extraText = nullptr;
	temp.editable = false;
	temp.index = 0;

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

	if (item) {
		item->setLightColor(m_offColor);
		// Sound
		Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffectWithRndPitch(SoundManager::SoundEffect::Switch, 0.95f, 1.05f, 3.f);
	}
	if(text) {
		text->setColor(m_offColor);
		// Sound
		Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffectWithRndPitch(SoundManager::SoundEffect::Switch, 0.95f, 1.05f, 3.f);
	}
	if (selector)
		selector->setSelectionColor(m_onColor*0.7f);

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
	if (selector)
		selector->setSelectionColor(m_onColor);
	updateTransform();


}

void MenuHandler::back() {
	MenuItem* item = m_itemList[m_activeItem].item;
	MenuText* text = m_itemList[m_activeItem].text;
	MenuSelector* selector = m_itemList[m_activeItem].selector;

	if(item) {
		item->setLightColor(m_offColor);
		// Sound
		Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffectWithRndPitch(SoundManager::SoundEffect::Switch, 0.95f, 1.05f, 3.f);
	}
	if (text) {
		text->setColor(m_offColor);
		// Sound
		Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffectWithRndPitch(SoundManager::SoundEffect::Switch, 0.95f, 1.05f, 3.f);
	}
	if (selector)
		selector->setSelectionColor(m_onColor*0.7f);

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
	if (selector)
		selector->setSelectionColor(m_onColor);

	updateTransform();
}

void MenuHandler::right() {
	MenuIndex* active = &m_itemList[m_activeItem];

	if (active->selector) {
		active->selector->next();
	}
	if (active->editable) {
		active->index++;
		if (active->index >= active->extraText->getLetterNr()) {
			active->index = 0;
		}
		
	}
	updateTransform();
}

void MenuHandler::left() {
	MenuIndex* active = &m_itemList[m_activeItem];

	if (active->selector) {
		active->selector->back();
	}
	if (active->editable) {
		if (active->index == 0) {
			active->index = active->extraText->getLetterNr()-1;
		}
		else
			active->index--;
	}
	updateTransform();
}

void MenuHandler::up() {
	MenuIndex* active = &m_itemList[m_activeItem];
	if (active->editable) {
		active->extraText->changeLetter(active->index,true);


	}
}

void MenuHandler::down() {
	MenuIndex* active = &m_itemList[m_activeItem];
	if (active->editable) {
		active->extraText->changeLetter(active->index, false);



	}
}





size_t MenuHandler::getActive() {
	return m_activeItem;
}

size_t MenuHandler::getActiveOption() {
	if (m_itemList[m_activeItem].selector) {
		return m_itemList[m_activeItem].selector->getOption();
	}
	return 0;
}

size_t MenuHandler::getOptionAt(size_t index) {
	if (m_itemList[index].selector) {
		return m_itemList[index].selector->getOption();
	}
	return 0;
}


Object* MenuHandler::getTarget() {
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

std::string MenuHandler::getEditorText() {
	if (m_itemList[m_activeItem].editable) 
		return m_itemList[m_activeItem].extraText->getString();
	return "";
}



void MenuHandler::activate() {
	m_active = true;
	m_visible = true;
	m_colorChange.active = true;


}

void MenuHandler::deActivate() {
	m_active = false;
	m_colorChange.active = true;
	
}

bool MenuHandler::isActive() {

	return m_active;
}

void MenuHandler::crazy(bool watafaq) {
	m_crazy = watafaq;
}

void MenuHandler::updateTransform() {
	Vector3 extraHeight(0,0,0);
	Vector3 selectionSpace(0,0,0);


	for (size_t i = 0; i < m_itemList.size(); i++) {
		if ((int)((int)i - (int)m_activeItem) > 0) {
			selectionSpace = m_growth * m_size * 0.1f;
		}
		if ((int)((int)i - (int)m_activeItem) == 0) {
			selectionSpace = Vector3(0,0,0);
		}
		if ((int)((int)i - (int)m_activeItem) < 0) {
			selectionSpace = m_growth * -1 * m_size * 0.1f;
		}

		if (m_itemList[i].item) {
			Vector3 position = m_position + m_growth * ((float)i - (float)(m_itemList.size()-1)*0.5f) * (m_size + m_offset);
			position += selectionSpace;
			Vector3 growth = m_growth;
			growth.Normalize();
			Vector3 right = growth.Cross(m_direction);
			m_itemList[i].item->setPosition(position + extraHeight);
			m_itemList[i].item->getTransform().setRotations(Vector3(0, -atan2(m_direction.z, m_direction.x) + 1.55f, 0));
			if (m_itemList[m_activeItem].text) {
				m_itemList[i].text->setPosition(position + m_direction*m_size*0.2f - m_growth*0.1f*m_size + extraHeight);
				m_itemList[i].text->setDirection(right);
				m_itemList[i].text->setSize(m_size*4);
				m_itemList[i].text->setFacingDirection(m_direction);

			}
		}
		if (m_itemList[i].selector) {

			Vector3 position = m_position + m_growth * ((float)i - (float)(m_itemList.size()-1)*0.5f)*(m_size*0.7f + m_offset);
			position += selectionSpace;
			Vector3 right = m_growth.Cross(m_direction);
			m_itemList[i].selector->setPosition(position + extraHeight);
			m_itemList[i].selector->setSize(m_size*3);
			m_itemList[i].selector->setgrowth(right);
			m_itemList[i].selector->setDirection(m_direction);

			if (m_itemList[i].text) {
				m_itemList[i].text->setPosition(position - m_growth * 0.3f*m_size + extraHeight);
				m_itemList[i].text->setDirection(right);
				m_itemList[i].text->setSize(m_size*3);
				m_itemList[i].text->setFacingDirection(m_direction);

			}
			extraHeight += m_growth * m_step;



		}
		if (m_itemList[i].extraText) {
			Vector3 position = m_position + m_growth * ((float)i - (float)(m_itemList.size() - 1)*0.5f)*(m_size*0.7f + m_offset);
			position += selectionSpace;
			Vector3 right = m_growth.Cross(m_direction);

			m_itemList[i].extraText->setPosition(position + extraHeight + m_growth*m_size*0.1f);
			m_itemList[i].extraText->setSize(m_size * 2);
			m_itemList[i].extraText->setDirection(right);
			m_itemList[i].extraText->setFacingDirection(m_direction);
			if (m_itemList[i].editable) {
				
				Vector3 textHeight(0, 0.3f*m_size, 0);
				m_pointers[0]->setPosition(position+textHeight + right * (((float)m_itemList[i].index - 3.5f) * 0.2f*m_size) + m_growth * m_size*0.1f);
				m_pointers[1]->setPosition(position-textHeight*0.5 + right * (((float)m_itemList[i].index - 3.5f) * 0.2f*m_size) + m_growth * m_size*0.1f);

			}



			if (m_itemList[i].text) {
				m_itemList[i].text->setPosition(position - m_growth * 0.3f*m_size + extraHeight);
				m_itemList[i].text->setDirection(right);
				m_itemList[i].text->setSize(m_size * 3);
				m_itemList[i].text->setFacingDirection(m_direction);

			}
			extraHeight += m_growth * m_step * 2;
		}




	}

}

DirectX::SimpleMath::Vector4 MenuHandler::interpolateColor(float progress, const DirectX::SimpleMath::Vector4 & from, const DirectX::SimpleMath::Vector4 & to)
{
	progress = min(max(progress,0),1.000f);
	Vector4 diff(to-from);

	Vector4 res = from + diff * progress;
	res.w = 1;
	return res;
}

void MenuHandler::setColor(const DirectX::SimpleMath::Vector4 & color) {


}

