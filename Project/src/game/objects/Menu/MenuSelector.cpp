#include "MenuSelector.h"

using namespace DirectX::SimpleMath;

MenuSelector::MenuSelector() {
	m_position = Vector3(0, 0, 0);
	m_growth = Vector3(0, 0, 0);
	m_facing = Vector3(0, 0, 0);
	m_stat = false;
	m_statShow = 1;
	m_selector = 0;
	m_size = 1;
	m_targetSize = 1.5;
	m_offset = 0.12f;
	m_onColor = Vector4(1, 1, 1, 1);
	m_offColor = Vector4(0.5f, 0.5f, 0.5f, 1);
}


MenuSelector::~MenuSelector() {
	for (size_t i = 0; i < m_options.size(); i++) {
		Memory::safeDelete(m_options[i]);
	}

}

void MenuSelector::draw() {
	if (!m_stat) {
		for (size_t i = 0; i < m_options.size(); i++) {
			m_options[i]->draw();
		}

	}
	if (m_stat) {
		m_options[m_selector]->draw();
	}
	
}

void MenuSelector::update() {

}

void MenuSelector::addOption(std::string text) {
	MenuText* temp = new MenuText(text);
	if (m_options.size() == 0) {
		temp->setColor(m_onColor);
	}
	else
		temp->setColor(m_offColor);
	
	m_options.push_back(temp);
	updatePositions();
}

size_t MenuSelector::getOption() {


	return m_selector;
}

Object * MenuSelector::getTarget() {
	if(m_options.size() > 0)
		return m_options[m_selector];
	else 
		return nullptr;
}

void MenuSelector::setSelector(size_t selection) {
	if(selection < m_options.size())
		m_selector = selection;
}

void MenuSelector::setStaticSelection(bool stat, size_t show) {
	if (stat && show * 2 + 1 <= m_options.size()) {
		m_stat = stat;
		m_statShow = show;
	}
	else if(!m_stat)
		m_stat = stat;
	updatePositions();
}

void MenuSelector::setPosition(const DirectX::SimpleMath::Vector3 & position) {
	m_position = position;
	Object::setPosition(position);
	updatePositions();
}

void MenuSelector::setgrowth(const DirectX::SimpleMath::Vector3 & growth) {
	m_growth = growth;
	updatePositions();
}

void MenuSelector::setDirection(const DirectX::SimpleMath::Vector3 & direction) {
	m_facing = direction;
	updatePositions();
}

void MenuSelector::setSize(float size) {
	m_size = size;
	updatePositions();
}

void MenuSelector::setActiveOption(size_t option) {
	if (option < m_options.size()) {
		int rounds = 0;

		while (option != m_selector && rounds < m_options.size()+1) {
			next();
			rounds++;
		}


	}
}

void MenuSelector::next() {

	if (m_options.size() > 0) {
		m_options[m_selector]->setColor(m_offColor);
		m_options[m_selector]->setSize(m_size*0.5f);
		m_selector++;
		if (m_selector == m_options.size())
			m_selector = 0;
		m_options[m_selector]->setColor(m_onColor);
		m_options[m_selector]->setSize(m_size*0.5f*m_targetSize);
	}


}

void MenuSelector::back() {

	if (m_options.size() > 0) {
		m_options[m_selector]->setColor(m_offColor);
		m_options[m_selector]->setSize(m_size*0.5f);

		if (m_selector == 0) {
			m_selector = m_options.size()-1;

		}
		else 
			m_selector--;

		m_options[m_selector]->setColor(m_onColor);
		m_options[m_selector]->setSize(m_size*0.5f*m_targetSize);
	}
}

void MenuSelector::updatePositions() {
	Vector3 middle = m_position;
	Vector3 shift = m_growth * m_size * 0.85f;
	Vector3 extraWidth(0, 0, 0);
	
	if (m_stat) {

		for (size_t i = 0; i < m_options.size(); i++) {
			m_options[i]->setPosition(middle);
			m_options[i]->setFacingDirection(m_facing);
			m_options[i]->setDirection(m_growth);
			m_options[i]->setSize(m_size*0.5f);
		}
		m_options[m_selector]->setPosition(middle);
		m_options[m_selector]->setSize(m_size*0.5f*m_targetSize);

		//for (size_t i = 1; i < m_statShow+1; i++) {
		//
		//
		//	m_options[m_selector + (i)]->setPosition(middle + shift * ((float)i - ((float)m_options.size()-1)//*0.5f));
		//	m_options[m_selector + (i)]->setFacingDirection(m_facing);
		//	m_options[m_selector + (i)]->setDirection(m_growth);
		//	m_options[m_selector + (i)]->setSize(m_size*0.5f);
		//
		//	m_options[m_selector - (i)]->setPosition(middle + shift * ((float)i - ((float)m_options.size() - 1)//*0.5f));
		//	m_options[m_selector - (i)]->setFacingDirection(m_facing);
		//	m_options[m_selector - (i)]->setDirection(m_growth);
		//	m_options[m_selector - (i)]->setSize(m_size*0.5f);
		//}
	}
	else {




		for (size_t i = 0; i < m_options.size(); i++) {
			//Vector3 _pos = middle + step*((float)i-(float)m_options.size())

			m_options[i]->setPosition(middle + shift * ((float)i - ((float)m_options.size() - 1)*0.5f));
			m_options[i]->setFacingDirection(m_facing);
			m_options[i]->setDirection(m_growth);
			if(i == m_selector)
				m_options[i]->setSize(m_size*0.5f*m_targetSize);
			else
				m_options[i]->setSize(m_size*0.5f);
		}


	}




}
