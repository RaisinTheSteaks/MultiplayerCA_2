/*
Charlie Duff
D00183790
*/
#pragma once
#include "Entity.hpp"
#include "ShipID.hpp"
#include "ResourceIdentifiers.hpp"

class Gun : public Entity
{
public:
	Gun(ShipID type,const TextureHolder& textures);
	
	
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	/*
	Next iteration going to add the fire code into here so that the direction of the guns, not the direction of the ship determine the direction of the bullet
	*/

private:
	//Different ship categories have different gun textures
	ShipID mType;
	sf::Sprite mSprite;

};