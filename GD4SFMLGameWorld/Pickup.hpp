/*
Charlie Duff
D00183790
*/
#pragma once
#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "PickUpID.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Ship;

class Pickup : public Entity
{
public:
	Pickup(PickupID type, const TextureHolder& textures);

	virtual unsigned int	getCategory() const;
	virtual sf::FloatRect	getBoundingRect() const;

	void 					apply(Ship& player) const;


protected:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


private:
	PickupID 					mType;
	sf::Sprite				mSprite;
};
