#include "MenuText.h"

using namespace DirectX::SimpleMath;

MenuText::MenuText(std::string text) {
	size_t size = text.size();
	
	setText(text);
	m_position = Vector3(0.0f, 0.0f, 0.0f);
	m_growth = Vector3(1.0f, 0.0f, 0.0f);
	m_facing = Vector3(0.0f, 0.0f, -1.0f);
	m_size = 1;
	updateLetterPosition();

	m_crazy = false;
	m_crazyAcc = 0;
	m_crazyThresh = 1.0f / 3.0f;
}

MenuText::MenuText(size_t size) {
	if (size > 10)
		size = 10;
	for (size_t i = 0; i < size; i++) {
		TextLetter* temp = new TextLetter();
		m_letters.push_back(temp);
	}
	m_position = Vector3(0.0f, 0.0f, 0.0f);
	m_growth = Vector3(1.0f, 0.0f, 0.0f);
	m_facing = Vector3(0.0f, 0.0f, -1.0f);
	m_size = 1;
	updateLetterPosition();
}


MenuText::~MenuText() {
	for (size_t i = 0; i < m_letters.size(); i++) {
		Memory::safeDelete(m_letters[i]);
	}
}

void MenuText::draw() {
	for (size_t i = 0; i < m_letters.size(); i++) {
		m_letters[i]->draw();
	}
}

void MenuText::setPosition(const DirectX::SimpleMath::Vector3& pos) {
	m_position = pos;
	Object::setPosition(pos);
	updateLetterPosition();
}

void MenuText::setDirection(const DirectX::SimpleMath::Vector3& dir) {
	m_growth = dir;
	updateLetterPosition();
}

void MenuText::setFacingDirection(const DirectX::SimpleMath::Vector3& norm) {
	m_facing = norm;
	updateLetterPosition();
}

void MenuText::setSize(float size) {
	m_size = size;
	updateLetterPosition();
}

void MenuText::setLetter(size_t index, size_t letter) {
	
	if (index < m_letters.size()) {
		m_letters[index]->m_letter = letter;
		if (letter > 0) {
			m_letters[index]->setModel(Application::getInstance()->getResourceManager().getFBXModel("letters/Alphabet-" + std::to_string(letter)).getModel());
		}
	}
	
	
}

void MenuText::changeLetter(size_t index, bool direction) {
	if (index < m_letters.size()) {
		if (direction) {
			m_letters[index]->m_letter++;
			if (m_letters[index]->m_letter > 36) {
				m_letters[index]->m_letter = 1;
			}

		}
		else {
			if (m_letters[index]->m_letter == 1) {
				m_letters[index]->m_letter = 36;
			}

		}
		m_letters[index]->setModel(Application::getInstance()->getResourceManager().getFBXModel("letters/Alphabet-" + std::to_string(m_letters[index]->m_letter)).getModel());
	}
}

void MenuText::setColor(const DirectX::SimpleMath::Vector4 & color, int index) {
	if (index == -1) {
		for (size_t i = 0; i < m_letters.size(); i++) {
			m_letters[i]->setLightColor(color);
		}
	}
	else if((size_t)index < m_letters.size() && index >= 0){
		m_letters[index]->setLightColor(color);
	}
	
}

void MenuText::setText(std::string text) {
	if (m_letters.size() > 0) {
		for (size_t i = 0; i < m_letters.size(); i++) {
			Memory::safeDelete(m_letters[i]);
		}
		m_letters.clear();
	}

	size_t size = text.size();
	for (size_t i = 0; i < size; i++) {
		
		TextLetter* temp = new TextLetter(text[i]);
		m_letters.push_back(temp);
	}
	updateLetterPosition();

}

size_t MenuText::getLetterNr() {
	return m_letters.size();
}

void MenuText::updateLetterPosition() {
	static float spacing = 0.2f;
	

	for (size_t i = 0; i < m_letters.size(); i++) {
		Vector3 shifting = m_growth * (float)i;
		Vector3 startPos = (float)(m_letters.size()-1)*0.5f*m_growth;

		Vector3 _pos = (shifting - startPos)*(spacing*m_size*0.5f) + m_position;

		m_letters[i]->setPosition(_pos);
		
		m_letters[i]->getTransform().setRotations(Vector3(0.0f, -atan2(m_facing.z, m_facing.x)+1.57f,0.0f));

		if (m_letters[i]->m_letter == 62) {
			m_letters[i]->getTransform().setRotations(Vector3(0.0f, -atan2(m_facing.z, m_facing.x)- 1.57f, 0.0f));
		}
		m_letters[i]->getTransform().setScale(m_size);
	}

}



MenuText::TextLetter::TextLetter(char _letter) {
	if (_letter == ' ') {
		m_letter = 0;
	}
	else if(_letter > 64 && _letter < 91) {
		m_letter = _letter - 64;
		setModel(Application::getInstance()->getResourceManager().getFBXModel("letters/Alphabet-" + std::to_string(m_letter)).getModel());
	}
	else if (_letter > 96 && _letter < 123) {
		m_letter = _letter - 96;
		setModel(Application::getInstance()->getResourceManager().getFBXModel("letters/Alphabet-" + std::to_string(m_letter)).getModel());
	}
	else if (_letter > 47 && _letter < 58) {
		m_letter = _letter - 21;
		setModel(Application::getInstance()->getResourceManager().getFBXModel("letters/Alphabet-" + std::to_string(m_letter)).getModel());
	}
	//<   >
	else if (_letter == 60 || _letter == 62) {
		m_letter = _letter;
		setModel(Application::getInstance()->getResourceManager().getFBXModel("arrow").getModel());
	}
	
	


}

MenuText::TextLetter::~TextLetter() {

}

void MenuText::TextLetter::draw() {
	if (m_letter > 0) {
		model->setTransform(&this->getTransform());
		model->getMaterial()->setColor(this->lightColor);
		getModel()->draw();


	}
}
