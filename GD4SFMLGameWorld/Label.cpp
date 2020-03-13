/*
Charlie Duff
D00183790
*/
#include "Label.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

GUI::Label::Label(const std::string& text, const FontHolder& fonts)
	:mText(text, fonts.get(FontID::Main), 16)
{
}

bool GUI::Label::isSelectable() const
{
	return false;
}

void GUI::Label::setText(const std::string& text)
{
	mText.setString(text);
}

void GUI::Label::handleEvent(const sf::Event& event)
{
}

void GUI::Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mText, states);
}
