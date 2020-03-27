/*
Charlie Duff
D00183790
*/
#pragma once

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "ProjectileID.hpp"
#include "CommandQueue.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Projectile : public Entity
{
public:
	Projectile(ProjectileID type, const TextureHolder& textures);
	Projectile(ProjectileID type, const TextureHolder& textures, float rotation, sf::Uint8 firerID);

	void					guideTowards(sf::Vector2f position);
	bool					isGuided() const;

	virtual unsigned int	getCategory() const;
	virtual sf::FloatRect	getBoundingRect() const;
	float					getMaxSpeed() const;
	int						getDamage() const;
	void					setMRotation(float rotation);
	float					getMRotation();
	sf::Uint8				getFirerID();

private:
	virtual void			updateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


private:
	ProjectileID			mType;
	sf::Sprite				mSprite;
	sf::Vector2f			mTargetDirection;
	float					mRotation;
	sf::Uint8				mFirer;
};