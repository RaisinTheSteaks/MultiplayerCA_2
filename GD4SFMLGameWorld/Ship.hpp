/*
Charlie Duff
D00183790
*/
#pragma once
#include "Entity.hpp"
#include "ShipID.hpp"
#include "ResourceIdentifiers.hpp"
#include "CommandQueue.hpp"
#include "TextNode.hpp"
#include "Projectile.hpp"
#include "Animation.hpp"
#include <array>
//Including gun instead of forward declaring to make sure that I can access the gun's fire method
#include "Gun.hpp"

class Ship : public Entity
{
public:
	Ship(ShipID type, TextureHolder& textures, const FontHolder& fonts);
	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;

	virtual bool isMarkedForRemoval() const;

	virtual void remove();
	void disablePickups();

	void setIdentifier(int identifier);
	int getIdentifier() const;

	ShipID getType();
	void addGun(Gun* gun);

	float getMaxSpeed() const;
	float getTurnSpeed() const;
	void fire();
	void launchMissile();
	bool isAllied() const;
	void increaseFireRate();
	void increaseSpread();
	void collectMissiles(unsigned int count);

	void setTexture(TextureID texture);
	
	void playerLocalSound(CommandQueue& command, SoundEffectID effect);
	sf::Vector2f getDirectionVec();
	void setDirectionVec(sf::Vector2f dir);

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateMovementPattern(sf::Time dt);
	void updateTexts();

	void checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void createProjectile(SceneNode& node, ProjectileID type, float xOffset, float yOffset, const TextureHolder& textures) const;

	void createPickup(SceneNode& node, const TextureHolder& textures) const;
	void checkPickupDrop(CommandQueue& commands);
	void updateRollAnimation();

private:
	ShipID mType;
	sf::Sprite mSprite;
	Animation mExplosion;
	TextNode* mHealthDisplay;
	TextNode* mMissileDisplay;

	bool mIsFiring;
	bool mIsLaunchingMissile;
	int	mFireRateLevel;
	sf::Time mFireCountdown;

	bool mIsMarkedForRemoval;

	Command mFireCommand;
	Command	mMissileCommand;
	Command mDropPickupCommand;
	bool mShowExplosion;
	bool mPlayedExplosionSound;
	bool mSpawnedPickup;
	int mSpreadLevel;

	int mMissileAmmo;
	float mTravelledDistance;
	std::size_t mDirectionIndex;
	/*
		Joshua Corcoran
		D00190830
		____________
		mDirectionVec is from me trying to get the boat to 'steer' instead of slide around
		
	*/
	sf::Vector2f mDirectionVec;
	std::array<Gun*,2>mGuns;

	bool mPickupsEnabled;
	bool mExplosionBegan;
	int mIdentifier;

	Animation mForward;
	Animation mFrigateForward;
	TextureHolder* mTextureHolder;
};