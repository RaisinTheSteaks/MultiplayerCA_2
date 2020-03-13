/*
Joshua Corcoran 
D00190830 
Added full file
*/
/*
Charlie Duff
D00183790
*/

#include "Gun.hpp"
#include "ResourceHolder.hpp"
#include "CategoryID.hpp"

/*
toTextureID takes in the type of ship being constructed and attaches the appropriate gun.
Currently is only called to build battleships.
*/

TextureID  toTextureID(ShipID type)
{

	switch (type)
	{
	case ShipID::Battleship:
		return TextureID::BattleshipGun;
	case ShipID::Cruiser:
		return TextureID::CruiserGun;
	case ShipID::Submarine:
		return TextureID::SubmarineGun;
	default:
		return TextureID::BattleshipGun;
	}
}

Gun::Gun(ShipID type, const TextureHolder& textures) :
	Entity(1000)
	,mType(type)
	, mSprite(textures.get(toTextureID(type)))
{
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Gun::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}
