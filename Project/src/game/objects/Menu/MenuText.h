#pragma once
#include "../common/Object.h"
class MenuText : public Object
{
public:
	MenuText(std::string text = "");
	MenuText(size_t size = 5);
	~MenuText();
	void draw();

	void setPosition(const DirectX::SimpleMath::Vector3& pos);
	void setDirection(const DirectX::SimpleMath::Vector3& dir);
	void setFacingDirection(const DirectX::SimpleMath::Vector3& norm);
	void setSize(float size);
	void setLetter(size_t index, size_t letter);
	void changeLetter(size_t index, bool direction);
	void setColor(const DirectX::SimpleMath::Vector4& color, int index = -1);
	void setText(std::string text);
	size_t getLetterNr();
	std::string getString();

	class TextLetter : public Object {
	public:
		
		TextLetter(char _letter = ' ');
		~TextLetter();
		size_t m_letter;

		void draw();
	private:
	};

private:
	std::vector<TextLetter*> m_letters;

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_growth;
	DirectX::SimpleMath::Vector3 m_facing;
	DirectX::SimpleMath::Vector3 m_angles;
	float m_size;
	bool m_crazy;
	float m_crazyAcc;
	float m_crazyThresh;

	void updateLetterPosition();
};



