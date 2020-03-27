/*
Charlie Duff
D00183790
added full file
*/
#include "Island.hpp"
#include "ResourceHolder.hpp"
#include "CategoryID.hpp"
#include "DataTables.hpp"


//added island vector to store island sizes, textureIDs, and categoryIDs
namespace
{
	const std::vector<IslandData> Table = initializeIslandData();
}


Island::Island(IslandID type, const TextureHolder& textures) :
	Entity(1000)
	, mType(type)
	, mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
{
	setHitpoints(1);
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

sf::FloatRect Island::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Island::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

unsigned int Island::getCategory() const
{
	if (mType == IslandID::Island)
		return static_cast<int>(CategoryID::Island1);
	else
		return static_cast<int>(CategoryID::Island1);
}


