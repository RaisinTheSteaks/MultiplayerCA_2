/*
Charlie Duff
D00183790
Joshua Corcoran
D00190830
*/
#include "World.hpp"
#include "ParticleID.hpp"
#include "ParticleNode.hpp"
#include "NetworkNode.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <iostream>

#include "EmitterNode.hpp"

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mCamera(outputTarget.getDefaultView())
	, mFonts(fonts)
	, mSounds(sounds)
	, mTextures()
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds(0.f, 0.f, mCamera.getSize().x, 5000.f)
	, mSpawnPosition(mCamera.getSize().x / 2.f, mWorldBounds.height - mCamera.getSize().y / 2.f)
	, mScrollSpeed(-50.f)
	, mPlayerShip()
	, mPlayerGuns()
	, mNetworkedWorld(networked)
	, mIsland()
	, mEnemySpawnPoints()
	, mActiveEnemies()
	, mSpawnPositions()
	, mNetworkNode(nullptr)
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);
	loadTextures();
	buildScene();

	// Prepare the view
	mCamera.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	//mCamera.move(0.f, mScrollSpeed * dt.asSeconds());
	
	//both players velocity is reset
	for (Ship* s : mPlayerShip)
	{
		s->setVelocity(0.f, 0.f);
	}
	// Setup commands to destroy entities, and guide missiles
	destroyEntitiesOutsideView();
	guideMissiles();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	// Collision detection and response (may destroy entities)
	handleCollisions();

	auto firstToRemove = std::remove_if(mPlayerShip.begin(), mPlayerShip.end(), std::mem_fn(&Ship::isMarkedForRemoval));
	mPlayerShip.erase(firstToRemove, mPlayerShip.end());

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();


	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();

	updateSounds();
}

void World::draw()
{
	if (PostEffect::isSupported())
	{
		mSceneTexture.clear();
		mSceneTexture.setView(mCamera);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		//mBloomEffect.apply(mSceneTexture, mTarget);
		mNewEffect.apply(mSceneTexture, mTarget);
	}
	else
	{
		mTarget.setView(mCamera);
		mTarget.draw(mSceneGraph);
	}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

Ship* World::getShip(int identifier) const
{
	for (Ship* s : mPlayerShip)
	{
		if (s->getIdentifier() == identifier)
			return s;
	}
	return nullptr;
}

void World::setWorldHeight(float height)
{
	mWorldBounds.height = height;
}

Ship* World::addShip(int identifier)
{
	std::unique_ptr<Ship> player(new Ship(ShipID::Battleship, mTextures, mFonts));

	//TODO - custom spawning points
	player->setPosition(mCamera.getCenter());
	player->setIdentifier(identifier);

	mPlayerShip.push_back(player.get());
	mSceneLayers[static_cast<int>(LayerID::UpperAir)]->attachChild(std::move(player));
	return mPlayerShip.back();
}

void World::removeShip(int identifier)
{
	Ship* ship = getShip(identifier);
	if (ship)
	{
		ship->destroy();
		mPlayerShip.erase(std::find(mPlayerShip.begin(), mPlayerShip.end(), ship));
	}
}

void World::createPickup(sf::Vector2f position, PickupID type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, mTextures));
	pickup->setPosition(position);
	pickup->setVelocity(0.f, 1.f);
	mSceneLayers[static_cast<int>(LayerID::UpperAir)]->attachChild(std::move(pickup));
}

bool World::hasAlivePlayer() const
{
	
	return mPlayerShip.size() > 0;
}



bool World::hasPlayerReachedEnd() const
{
	if (Ship* ship = getShip(1))
	{
		return !mWorldBounds.contains(ship->getPosition());
	}
	else
	{
		return false;
	}
}

void World::updateSounds()
{
	sf::Vector2f listenerPosition;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (mPlayerShip.empty())
	{
		listenerPosition = mCamera.getCenter();
	}

	// 1 or more players -> mean position between all aircrafts
	else
	{
		for (Ship* ship : mPlayerShip)
			listenerPosition += ship->getWorldPosition();

		listenerPosition /= static_cast<float>(mPlayerShip.size());
	}
	//Set the listener to the player position
	mSounds.setListenPosition(listenerPosition);
	//Remove unused sounds
	mSounds.removeStoppedSounds();

}

bool World::pollGameAction(Action& out)
{
	return mNetworkNode->pollGameAction(out);
}

void World::loadTextures()
{
	mTextures.load(TextureID::Entities, "Media/Textures/Entities.png");
	mTextures.load(TextureID::Jungle, "Media/Textures/Jungle.png");
	mTextures.load(TextureID::Explosion, "Media/Textures/Explosion.png");
	mTextures.load(TextureID::Particle, "Media/Textures/Particle.png");
	mTextures.load(TextureID::FinishLine, "Media/Textures/FinishLine.png");

	//Assets sourced from:
	//https://opengameart.org/content/water
	mTextures.load(TextureID::Ocean, "Media/Textures/Ocean/water1.png");
	//https://opengameart.org/content/sea-warfare-set-ships-and-more
	mTextures.load(TextureID::Battleship, "Media/Textures/Battleship/ShipBattleshipHullTest.png");
	mTextures.load(TextureID::BattleshipGun, "Media/Textures/Battleship/WeaponBattleshipStandardGun.png");
	mTextures.load(TextureID::Island, "Media/Textures/Island/Island.png");
	mTextures.load(TextureID::Arrows, "Media/Textures/Arrows.png");

	mTextures.load(TextureID::FrigateForward, "Media/Textures/RomanShips/forwardFrigate.png");
	mTextures.load(TextureID::ShipForward, "Media/Textures/RomanShips/ShipSpriteSheet.png");
}

bool matchesCategories(SceneNode::Pair& colliders, CategoryID type1, CategoryID type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (((static_cast<int>(type1))& category1) && ((static_cast<int>(type2))& category2))
	{
		return true;
	}
	else if (((static_cast<int>(type1))& category2) && ((static_cast<int>(type2))& category1))
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	for (SceneNode::Pair pair : collisionPairs)
	{
		if (matchesCategories(pair, CategoryID::PlayerShip, CategoryID::EnemyShip))
		{
			auto& player = static_cast<Ship&>(*pair.first);
			auto& enemy = static_cast<Ship&>(*pair.second);

			// Collision: Player damage = enemy's remaining HP
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}

		else if (matchesCategories(pair, CategoryID::PlayerShip, CategoryID::Pickup))
		{
			auto& player = static_cast<Ship&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			player.playerLocalSound(mCommandQueue, SoundEffectID::CollectPickup);
			pickup.destroy();
		}

		else if (matchesCategories(pair, CategoryID::EnemyShip, CategoryID::AlliedProjectile)
			|| matchesCategories(pair, CategoryID::PlayerShip, CategoryID::EnemyProjectile))
		{
			auto& ship = static_cast<Ship&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			// Apply projectile damage to Ship, destroy projectile
			ship.damage(projectile.getDamage());

			std::unique_ptr<EmitterNode> smoke(new EmitterNode(ParticleID::Smoke));
			smoke->setPosition(0.f, ship.getBoundingRect().height / 2.f);
			ship.attachChild(std::move(smoke));


			projectile.destroy();
		}

		//both player ships can now damage each other, Projectile draw distance from ship was increased
		else if (matchesCategories(pair, CategoryID::PlayerShip, CategoryID::AlliedProjectile) || matchesCategories(pair, CategoryID::Player2Ship, CategoryID::AlliedProjectile))
		{
			auto& ship = static_cast<Ship&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			/*
			[TODO] ADD SHADER STUFF
			*/

			// Apply projectile damage to Ship, destroy projectile
			ship.damage(projectile.getDamage());
			projectile.destroy();
		}

		//player to island collision
		//When either player1/2 collides with island, player1/2 is destroyed
		else if (matchesCategories(pair, CategoryID::PlayerShip, CategoryID::Island1) || matchesCategories(pair, CategoryID::Player2Ship, CategoryID::Island1))
		{
			auto& ship = static_cast<Ship&>(*pair.first);
			auto& island = static_cast<Island&>(*pair.second);
			
			//island.destroy();
			//plays sound for when player collides with island
			ship.playerLocalSound(mCommandQueue, SoundEffectID::Scream);
			std::cout << "Hit Island!" << std::endl;
			ship.damage(100);
		}
	
		//player to player collision
		// each player takes 
		else if (matchesCategories(pair, CategoryID::PlayerShip, CategoryID::Player2Ship))
		{
			auto& player = static_cast<Ship&>(*pair.first);
			auto& player2 = static_cast<Ship&>(*pair.second);

			// Collision: each player takes 10 damage
			player.damage(1.f);
			player2.damage(1.f);
		}
	}
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(LayerID::LayerCount); ++i)
	{
		CategoryID category = (i == (static_cast<int>(LayerID::LowerAir))) ? CategoryID::SceneAirLayer : CategoryID::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
#pragma region Background

	sf::Texture& texture = mTextures.get(TextureID::Ocean);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);

	//add islands
	sf::Texture& island = mTextures.get(TextureID::Island);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[static_cast<int>(LayerID::Background)]->attachChild(std::move(backgroundSprite));
#pragma endregion

	////Add the finish line to the scene
	//sf::Texture& finishTexture = mTextures.get(TextureID::FinishLine);
	//std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	//finishSprite->setPosition(0.f, -76.f);
	//mSceneLayers[static_cast<int>(LayerID::Background)]->attachChild(std::move(finishSprite));

	//Add particle nodes for smoke and propellant
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(ParticleID::Smoke, mTextures));
	mSceneLayers[static_cast<int>(LayerID::LowerAir)]->attachChild(std::move(smokeNode));

	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(ParticleID::Propellant, mTextures));
	mSceneLayers[static_cast<int>(LayerID::LowerAir)]->attachChild(std::move(propellantNode));

	//Add the sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
	mSceneGraph.attachChild(std::move(soundNode));

	// Add network node, if necessary
	if (mNetworkedWorld)
	{
		std::unique_ptr<NetworkNode> networkNode(new NetworkNode());
		mNetworkNode = networkNode.get();
		mSceneGraph.attachChild(std::move(networkNode));
	}

	//Spawn Points
	sf::Vector2f spawnPoint1(50.f, 50.f);
	sf::Vector2f spawnPoint2(600.f, 100.f);
	mSpawnPositions[0] = spawnPoint1;
	mSpawnPositions[1] = spawnPoint2;

	// Add player's Ship
	/*
	Player Ship is added to the lower air layer. All boats and projectiles will be added here.
	Forward Gun is a child of the player ship so it moves with the ship.
	Forward Gun gets drawn on top of the ship.

	*/
	/*std::unique_ptr<Ship> player(new Ship(ShipID::Battleship, mTextures, mFonts));
	mPlayerShip = player.get();
	mPlayerShip->setPosition(mSpawnPositions[0]);
	mPlayerShip->setRotation(180.f);
	mSceneLayers[static_cast<int>(LayerID::LowerAir)]->attachChild(std::move(player));*/

#pragma region Josh Code
	for (Ship* ship : mPlayerShip)
	{
		std::unique_ptr<Gun> player1ForwardGun(new Gun(ship->getType(), mTextures));
		mPlayerGuns[0] = player1ForwardGun.get();
		sf::Vector2f offset(0.f, -35.f);
		mPlayerGuns[0]->setPosition(offset);
		ship->attachChild(std::move(player1ForwardGun));

		ship->addGun(mPlayerGuns[0]);
	}
	
#pragma endregion


	// Add player2's Ship
	/*std::unique_ptr<Ship> player2(new Ship(ShipID::Battleship2, mTextures, mFonts));
	mPlayerShip2 = player2.get();
	mPlayerShip2->setPosition(mSpawnPositions[1]);
	mPlayerShip2->setRotation(180.f);
	mSceneLayers[static_cast<int>(LayerID::LowerAir)]->attachChild(std::move(player2));*/
	
	
	//adding island(s) 
	// will add collision later for islands
	std::unique_ptr<Island> Island1(new Island(IslandID::Island, mTextures));
	mIsland[0] = Island1.get();
	mIsland[0]->setPosition(mCamera.getSize().x / 5.f, mWorldBounds.height - mCamera.getSize().y / 1.5f);
	mSceneLayers[static_cast<int>(LayerID::UpperAir)]->attachChild(std::move(Island1));

	std::unique_ptr<Island> Island2(new Island(IslandID::Island, mTextures));
	mIsland[1] = Island2.get();
	mIsland[1]->setPosition(mCamera.getSize().x / 4.f, mWorldBounds.height - mCamera.getSize().y / 3.f);
	mSceneLayers[static_cast<int>(LayerID::UpperAir)]->attachChild(std::move(Island2));

	std::unique_ptr<Island> Island3(new Island(IslandID::Island, mTextures));
	mIsland[2] = Island3.get();
	mIsland[2]->setPosition(mCamera.getSize().x / 1.5f, mWorldBounds.height - mCamera.getSize().y / 1.7f);
	mSceneLayers[static_cast<int>(LayerID::UpperAir)]->attachChild(std::move(Island3));


	//addEnemies();
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	for (Ship* ship : mPlayerShip)
	{
		sf::Vector2f position = ship->getPosition();
		position.x = std::max(position.x, viewBounds.left + borderDistance);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
		position.y = std::max(position.y, viewBounds.top + borderDistance);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
		ship->setPosition(position);
	}
	

	
}

void World::adaptPlayerVelocity()
{
	for (Ship* ship : mPlayerShip)
	{
		sf::Vector2f velocity = ship->getVelocity();

		// If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
			ship->setVelocity(velocity / std::sqrt(2.f));



		// Add scrolling velocity
		//mPlayerShip->accelerate(mPlayerShip->getVelocity());
	}
	
}



void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = static_cast<int>(CategoryID::Projectile) | static_cast<int>(CategoryID::EnemyShip);
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
	{
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
			e.destroy();
	});

	mCommandQueue.push(command);
}

void World::guideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = static_cast<int>(CategoryID::EnemyShip);
	enemyCollector.action = derivedAction<Ship>([this](Ship& enemy, sf::Time)
	{
		if (!enemy.isDestroyed())
			mActiveEnemies.push_back(&enemy);
	});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = static_cast<int>(CategoryID::AlliedProjectile);
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time)
	{
		// Ignore unguided bullets
		if (!missile.isGuided())
			return;

		float minDistance = std::numeric_limits<float>::max();
		Ship* closestEnemy = nullptr;

		// Find closest enemy
		for (Ship* enemy : mActiveEnemies)
		{
			float enemyDistance = distance(missile, *enemy);

			if (enemyDistance < minDistance)
			{
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}

		if (closestEnemy)
			missile.guideTowards(closestEnemy->getWorldPosition());
	});

	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mCamera.getCenter() - mCamera.getSize() / 2.f, mCamera.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}