/*
Charlie Duff
D00183790
added full file
*/
#pragma once
#include "Entity.hpp"
#include "IslandID.hpp"
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"

class Island : public Entity
{
public:
	Island(IslandID type, const TextureHolder& textures);
	virtual sf::FloatRect getBoundingRect() const;
	virtual unsigned int	getCategory() const;
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	

private:
	//Different ship categories have different gun textures
	IslandID mType;
	sf::Sprite mSprite;

};