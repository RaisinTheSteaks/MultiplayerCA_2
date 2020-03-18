/*
Charlie Duff
D00183790
*/
#include "Ship.hpp"
#include "ResourceHolder.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include "SFML/Graphics/RenderStates.hpp"
#include "TextureID.hpp"
#include "ShipID.hpp"
#include "ProjectileID.hpp"
#include "PickupID.hpp"

#include <cmath>

namespace
{
	const std::vector<ShipData> Table = initializeShipData();
}

//To be used in later iterations to change the texture of the ship based on type chosen by player

//TextureID toTextureID(ShipID type)
//{
//	switch (type)
//	{
//	case ShipID::Battleship:
//		return TextureID::Battleship;
//
//	case ShipID::Raptor:
//		return TextureID::Raptor;
//	}
//	return TextureID::Battleship;
//}

Ship::Ship(ShipID type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[static_cast<int>(type)].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
	, mExplosion(textures.get(TextureID::Explosion))
	, mFireCommand()
	, mMissileCommand()
	, mFireCountdown(sf::Time::Zero)
	, mIsFiring(false)
	, mIsLaunchingMissile(false)
	, mShowExplosion(true)
	, mPlayedExplosionSound(false)
	, mSpawnedPickup(false)
	, mIsMarkedForRemoval(false)
	, mFireRateLevel(1)
	, mSpreadLevel(1)
	, mMissileAmmo(2)
	, mDropPickupCommand()
	, mTravelledDistance(0.f)
	, mDirectionIndex(0)
	, mHealthDisplay(nullptr)
	, mMissileDisplay(nullptr)
	, mGuns()
	, mDirectionVec(0.f,0.f)	//Added to store direction
	, mForward(textures.get(Table[static_cast<int>(mType)].forward))
{


	mForward.setFrameSize(sf::Vector2i(80,80));
	mForward.setNumFrames(3);
	mForward.setDuration(sf::seconds(0.5));
	mForward.setRepeating(true);

	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	centreOrigin(mForward);
	centreOrigin(mSprite);
	centreOrigin(mExplosion);

	mFireCommand.category = static_cast<int>(CategoryID::SceneAirLayer);
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = static_cast<int>(CategoryID::SceneAirLayer);
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createProjectile(node, ProjectileID::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = static_cast<int>(CategoryID::SceneAirLayer);
	mDropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == (static_cast<int>(CategoryID::PlayerShip)))
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	if (getCategory() == (static_cast<int>(CategoryID::Player2Ship)))
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}


void Ship::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else if (getVelocity().y != 0 || getVelocity().x != 0)
		target.draw(mForward, states);
	else
		target.draw(mSprite, states);
}

void Ship::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		mExplosion.update(dt);
		//mIsMarkedForRemoval = true;
		//Play explosion sound
		if (!mPlayedExplosionSound)
		{
			SoundEffectID soundEffect = (randomInt(2) == 0) ? SoundEffectID::Explosion1 : SoundEffectID::Explosion2;
			playerLocalSound(commands, soundEffect);

			mPlayedExplosionSound = true;
		}
		return;
	}
	else if (getVelocity().y != 0 || getVelocity().x != 0)
	{
		mForward.update(dt);
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);

	// Update texts
	updateTexts();
	updateRollAnimation();
}

ShipID Ship::getType()
{
	return mType;
}

//Used to give the Ship access to the guns methods.
//To replace magic numbers with better system soon
void Ship::addGun(Gun* gun)
{
	mGuns[0] = gun;
}

unsigned int Ship::getCategory() const
{
	if (isAllied())
		if (mType == ShipID::Battleship)
		{
			return static_cast<int>(CategoryID::PlayerShip);
		}

		else
		{
			return static_cast<int>(CategoryID::Player2Ship);
		}
	else
		return static_cast<int>(CategoryID::EnemyShip);
}

sf::FloatRect Ship::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}


bool Ship::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

bool Ship::isAllied() const
{
	return mType == ShipID::Battleship || mType == ShipID::Battleship2;
}

float Ship::getMaxSpeed() const
{
	return Table[static_cast<int>(mType)].speed;
}

float Ship::getTurnSpeed() const
{
	return Table[static_cast<int>(mType)].turnSpeed;
}

void Ship::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Ship::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Ship::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Ship::playerLocalSound(CommandQueue& commands, SoundEffectID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = static_cast<int>(CategoryID::SoundEffect);
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition](SoundNode& node, sf::Time)
	{
		node.playSound(effect, worldPosition);
	});
	commands.push(command);
}

sf::Vector2f Ship::getDirectionVec()
{
	return mDirectionVec;
}

void Ship::setDirectionVec(sf::Vector2f dir)
{
	mDirectionVec = dir;
}

void Ship::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[static_cast<int>(mType)].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Ship::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Ship::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[static_cast<int>(mType)].directions;
	if (!directions.empty())
	{
		// Moved long enough in current direction: Change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Compute velocity from direction
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

void Ship::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup)
		commands.push(mDropPickupCommand);
	mSpawnedPickup = true;
}

void Ship::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		/*
		Joshua Corcoran
		D00190830
		Currently, only 1 kind of player ship with only 1 type of gun
		*/
		playerLocalSound(commands, isAllied() ? SoundEffectID::AlliedGunfire : SoundEffectID::EnemyGunfire);
		mFireCountdown += Table[static_cast<int>(mType)].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		playerLocalSound(commands, SoundEffectID::LaunchMissile);
		mIsLaunchingMissile = false;
	}
}

void Ship::createBullets(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileID type = isAllied() ? ProjectileID::AlliedBullet : ProjectileID::EnemyBullet;

	switch (mSpreadLevel)
	{
	case 1:
		createProjectile(node, type, 0.0f, 0.6f, textures);
		break;

	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, +0.33f, 0.33f, textures);
		break;

	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, 0.0f, 0.5f, textures);
		createProjectile(node, type, +0.5f, 0.33f, textures);
		break;
	}
}

void Ship::createProjectile(SceneNode& node, ProjectileID type, float xOffset, float yOffset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures,getRotation()));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(1,1);
	/*
		Joshua Corcoran
		D00190830
		_________
		Trying to get bullets to turn firing direction with the ship
	*/
	float pi = 3.14159265f;
	velocity.y *= cos(projectile->getMRotation()*pi / 180) * -1;
	velocity.x *= sin(projectile->getMRotation()*pi / 180) * 1;

	projectile->setPosition(getWorldPosition() +velocity*(offset.x+offset.y+10));
	projectile->setRotation(projectile->getMRotation());
	projectile->setVelocity(velocity*getMaxSpeed());
	node.attachChild(std::move(projectile));
}

void Ship::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupID>(randomInt(static_cast<int>(PickupID::TypeCount)));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Ship::updateTexts()
{
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

	if (mMissileDisplay)
	{
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}

void Ship::updateRollAnimation()
{
	if (Table[static_cast<int>(mType)].hasRollAnimation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(mType)].textureRect;

		// Roll left: Texture rect offset once
		if (getVelocity().x < 0.f)
			textureRect.left += textureRect.width;

		// Roll right: Texture rect offset twice
		else if (getVelocity().x > 0.f)
			textureRect.left += 2 * textureRect.width;

		mSprite.setTextureRect(textureRect);
	}
}
