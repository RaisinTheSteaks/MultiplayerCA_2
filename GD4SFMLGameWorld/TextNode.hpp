/*
Charlie Duff
D00183790
*/
#pragma once
#include "SceneNode.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/Text.hpp>

class TextNode : public SceneNode
{
public:
	/*
	Joshua Corcoran
	D00190830
	____________
	Would override this function to take in a FontID to determine which font to write this in, but not enough time to play around with it
	*/
	explicit TextNode(const FontHolder& fonts, const std::string& text);
	void setString(const std::string& text);

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	sf::Text mText;
};